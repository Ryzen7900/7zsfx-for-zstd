/*---------------------------------------------------------------------------*/
/* File:        main.cpp                                                     */
/* Created:     Fri, 29 Jul 2005 03:23:00 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Last update: Tue, 29 Dec 2015 06:34:19 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Revision:    3806                                                         */
/*---------------------------------------------------------------------------*/
/* Revision:    1798                                                         */
/* Updated:		Wed, 30 Jun 2010 09:24:36 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Description: Fix: extraction folder was not set as default				 */
/*				for external programs										 */
/*---------------------------------------------------------------------------*/
/* Revision:    1794                                                         */
/* Updated:     Sat, 26 Jun 2010 04:23:10 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Description: Execution of external programs was rewritten to support		 */
/*				'wait all childs' of the executed process					 */
/*				new execution prefix 'waitall'								 */
/*---------------------------------------------------------------------------*/
/* Revision:    1774                                                         */
/* Updated:     Sun, 06 Jun 2010 08:51:48 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Description: Add warning stuff											 */
/*				(WarningTitle, MiscFlags, -mf command line switch)			 */
/*---------------------------------------------------------------------------*/
/* Revision:    1698                                                         */
/* Updated:     Mon, 22 Mar 2010 11:26:58 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Description: New file stamp.                                              */
/*---------------------------------------------------------------------------*/
#include "stdafx.h"
#include "7zSfxModInt.h"
#include "SfxDialogs.h"
#include "ExtractEngine.h"

#define SKIP_WHITESPACES_W(str) 	while( *str != L'\0' && ((unsigned)*str) <= L' ' ) str++;

HRESULT ExtractArchive( CSfxInStream * inStream, const CSfxStringU &folderName );
bool	ReadConfig( IInStream * inStream, LPCSTR startID, LPCSTR endID, CSfxStringA &stringResult );
void	DeleteSFX( CSfxStringU moduleName );

CSfxStringU SfxMultiByteToUnicodeString( const CSfxStringA &srcString, UINT codePage );
CSfxStringA SfxUnicodeStringToMultiByte( const CSfxStringU &srcString, UINT codePage );

const UInt64 kMaxCheckStartPosition = 1 << 20;

char kSignatureConfigStart[] = ",!@Install@!UTF-8!";
char kSignatureConfigEnd[] = ",!@InstallEnd@!";


LPCWSTR	lpwszErrorTitle;
#ifdef _SFX_USE_WARNINGS
	LPCWSTR		lpwszWarningTitle;
	CSfxStringU	strWarningTitle;
#endif // _SFX_USE_WARNINGS
LPCWSTR	lpwszTitle;
LPCWSTR lpwszExtractTitle;
LPCWSTR lpwszExtractPathTitle;
LPCWSTR lpwszExtractPathText;
LPCWSTR lpwszCancelPrompt;
LPCWSTR	lpwszCancelText;
LPCWSTR	lpwszExtractDialogText;
#ifdef SFX_CRYPTO
	LPCWSTR	lpwszPasswordTitle = NULL;
	LPCWSTR	lpwszPasswordText = NULL;
#endif // SFX_CRYPTO
#ifdef _SFX_USE_VOLUME_NAME_STYLE
	int nVolumeNameStyle=0;
#endif // _SFX_USE_VOLUME_NAME_STYLE

int		GUIMode = 0;
int		GUIFlags = -1;
int		MiscFlags = 0;
int		ExtractDialogWidth = 300;
int		ExtractPathWidth = 300;
int		OverwriteMode = OVERWRITE_MODE_ALL;
int		OverwriteFlags = 0;
int		FinishMessage = -1;
#ifdef _SFX_USE_BEGINPROMPTTIMEOUT
	int		BeginPromptTimeout = 0;
#endif // _SFX_USE_BEGINPROMPTTIMEOUT
CSfxStringU	extractPath;
CSfxStringU strSfxFolder;
CSfxStringU strSfxName;
CSfxStringU	strErrorTitle;
CSfxStringU	strTitle;
#ifdef _SFX_USE_PREFIX_PLATFORM
	CSfxStringU	strOSPlatform;
#endif // _SFX_USE_PREFIX_PLATFORM

bool	fUseInstallPath;

CSfxStringU	strModulePathName;

HMODULE	hKernel32;

CObjectVector<CTextConfigPair> Variables;
bool fUseBackward = false;

typedef struct tagENVALIAS {
	int		nFolder;
	LPCWSTR	lpwszName;
} ENVALIAS, * PENVALIAS;

#ifndef CSIDL_COMMON_DOCUMENTS
	#define CSIDL_COMMON_DOCUMENTS          0x002e        // All Users\Documents
#endif

ENVALIAS EnvAliases [] = {
	{ CSIDL_COMMON_DESKTOPDIRECTORY,	L"CommonDesktop" },
	{ CSIDL_COMMON_DOCUMENTS,			L"CommonDocuments" },
	{ CSIDL_DESKTOPDIRECTORY,			L"UserDesktop" },
	{ CSIDL_PERSONAL,					L"MyDocuments" },
	{ CSIDL_PERSONAL,					L"MyDocs" }
};

void SfxInit()
{
	hKernel32 = ::LoadLibraryA("kernel32");
	InitCommonControls();
	CrcGenerateTable();

#ifdef _SFX_USE_LANG
	GetUILanguage();
#endif // _SFX_USE_LANG

	lpwszErrorTitle = GetLanguageString( STR_ERROR_TITLE );
#ifdef _SFX_USE_WARNINGS
	lpwszWarningTitle = GetLanguageString( STR_WARNING_TITLE );
#endif // _SFX_USE_WARNINGS
	lpwszTitle = GetLanguageString( STR_TITLE );
	lpwszExtractTitle = GetLanguageString( STR_EXTRACT_TITLE );
	lpwszExtractPathTitle = GetLanguageString( STR_EXTRACT_PATH_TITLE );
	lpwszExtractPathText = GetLanguageString( STR_EXTRACT_PATH_TEXT );
	lpwszCancelPrompt = GetLanguageString( STR_CANCEL_PROMPT );
	lpwszCancelText = lpwszExtractDialogText = NULL;

	WCHAR	wszPath[MAX_PATH+1];
	WCHAR	wszName[sizeof(SPECIAL_FOLDER_FORMAT)/sizeof(WCHAR)+32];
	for( int i = 0; i < 0x40; i++ )
	{
		if( SHGetSpecialFolderPath(NULL,wszPath,i,FALSE) != FALSE )
		{
			wsprintf( wszName, SPECIAL_FOLDER_FORMAT, i );
			SfxAddEnvironmentVarWithAlias( wszName, wszPath );
			for( int j = 0; j < (sizeof(EnvAliases)/sizeof(EnvAliases[0])); j++ )
			{
				if( EnvAliases[j].nFolder == i )
					SfxAddEnvironmentVar( EnvAliases[j].lpwszName, wszPath );
			}
		}
	}
}

CSfxStringU CreateTempName( LPCWSTR lpwszFormat )
{
	CSfxStringU path;
	DWORD dwSize = ::GetTempPath( 1, path.GetBuffer(2) );
	path.ReleaseBuffer( 0 );
	if( dwSize > 0 )
	{
		::GetTempPath( dwSize+1, path.GetBuffer(dwSize+1) );
		path.ReleaseBuffer();
	}

	int length = path.Len();
	for( int i = 0; i < 0xfff; i++ )
	{
		LPWSTR buf = path.GetBuffer( length+15 );
		wsprintf( buf+length, lpwszFormat, i );
		path.ReleaseBuffer();
		if( ::GetFileAttributes( path ) == (DWORD)-1 )
			break;
	}
	return path;
}

void ReplaceVariables( CSfxStringU& str )
{
	ExpandEnvironmentStrings( str );
	ReplaceWithArchivePath( str, strSfxFolder );
	ReplaceWithArchiveName( str, strSfxName );
#ifdef _SFX_USE_PREFIX_PLATFORM
	ReplaceWithPlatform( str );
#endif // _SFX_USE_PREFIX_PLATFORM
	ExpandEnvironmentStrings( str );
}

void ReplaceVariablesEx( CSfxStringU& str )
{
	ExpandEnvironmentStrings( str );
	ReplaceWithExtractPath( str, extractPath );
	ReplaceVariables( str );
}

void SetEnvironment()
{
	for( int i = 0; i < (int)Variables.Size(); i++ )
	{
		CSfxStringU val = Variables[i].String;
		ReplaceVariablesEx( val );
		::SetEnvironmentVariable( Variables[i].ID, val );
	}
}

LPCWSTR IsSubString( LPCWSTR lpwszString, LPCWSTR lpwszSubString )
{
	int nLength = lstrlenW( lpwszSubString );
	if( lstrlenW( lpwszString ) >= nLength &&
			_wcsnicmp( lpwszString, lpwszSubString, nLength ) == 0 )
		return lpwszString + nLength;
	return NULL;
}

#define	CPF_NONE		0
#define CPF_DIGIT		1
#define	CPF_NUMBER		2
#define CPF_SFX_INDEX	3

LPCWSTR CheckPrefix( LPCWSTR lpwszString, LPCWSTR lpwszPrefix, BOOL nCPFlags )
{
	LPCWSTR p = IsSubString( lpwszString, lpwszPrefix );
	if( p == NULL )
		return NULL;
	switch( nCPFlags )
	{
	case CPF_DIGIT:
		if( *p >= L'0' && *p <= L'9' && p[1] == L':' )
			return p+2;
		break;
	case CPF_NUMBER:
		if( *p >= L'0' && *p <= L'9' )
		{
			while( *p >= L'0' && *p <= L'9' ) p++;
			if( *p == L':' )
				return p+1;
		}
		break;
	case CPF_SFX_INDEX:
		if( p[1] == ':' )
		{
			if( (*p >= L'0' && *p <= L'9') ||
				(*p >= L'a' && *p <= L'z') ||
				(*p >= L'A' && *p <= L'Z') )
			{
				return p+2;
			}
		}
		break;
	default:
		if( *p == L':' )
			return p+1;
	}
	return NULL;
}


#define IsCommandLineSwitch( lpwszCommandLine, lpwszSwitch )	\
	(IsSubString( lpwszCommandLine+1, lpwszSwitch ) != NULL )


LPCWSTR UpdateFlagsCommon( LPCWSTR lpwszText, int * pnValue )
{
	while( (*lpwszText >= L'0' && *lpwszText <= L'9') || *lpwszText == L'+' || *lpwszText == L'-' )
	{
		int nValue = StringToLong( lpwszText );
		// skip to next
		while( *lpwszText == L'-' || *lpwszText == L'+' ) lpwszText++;
		while( *lpwszText >= L'0' && *lpwszText <= L'9' ) lpwszText++;
		if( nValue > 0 )
			pnValue[0] |= nValue;
		else
			pnValue[0] &= (~(0-nValue));
	}

	return lpwszText;
}

LPCWSTR UpdateGUIFlags( LPCWSTR lpwszText )
{
	if( GUIFlags == -1 || (*lpwszText != L'+' && *lpwszText != L'-') )
		GUIFlags = 0;

	return UpdateFlagsCommon( lpwszText, &GUIFlags );
}

LPCWSTR UpdateMiscFlags( LPCWSTR lpwszText )
{
	if( *lpwszText != L'+' && *lpwszText != L'-' )
		MiscFlags = 0;

	return UpdateFlagsCommon( lpwszText, &MiscFlags );
}

LPCWSTR UpdateOverwriteMode( LPCWSTR lpwszText )
{
	int nValue = OverwriteMode | OverwriteFlags;
	if( *lpwszText != L'+' && *lpwszText != L'-' )
		nValue = OVERWRITE_MODE_ALL;
	LPCWSTR lpwszRet = UpdateFlagsCommon( lpwszText, &nValue );
	if( (nValue&OVERWRITE_MODE_MASK) <= OVERWRITE_MODE_MAX )
		OverwriteMode = nValue & OVERWRITE_MODE_MASK;
	OverwriteFlags = nValue & (~OVERWRITE_MODE_MASK);
	return lpwszRet;
}

void GetConfigDirectory( CObjectVector<CTextConfigPair>& config, CSfxStringU & result )
{
	LPCWSTR lpwszValue;
	result.Empty();
	if( (lpwszValue = GetTextConfigValue( config, CFG_DIRECTORY)) != NULL )
	{
		result = lpwszValue;
		ReplaceVariablesEx( result );
		if( result[result.Len()-1] != L'\\' && result[result.Len()-1] != L'/' )
			result += L'\\';
	}
}

LPCWSTR LoadQuotedString( LPCWSTR lpwszSrc, CSfxStringU & result )
{
	if( *lpwszSrc == L'\"' )
	{
		lpwszSrc++;
		while( *lpwszSrc != L'\0' && *lpwszSrc != L'\"' )
		{
			result += *lpwszSrc;
			lpwszSrc++;
		}
		if( *lpwszSrc != L'\0' )
			lpwszSrc++;
	}
	else
	{
		while( *lpwszSrc != L'\0' && ((unsigned)*lpwszSrc) > L' ' )
		{
			result += *lpwszSrc;
			lpwszSrc++;
		}
	}

	SKIP_WHITESPACES_W( lpwszSrc );
	return lpwszSrc;
}

bool SaveConfiguration( LPCWSTR lpwszFileName, CSfxStringA & CfgData )
{
	COutFileStream	cfg;
	UInt32 writed;
	static BYTE utf8prefix[4] = { 0xEF, 0xBB, 0xBF, 0 };
	if( cfg.Create( lpwszFileName, true ) == false )
	{
		CSfxStringU filePath = lpwszFileName;
		int nPos  = GetDirectorySeparatorPos( filePath );
		if( nPos < 0 )
			return false;
		filePath.ReleaseBuffer( nPos );
		if( CreateFolderTree( (LPCWSTR)filePath ) == FALSE || cfg.Create( lpwszFileName, true ) == false )
			return false;
	}
	CSfxStringA fullCfg;
	fullCfg = (char *)utf8prefix;
	fullCfg += kSignatureConfigStart;
	fullCfg += (LPCSTR)CfgData;
	fullCfg += kSignatureConfigEnd;
	if( cfg.Write(fullCfg, fullCfg.Len(), &writed) != S_OK || writed != fullCfg.Len() )
		return false;
	return true;
}

void ReplaceVariableInShortcut( CSfxStringU& strShortcut, CSfxStringU& strVarName, CSfxStringU& strVarValue )
{
	int nVarNameLength = strVarName.Len();
	for( int i = 0; i < (int)strShortcut.Len(); i++ )
	{
		if( strShortcut[i] == L'%' &&
				MyStrincmp( ((const wchar_t *)strShortcut)+i+1, strVarName, nVarNameLength ) == 0 &&
					strShortcut[i+nVarNameLength+1] == L'%' )
		{
			// var found
			strShortcut.Delete( i, nVarNameLength+2 );
			strShortcut.Insert( i, strVarValue );
		}
	}
}

LPCWSTR IsSfxSwitch( LPCWSTR lpwszCommandLine, LPCWSTR lpwszSwitch )
{
	// 1. check old notation
	if( lpwszCommandLine[0] == L'-' && lpwszCommandLine[1] == '-' )
	{
		lpwszCommandLine++;
	}
	// 2. New (common) notation
	if( lpwszCommandLine[0] == L'-' || lpwszCommandLine[0] == '/')
	{
		int nSwitchLength = lstrlen(lpwszSwitch);
		if( MyStrincmp( lpwszCommandLine+1, lpwszSwitch, lstrlen(lpwszSwitch) ) == 0 &&
				(((unsigned)lpwszCommandLine[nSwitchLength+1]) <= L' ' || lpwszCommandLine[nSwitchLength+1] == L':') )
		{
			return lpwszCommandLine+nSwitchLength+1;
		}
	}
	
	return NULL;
}

LPCWSTR ParseConfigOverride( LPCWSTR lpwszCommandLine, CObjectVector<CTextConfigPair> & pairs )
{
	static LPCWSTR ConfigParams[] = {
		CFG_TITLE,
		CFG_ERRORTITLE,
#ifdef _SFX_USE_WARNINGS
		CFG_WARNINGTITLE,
#endif // _SFX_USE_WARNINGS
		CFG_GUIMODE,
		CFG_GUIFLAGS,
		CFG_MISCFLAGS,
		CFG_BEGINPROMPT,
		CFG_INSTALLPATH,
		CFG_EXTRACT_TITLE,
		CFG_EXTRACT_CANCELTEXT,
		CFG_EXTRACT_DIALOGTEXT,
		CFG_EXTRACT_DIALOGWIDTH,
		CFG_SELFDELETE,
		CFG_EXTRACT_PATH_TITLE,
		CFG_EXTRACT_PATH_TEXT,
		CFG_HELP_TEXT,
		CFG_OVERWRITE_MODE,
		CFG_CANCEL_PROMPT,
		CFG_EXTRACTPATH_WIDTH,
		CFG_FINISHMESSAGE,
		CFG_EXECUTEFILE,
		CFG_EXECUTEPARAMETERS,
		CFG_DIRECTORY,
		CFG_PROGRESS,
		CFG_SETENVIRONMENT,
#ifdef _SFX_USE_BEGINPROMPTTIMEOUT
		CFG_BEGINPROMPTTIMEOUT,
#endif // _SFX_USE_BEGINPROMPTTIMEOUT
#ifdef SFX_CRYPTO
		CFG_PASSWORD_TITLE,
		CFG_PASSWORD_TEXT,
#endif // SFX_CRYPTO
#ifdef _SFX_USE_VOLUME_NAME_STYLE
		CFG_VOLUME_NAME_STYLE,
#endif // _SFX_USE_VOLUME_NAME_STYLE
		NULL
	};

	LPCWSTR * pParams = ConfigParams;
	while( *pParams != NULL )
	{
		LPCWSTR p = lpwszCommandLine;
		int nLen = lstrlen(*pParams);
		if( MyStrincmp( lpwszCommandLine, *pParams, nLen ) == 0 && lpwszCommandLine[nLen] == L'=' )
		{
			CSfxStringU str = lpwszCommandLine;
			LPCWSTR p = str;
			bool inQuotes=false;
			while( *p )
			{
				if( *p <= L' ' && inQuotes == false )
					break;
				if( *p == L'\"' )
				{
					if( inQuotes == false )
						inQuotes = true;
					else
						inQuotes = false;
				}
				else
				{
					if( p[0] == L'\\' && p[1] == L'\"' )
						p++;
				}
				p++;
			}
			int nSwitchLen = (int)(p-(LPCWSTR)str);
			str.ReleaseBuffer( nSwitchLen );
			CSfxStringA	aStr= SfxUnicodeStringToMultiByte( str, CP_UTF8 );
			if( GetTextConfig( aStr, pairs, true ) == false )
				return (LPCWSTR)1;
			return lpwszCommandLine + nSwitchLen;
		}
		pParams++;
	}
	return NULL;
}

void SetConfigVariables( CObjectVector<CTextConfigPair>& pairs )
{
	LPCWSTR	lpwszValue;
	int from;

	// Main title
	if( (lpwszValue = GetTextConfigValue( pairs, CFG_TITLE )) != NULL )
	{
		strErrorTitle = lpwszValue; strErrorTitle += GetLanguageString( STR_ERROR_SUFFIX );
		lpwszErrorTitle = strErrorTitle;
#ifdef _SFX_USE_WARNINGS
		strWarningTitle = lpwszValue; strWarningTitle += GetLanguageString( STR_WARNING_SUFFIX );
		lpwszWarningTitle = strWarningTitle;
#endif // _SFX_USE_WARNINGS
		lpwszTitle = lpwszValue;
	}
	// Update error title
	if( (lpwszValue = GetTextConfigValue( pairs, CFG_ERRORTITLE )) != NULL )
		lpwszErrorTitle = lpwszValue;
#ifdef _SFX_USE_WARNINGS
	// Update warnings title
	if( (lpwszValue = GetTextConfigValue( pairs, CFG_WARNINGTITLE )) != NULL )
		lpwszWarningTitle = lpwszValue;
#endif // _SFX_USE_WARNINGS
	// Extract title
	if( (lpwszValue = GetTextConfigValue( pairs, CFG_EXTRACT_TITLE )) != NULL )
		lpwszExtractTitle = lpwszValue;
	// Load GUIMode & 7-Zip > 4.42 'Progress'
	if( (lpwszValue = GetTextConfigValue( pairs, CFG_PROGRESS)) != NULL && lstrcmpi( lpwszValue, L"no" ) == 0 )
		GUIMode = GUIMODE_HIDDEN;
	if( (lpwszValue = GetTextConfigValue( pairs, CFG_GUIMODE )) != NULL )
	{
		if( lpwszValue[0] >= L'0' && lpwszValue[0] <= L'0'+GUIMODE_MAX )
			GUIMode = lpwszValue[0] - L'0';
	}
	// Load OverwriteMode
	if( (lpwszValue = GetTextConfigValue( pairs, CFG_OVERWRITE_MODE)) != NULL )
	{
		UpdateOverwriteMode( lpwszValue );
	}
	// Load GUIFlags
	from = 0;
	while( (lpwszValue = GetTextConfigValue( pairs, CFG_GUIFLAGS, &from )) != NULL )
	{
		from++;
		UpdateGUIFlags( lpwszValue );
	}
	// Load MiscFlags
	from = 0;
	while( (lpwszValue = GetTextConfigValue( pairs, CFG_MISCFLAGS, &from )) != NULL )
	{
		from++;
		UpdateMiscFlags( lpwszValue );
	}
	
	lpwszCancelText = GetTextConfigValue( pairs, CFG_EXTRACT_CANCELTEXT );
	lpwszExtractDialogText = GetTextConfigValue( pairs, CFG_EXTRACT_DIALOGTEXT );
	if( (lpwszValue = GetTextConfigValue( pairs, CFG_EXTRACT_DIALOGWIDTH)) != NULL )
		ExtractDialogWidth = StringToLong( lpwszValue );
	if( (lpwszValue = GetTextConfigValue( pairs, CFG_EXTRACTPATH_WIDTH)) != NULL )
		ExtractPathWidth = StringToLong( lpwszValue );
	
	// Extract path strings
	if( (lpwszValue = GetTextConfigValue(pairs,CFG_EXTRACT_PATH_TITLE)) != NULL )
		lpwszExtractPathTitle = lpwszValue;
	if( (lpwszValue = GetTextConfigValue(pairs,CFG_EXTRACT_PATH_TEXT)) != NULL )
		lpwszExtractPathText = lpwszValue;
	
	// Cancel prompt text
	if( (lpwszValue = GetTextConfigValue(pairs,CFG_CANCEL_PROMPT)) != NULL )
		lpwszCancelPrompt = lpwszValue;

#ifdef SFX_CRYPTO
	if( (lpwszValue = GetTextConfigValue(pairs,CFG_PASSWORD_TITLE)) != NULL )
		lpwszPasswordTitle = lpwszValue;
	else
		lpwszPasswordTitle = lpwszTitle;
	if( (lpwszValue = GetTextConfigValue(pairs,CFG_PASSWORD_TEXT)) != NULL )
		lpwszPasswordText = lpwszValue;
	else
		lpwszPasswordText = GetLanguageString( STR_PASSWORD_TEXT );
#endif // SFX_CRYPTO
#ifdef _SFX_USE_VOLUME_NAME_STYLE
	if( (lpwszValue = GetTextConfigValue( pairs, CFG_VOLUME_NAME_STYLE )) != NULL )
	{
		if( lpwszValue[0] >= L'0' && lpwszValue[0] <= L'9' )
			nVolumeNameStyle = lpwszValue[0] - L'0';
	}
#endif // _SFX_USE_VOLUME_NAME_STYLE
}

void OverrideConfigParam( LPCWSTR lpwszName, LPCWSTR lpwszValue, CObjectVector<CTextConfigPair>& pairs )
{
	CSfxStringU strParam = lpwszName;
	strParam += L"=\"";
	while( *lpwszValue > L' ' ) strParam += *lpwszValue++;
	strParam += L"\"";
	ParseConfigOverride( strParam, pairs );
}

void PostExecute_Shortcut( LPCWSTR lpwszValue )
{
	CSfxStringU strShortcut = lpwszValue;
	ReplaceWithExtractPath( strShortcut, extractPath );
	ReplaceWithArchivePath( strShortcut, strSfxFolder );
	ReplaceWithArchiveName( strShortcut, strSfxName );
#ifdef _SFX_USE_PREFIX_PLATFORM
	ReplaceWithPlatform( strShortcut );
#endif // _SFX_USE_PREFIX_PLATFORM
	for( int i = 0; i < (int)Variables.Size(); i++ )
	{
		CSfxStringU varValue = MyGetEnvironmentVariable( Variables[i].ID );
		ExpandEnvironmentStrings( varValue );
		ReplaceWithExtractPath( varValue, extractPath );
		ReplaceWithArchivePath( varValue, strSfxFolder );
		ReplaceWithArchiveName( varValue, strSfxName );
#ifdef _SFX_USE_PREFIX_PLATFORM
		ReplaceWithPlatform( varValue );
#endif // _SFX_USE_PREFIX_PLATFORM

		ReplaceVariableInShortcut( strShortcut, Variables[i].ID, varValue );
	}
	CreateShortcut( strShortcut );
}

void PostExecute_Delete( LPCWSTR lpwszValue )
{
	CSfxStringU tmp = lpwszValue;
	ReplaceVariablesEx( tmp );
	DeleteFileOrDirectoryAlways( tmp );
}

typedef void (* POST_EXECUTE_PROC)( LPCWSTR lpwszParamName );

void ProcessPostExecuteSub( POST_EXECUTE_PROC pfnPostExecute,
							CObjectVector<CTextConfigPair>& pairs,
						    LPCWSTR lpwszParamPrefix,
							LPCWSTR lpwszBatchIndexes,
							int nUseDefault )
{
		CSfxStringU	ustrParamName;
		LPCWSTR	lpwszValue;
		LPCWSTR p1 = lpwszBatchIndexes;
		bool	fBatchExecuted = false;
		int nIndex;
		while( (*p1 >= L'0' && *p1 <= L'9') ||
			   (*p1 >= L'a' && *p1 <= L'z') ||
			   (*p1 >= L'A' && *p1 <= L'Z') ||
			   (lpwszBatchIndexes == p1 && nUseDefault == -1) )
		{
			ustrParamName = lpwszParamPrefix;
			ustrParamName += *p1;
			nIndex = 0;
			p1++;

			while( (lpwszValue = GetTextConfigValue( pairs, ustrParamName, &nIndex)) != NULL )
			{
				pfnPostExecute( lpwszValue );
				nIndex++;
				fBatchExecuted = true;
			}
		}
		switch( nUseDefault )
		{
		case 0:
			break;
		case 1:
			ProcessPostExecuteSub( pfnPostExecute, pairs, lpwszParamPrefix, L"\0", -1 );
			break;
		default:
			if( *lpwszBatchIndexes == L'\0' && nUseDefault == -1 )
				break;
			if( fBatchExecuted == false && *lpwszBatchIndexes != L'\0' )
				ProcessPostExecuteSub( pfnPostExecute, pairs, lpwszParamPrefix, L"\0",  -1 );
		}
}

void ShowSfxVersion()
{
	GUIFlags = GUIFLAGS_XPSTYLE;
	CSfxDialog_Version dlg;
	extern unsigned int g_NumCodecs;
	extern const CCodecInfo *g_Codecs[]; 
	CSfxStringU ustrVersion = GetLanguageString( STR_SFXVERSION );
	unsigned int ki;
	for( ki = 0; ki < g_NumCodecs; ki++ )
	{
		if( ki != 0 )
			ustrVersion += L", ";
		ustrVersion +=g_Codecs[ki]->Name;
		//ustrVersion.AddAscii( g_Codecs[ki]->Name );
	}
#ifdef SFX_VOLUMES
	if( ki != 0 )
		ustrVersion += L", ";
	ustrVersion += L"Volumes";
#endif // SFX_VOLUMES
	WCHAR tmp[128];
	wsprintf( tmp, L" \n\t%X - %03X - %03X - %03X - %03X",
				SFXBUILD_OPTIONS1, SFXBUILD_OPTIONS2, SFXBUILD_OPTIONS3, SFXBUILD_OPTIONS4, SFXBUILD_OPTIONS5 );
	ustrVersion += tmp;
	ustrVersion += L"\n";
	dlg.Show( SD_OK|SD_ICONINFORMATION, lpwszTitle, ustrVersion );
}

void SfxCleanup()
{
	if( fUseInstallPath == false )
		DeleteFileOrDirectoryAlways( extractPath );
}

BOOL SfxExecute( LPCWSTR lpwszCmdLine, DWORD dwFlags, LPCWSTR lpwszDirectory )
{
	CSfxStringU filePath;
	CSfxStringU fileParams;
	SHELLEXECUTEINFO execInfo;
	memset( &execInfo, 0, sizeof(execInfo) );
	execInfo.cbSize = sizeof(execInfo);
	execInfo.lpDirectory = lpwszDirectory;

	execInfo.fMask = 
		SEE_MASK_NOCLOSEPROCESS |
		SEE_MASK_FLAG_DDEWAIT |
		SEE_MASK_DOENVSUBST |
		SEE_MASK_FLAG_NO_UI;
	execInfo.nShow = SW_SHOWNORMAL;

	if( (dwFlags&SFXEXEC_HIDCON) != 0 )
	{
		// use hidcon
		execInfo.nShow = SW_HIDE;
		execInfo.fMask |= SEE_MASK_NO_CONSOLE;
	}
	if( (dwFlags&SFXEXEC_RUNAS) != 0 )
		execInfo.lpVerb = L"runas";

	fileParams = LoadQuotedString( lpwszCmdLine, filePath );
	if( filePath.Len() == 0 )
		return TRUE;
	execInfo.lpFile = filePath;
	execInfo.lpParameters = fileParams;
	if( ::ShellExecuteEx( &execInfo ) != FALSE )
	{
		if( (dwFlags&SFXEXEC_NOWAIT) == 0 )
			::WaitForSingleObject( execInfo.hProcess, INFINITE );
		::CloseHandle( execInfo.hProcess );
		return TRUE;
	}
	return FALSE;
}


#ifdef _SFX_USE_PREFIX_WAITALL
	DWORD Parent_ExecuteSfxWaitAll( LPCWSTR lpwszApp, LPCWSTR lpwszCmdLine, int flags )
	{
		CSfxStringU sfxPath;
		CSfxStringU executeString;
		LoadQuotedString( ::GetCommandLine(), sfxPath );
		executeString = L"\"" + sfxPath + L"\" -" + CMDLINE_SFXWAITALL + L':' + (WCHAR)(flags+'0') + L' ' +
						L'\"' + lpwszApp + L"\" " + lpwszCmdLine;
		STARTUPINFO	si;
		PROCESS_INFORMATION pi;
		si.cb = sizeof(si);
		::GetStartupInfo(&si);
		if( ::CreateProcess(NULL, (LPWSTR)(LPCWSTR)executeString,NULL,NULL,TRUE,
					CREATE_BREAKAWAY_FROM_JOB|CREATE_SUSPENDED,NULL,(LPCTSTR)extractPath,&si,&pi) == FALSE )
			return ::GetLastError();
		bool fWaitProcess = true;
		HANDLE hJob = NULL;
		HANDLE hIocp = NULL;
		if( (hJob = ::CreateJobObject( NULL, NULL))!= NULL &&
			AssignProcessToJobObject( hJob,pi.hProcess ) != FALSE &&
			(hIocp = CreateIoCompletionPort( INVALID_HANDLE_VALUE, 0, 1, 0 )) != NULL )
		{
			JOBOBJECT_ASSOCIATE_COMPLETION_PORT	iop;
			iop.CompletionKey = (PVOID)1;
			iop.CompletionPort = hIocp;
			SetInformationJobObject( hJob, JobObjectAssociateCompletionPortInformation, &iop, sizeof(iop) );
			::ResumeThread( pi.hThread );
			// Wait for events
			DWORD dwBytes;
			ULONG_PTR completionkey;
			LPOVERLAPPED ove;
			while( GetQueuedCompletionStatus( hIocp, &dwBytes, &completionkey, &ove, INFINITE) != FALSE )
			{
				if( dwBytes == JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO )
				{
					fWaitProcess = false;
					break;
				}
			}
		}
		if( fWaitProcess != false )
		{
			::ResumeThread( pi.hThread );
			::WaitForSingleObject( pi.hProcess, INFINITE );
		}
		::CloseHandle( pi.hThread );
		DWORD dwExitCode;
		if( ::GetExitCodeProcess( pi.hProcess, &dwExitCode ) == FALSE )
			dwExitCode = ::GetLastError();
		::CloseHandle( pi.hProcess );
		if( hIocp != NULL ) :: CloseHandle( hIocp );
		if( hJob != NULL ) ::CloseHandle( hJob );
		return dwExitCode;
	}

	DWORD Child_ExecuteSfxWaitAll( LPCWSTR lpwszCmdLine )
	{
		DWORD dwFlags = 0;
		if( lpwszCmdLine[0] == L':' && lpwszCmdLine[1] >= L'0' && lpwszCmdLine[1] <= L'9' )
		{
			dwFlags=StringToLong( lpwszCmdLine+1 );
			while( ((unsigned)*lpwszCmdLine) > L' ' ) lpwszCmdLine++;
			SKIP_WHITESPACES_W( lpwszCmdLine);
		}
		if( SfxExecute( lpwszCmdLine, dwFlags, NULL ) == FALSE )
			return GetLastError();
		return ERROR_SUCCESS;
	}
#endif // _SFX_USE_PREFIX_WAITALL

#include <new.h>
int sfx_new_handler( size_t size )
{
	MessageBoxA( NULL, "Could not allocate memory", "7-Zip SFX", MB_OK|MB_ICONSTOP );
	return 0;
}

#ifdef _SFX_USE_CUSTOM_EXCEPTIONS
int APIENTRY WinMain2( HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPSTR lpCmdLine, int nCmdShow );

int APIENTRY WinMain( HINSTANCE hInstance,
					  HINSTANCE hPrevInstance,
					  LPSTR lpCmdLine, int nCmdShow )
{
	try {
		return WinMain2( hInstance, hPrevInstance, lpCmdLine, nCmdShow );
	}
	catch( ... ) {
		return ERRC_EXCEPTION;
	}
}
int APIENTRY WinMain2( HINSTANCE hInstance,
#else
int APIENTRY WinMain( HINSTANCE hInstance,
#endif // _SFX_USE_CUSTOM_EXCEPTIONS
					 HINSTANCE hPrevInstance,
					 LPSTR lpCmdLine, int nCmdShow )
{
	_set_new_handler( sfx_new_handler );
	CreateDummyWindow();

	OSVERSIONINFO versionInfo;
	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
	if( ::GetVersionEx(&versionInfo) == FALSE ||
			versionInfo.dwPlatformId != VER_PLATFORM_WIN32_NT ||
				versionInfo.dwMajorVersion < 5 )
	{
		::MessageBoxA( NULL, "Sorry, this program requires Microsoft Windows 2000 or later.", "7-Zip SFX", MB_OK|MB_ICONSTOP );
		return ERRC_PLATFORM;
	}

	kSignatureConfigStart[0] = kSignatureConfigEnd[0] = ';';

	int		ShortcutDefault = -1;
	int		DeleteDefault = -1;
	int		SelfDelete = -1;
	LPCWSTR	lpwszBatchInstall = L"\0";
	bool	fAssumeYes = false;
	bool	fUseAutoInstall = false;
	bool	fNoRun = false;
	bool	fShowHelp = false;
	LPCWSTR	lpwszValue;
	// Command line
	CSfxStringU tmpstr;
	CSfxStringU	SfxConfigFilename;
	CObjectVector<CTextConfigPair> pairs;

	CSfxStringU strCmdLine = L" ";
	strCmdLine += LoadQuotedString( ::GetCommandLine(), tmpstr );
	ReplaceHexChars( strCmdLine );
	LPCWSTR str = strCmdLine;
	str++;
#ifdef _SFX_USE_ENVIRONMENT_VARS
	LPCWSTR lpwszCmdLineStart = str;
	LPCWSTR lpwszCmdLineEnd = str + lstrlen(str);
	LPCWSTR lpwszCmdLine1End = NULL;
	SfxAddEnvironmentVarWithAlias( SFX_ENV_VAR_MODULE_PLATFORM, PLATFORM_NAME_W );
	SfxAddEnvironmentVarWithAlias( SFX_ENV_VAR_SYSTEM_PLATFORM, GetPlatformName() );
	SfxAddEnvironmentVarWithAlias( SFX_ENV_VAR_CMDLINE_0, lpwszCmdLineStart );
#endif // _SFX_USE_ENVIRONMENT_VARS

	SfxInit();

#ifdef _SFX_USE_LANG
	#ifdef _SFX_USE_ENVIRONMENT_VARS
		wsprintf( tmpstr.GetBuffer(32), L"%d", idSfxLang );
		tmpstr.ReleaseBuffer();
		SfxAddEnvironmentVarWithAlias( SFX_ENV_VAR_SYSTEM_LANGUAGE, (LPCWSTR)tmpstr );
	#endif // _SFX_USE_ENVIRONMENT_VARS

	if( (lpwszValue = IsSfxSwitch( str, CMDLINE_SFXLANG )) != NULL && *lpwszValue == L':' )
	{
		long idLang=StringToLong( lpwszValue+1 );
		if( idLang > 0 && idLang <= 0xffff )
			idSfxLang = (LANGID)idLang;
		while( ((unsigned)*str) > L' ' ) str++;
		SKIP_WHITESPACES_W( str );
	}
#endif // _SFX_USE_LANG

	if( IsSfxSwitch( str, CMDLINE_SFXVERSION ) != NULL )
	{
		ShowSfxVersion();
		return ERRC_NONE;
	}

#ifdef _SFX_USE_PREFIX_WAITALL
	if( (lpwszValue = IsSfxSwitch(str,CMDLINE_SFXWAITALL )) != NULL )
	{
		return Child_ExecuteSfxWaitAll( lpwszValue );
	}
#endif // _SFX_USE_PREFIX_WAITALL

#ifdef _SFX_USE_ELEVATION
	bool fInElevation = false;
	if( (lpwszValue = IsSfxSwitch(str,CMDLINE_SFXELEVATION)) != NULL )
	{
		fInElevation = true;
		SKIP_WHITESPACES_W( lpwszValue );
		str = lpwszValue;
	}
#endif // _SFX_USE_ELEVATION
#ifdef _DEBUG
	strModulePathName = L"C:\\tmp\\test_sfx.exe";
#else
	if( ::GetModuleFileName( NULL, strModulePathName.GetBuffer(MAX_PATH*2), MAX_PATH*2 ) == 0 )
	{
		SfxErrorDialog( TRUE, ERR_MODULEPATHNAME );
		return ERRC_GET_PATHNAME;
	}
#endif
	strModulePathName.ReleaseBuffer();

	if( (lpwszValue = IsSfxSwitch( str, CMDLINE_SFXTEST )) != NULL )
#ifdef _SFX_USE_TEST
	{
		if( lpwszValue[0] != L':' )
			return ERRC_SFXTEST;
		switch( lpwszValue[1]|L' ' )
		{
		case L'd':
			// dialogs test
			if( lpwszValue[1] == L'D' )
			{
				if( lpwszValue[2] != L':' )
					return ERRC_SFXTEST;
				lpwszValue += 3;
				while( ((unsigned)*lpwszValue) > L' ' && *lpwszValue != L':' )
				{
					TSD_Flags += *lpwszValue++;
				}
				if( TSD_Flags.IsEmpty() != false )
					return ERRC_SFXTEST;
			}
			else
				lpwszValue += 2;
			TSD_ExtractTimeout = 0;
			if( *lpwszValue == L':' )
			{
				TSD_ExtractTimeout=StringToLong( lpwszValue+1 );
			}
			if( TSD_ExtractTimeout > 3600 || TSD_ExtractTimeout == 0 )
					TSD_ExtractTimeout = 10;
			nTestModeType = TMT_DIALOGS;
			break;
		case L'a':
			nTestModeType = TMT_ARCHIVE;
			break;
		case L'c':
			if( lpwszValue[2] == L'c' )
				nTestModeType = TMT_CHECK_CONFIG;
			else
				nTestModeType = TMT_DIALOGS_TO_STDOUT;
			break;
		case 'o':
			if( lpwszValue[2] != L':' )
				return 0;
			switch( lpwszValue[3] )
			{
			case '1':
				return SFXBUILD_OPTIONS1;
			case '2':
				return SFXBUILD_OPTIONS2;
			case '3':
				return SFXBUILD_OPTIONS3;
			case '4':
				return SFXBUILD_OPTIONS4;
			case '5':
				return SFXBUILD_OPTIONS5;
			default:
				return 0;
			}
		case L'v':
			return 0x4000 | VERSION_REVISION;
		default:
			return ERRC_SFXTEST;
		}
	
		while( ((unsigned)*lpwszValue) > L' ' ) lpwszValue++;
		SKIP_WHITESPACES_W( lpwszValue );
		str = lpwszValue;
		
		if( (lpwszValue = IsSfxSwitch( str,CMDLINE_SFXCONFIG )) != NULL )
		{
			if( *lpwszValue == L':' ) lpwszValue++;
			SKIP_WHITESPACES_W( lpwszValue );
			tmpstr.Empty();
			str = LoadQuotedString( lpwszValue, tmpstr );
			if( nTestModeType != TMT_ARCHIVE )
				strModulePathName = tmpstr;
		}
	}
#else
	{
		return ERRC_SFXTEST;
	}
#endif // _SFX_USE_TEST

	strSfxFolder = strModulePathName;
	strSfxName = strModulePathName;
	int nPos = GetDirectorySeparatorPos( strModulePathName );
	if( nPos >= 0 )
	{
		strSfxFolder.ReleaseBuffer( nPos );
		strSfxName = ((const wchar_t *)strModulePathName) + nPos + 1;
		strTitle = ((const wchar_t *)strModulePathName)+nPos+1;
		if( (nPos = strTitle.ReverseFind( L'.' )) > 0 )
			strTitle.ReleaseBuffer( nPos );
		strErrorTitle = strTitle;
		strErrorTitle += GetLanguageString( STR_ERROR_SUFFIX );
		lpwszTitle = strTitle;
		lpwszErrorTitle = strErrorTitle;
#ifdef SFX_CRYPTO
		lpwszPasswordTitle = lpwszTitle;
#endif // SFX_CRYPTO
	}
#ifdef _SFX_USE_PREFIX_PLATFORM
	strOSPlatform = GetPlatformName();
#endif // _SFX_USE_PREFIX_PLATFORM
	
	// Create input stream
	CSfxInStream *inStreamSpec = new CSfxInStream;
	CMyComPtr<IInStream> inStream(inStreamSpec);
	if( !inStreamSpec->Open(strModulePathName) )
	{
		SfxErrorDialog( TRUE, ERR_OPEN_ARCHIVE, (LPCWSTR)strModulePathName );
		return ERRC_OPEN_ARCHIVE;
	}

	// Read SFX config
	CSfxStringA	config;

#if defined(_SFX_USE_CONFIG_PLATFORM) || defined(_SFX_USE_LANG)
	if( LoadConfigs( inStream, config ) == false )
#else
	if( ReadConfig( inStream, kSignatureConfigStart, kSignatureConfigEnd, config ) == false )
#endif // defined(_SFX_USE_CONFIG_PLATFORM) || defined(_SFX_USE_LANG)
	{
		SfxErrorDialog( FALSE, ERR_READ_CONFIG );
		return ERRC_READ_CONFIG;
	}

#ifdef _SFX_USE_TEST
	if( nTestModeType == 0 && (lpwszValue = IsSfxSwitch( str,CMDLINE_SFXCONFIG )) != NULL )
#else
	if( (lpwszValue = IsSfxSwitch( str,CMDLINE_SFXCONFIG )) != NULL )
#endif // _SFX_USE_TEST
	{
		if( *lpwszValue == L':' ) lpwszValue++;
		SKIP_WHITESPACES_W( lpwszValue );
		if( *lpwszValue != L'\0' && SaveConfiguration( lpwszValue, config ) == false )
		{
			SfxErrorDialog( FALSE, ERR_WRITE_CONFIG );
			return ERRC_CONFIG_DATA;
		}
		return ERRC_NONE;
	}
	
	pairs.Clear();
	if( !config.IsEmpty() )
	{
		// Split SFX config into pairs
		if( GetTextConfig(config, pairs,false) == false )
			return ERRC_CONFIG_DATA;
	}
#ifdef _SFX_USE_TEST
	if( nTestModeType == TMT_CHECK_CONFIG )
		return ERRC_NONE;
#endif // _SFX_USE_TEST
	// 
	LANGSTRING * pLangString = SfxLangStrings;
	while( pLangString->id != 0 )
	{
		WCHAR wszVarName[sizeof(SFX_STRING_FORMAT)/sizeof(WCHAR)+32];
		wsprintf( wszVarName, SFX_STRING_FORMAT, pLangString->id );
		SfxAddEnvironmentVar( wszVarName, GetLanguageString(pLangString->id) );
		pLangString++;
	}

	SetConfigVariables( pairs );
	// parse command line
	str--;
#ifdef _SFX_USE_ENVIRONMENT_VARS
	SfxAddEnvironmentVarWithAlias( SFX_ENV_VAR_CMDLINE_2, L"" );
#endif // _SFX_USE_ENVIRONMENT_VARS
	while( true )
	{
		while( ((unsigned)*str) > L' ' ) str++;
		SKIP_WHITESPACES_W( str );
		if( str[0] != L'/' && str[0] != L'-' )
			break;
		// '-!'
		if( str[1] == L'!' )
		{
#ifdef _SFX_USE_ENVIRONMENT_VARS
			lpwszCmdLine1End=str;
#endif // _SFX_USE_ENVIRONMENT_VARS
			str += 2;
			break;
		}
		
		// 'ai', 'aiX, 'aiXXX'
		if( IsCommandLineSwitch( str, L"ai" ) != FALSE )
		{
			// first check batch mode
			if( (str[3] >= L'0' && str[3] <= '9') ||
				(str[3] >= L'a' && str[3] <= 'z') ||
				(str[3] >= L'A' && str[3] <= 'Z') )
			{
				lpwszBatchInstall = str+3;
				fUseAutoInstall = fAssumeYes = true;
				continue;
			}
			// sample autoinstall
			if( ((unsigned)str[3]) <= L' ' )
			{
				lpwszBatchInstall = L"\0";
				fUseAutoInstall = fAssumeYes = true;
				continue;
			}
			break;
		}

		// 'om' - Overwrite mode
		if( IsCommandLineSwitch( str, L"om" ) != FALSE )
		{
			OverrideConfigParam( CFG_OVERWRITE_MODE,str+3, pairs );
			continue;
		}

		// 'gm' - GUI mode
		if( IsCommandLineSwitch( str, L"gm" ) != FALSE )
		{
			if( str[3] >= L'0' && str[3] <= (L'0'+GUIMODE_MAX) && str[4] <= L' ' )
			{
				OverrideConfigParam( CFG_GUIMODE,str+3, pairs );
				continue;
			}
			break;
		}

		// 'gf' - GUI flags
		if( IsCommandLineSwitch( str, L"gf" ) != FALSE )
		{
			OverrideConfigParam( CFG_GUIFLAGS,str+3,pairs );
			continue;
		}

		// 'mf' - Misc flags
		if( IsCommandLineSwitch( str, L"mf" ) != FALSE )
		{
			OverrideConfigParam( CFG_MISCFLAGS,str+3,pairs );
			continue;
		}

		// 'sd' - self delete
		if( IsCommandLineSwitch( str, L"sd" ) != FALSE )
		{
			if( (str[3] == L'0' || str[3] == L'1') && ((unsigned)str[4]) <= L' ' )
			{
				OverrideConfigParam( CFG_SELFDELETE,str+3,pairs );
				continue;
			}
			break;
		}

		// 'nr' - no run
		if( IsCommandLineSwitch( str, L"nr" ) != FALSE )
		{
			if( ((unsigned)str[3]) <= L' ' )
			{
				fNoRun = true;
				continue;
			}
			break;
		}

		// 'fm' - finish message
		if( IsCommandLineSwitch( str, L"fm" ) != FALSE )
		{
			if( str[3] >= L'0' && str[3] <= L'9' )
			{
				FinishMessage = StringToLong( str+3 );
				continue;
			}
			break;
		}

#ifdef _SFX_USE_BEGINPROMPTTIMEOUT
		// 'bpt' - BeginPrompt timeout
		if( IsCommandLineSwitch( str, L"bpt" ) != FALSE )
		{
			OverrideConfigParam( CFG_BEGINPROMPTTIMEOUT,str+4, pairs );
			continue;
		}
#endif // _SFX_USE_BEGINPROMPTTIMEOUT
#ifdef SFX_CRYPTO
		if( str[1] == L'p' || str[1] == L'P' )
		{
			CSfxStringU password;
			str = LoadQuotedString( str+2, password )-1;
			CSfxPassword::Set( password );
			continue;
		}
#endif // SFX_CRYPTO

		// assume 'yes'
		if( (str[1] == L'y' || str[1] == L'Y') && ((unsigned)str[2]) <= L' ' )
		{
			fAssumeYes = true;
			continue;
		}

		if( (str[1] == L'?' || str[1] == L'h' || str[1] == L'H') && ((unsigned)str[2]) <= L' ' )
		{
			fShowHelp = true;
			continue;
		}
		LPCWSTR pnext = ParseConfigOverride( str+1, pairs );
		if( pnext != NULL )
		{
			if( pnext == (LPCWSTR)1 )
				return ERR_CONFIG_CMDLINE;
			str = pnext;
			continue;
		}
		break;
	}
#ifdef _SFX_USE_ENVIRONMENT_VARS
	if( lpwszCmdLine1End == NULL ) lpwszCmdLine1End = str;
	if( lpwszCmdLineStart != lpwszCmdLine1End )
	{
		tmpstr = CSfxStringU(lpwszCmdLineStart).Left((int)(lpwszCmdLine1End-lpwszCmdLineStart));
		tmpstr.Trim();
		SfxAddEnvironmentVarWithAlias( SFX_ENV_VAR_CMDLINE_1, (LPCWSTR)tmpstr );
	}
	else
		SfxAddEnvironmentVarWithAlias( SFX_ENV_VAR_CMDLINE_1, L"" );
	tmpstr = CSfxStringU(str).Left((int)(lpwszCmdLineEnd-str));
	tmpstr.Trim();
	SfxAddEnvironmentVarWithAlias( SFX_ENV_VAR_CMDLINE_2, (LPCWSTR)tmpstr );
#endif // _SFX_USE_ENVIRONMENT_VARS

	SetConfigVariables( pairs );

#ifdef _SFX_USE_ELEVATION
// Check elevation
	if( fInElevation == false && (MiscFlags&MISCFLAGS_ELEVATE) != 0 && IsRunAsAdmin() == FALSE )
	{
		CSfxStringU sfxPath;
		CSfxStringU executeString;
		CSfxStringU strCmdLine = LoadQuotedString( ::GetCommandLine(), sfxPath );
		executeString = L"\"" + sfxPath + L"\" -" + CMDLINE_SFXELEVATION + L' ' + strCmdLine;
		::SetProcessWorkingSetSize( ::GetCurrentProcess(), (SIZE_T)-1, (SIZE_T)-1 );
		if( SfxExecute( executeString, SFXEXEC_RUNAS, NULL ) == FALSE )
			return ERRC_ELEVATE;
		return ERRC_NONE;
	}
#endif // _SFX_USE_ELEVATION

	// extra environment variables
	int		nIndex = 0;
	while( (lpwszValue = GetTextConfigValue( pairs, CFG_SETENVIRONMENT, &nIndex)) != NULL )
	{
		CSfxStringU strEnvName = lpwszValue;
		nPos = strEnvName.Find( L'=');
		if( nPos <= 0 )
			break;
		strEnvName.ReleaseBuffer( nPos );
		SfxAddEnvironmentVar( (LPCWSTR)strEnvName, lpwszValue+nPos+1 );
		nIndex++;
	}

	SetEnvironment();

	if( GUIFlags == -1 )
		GUIFlags = 0;

	if( fShowHelp != false )
	{
		if( (lpwszValue = GetTextConfigValue(pairs,CFG_HELP_TEXT)) == NULL )
				lpwszValue = GetLanguageString( STR_DEFAULT_HELP_TEXT );
		CSfxDialog_HelpText	dlg;
		dlg.Show( SD_OK|SD_ICONINFORMATION, lpwszTitle, lpwszValue );
		return ERRC_NONE;
	}

	if( fAssumeYes != false )
		GUIFlags &= (~GUIFLAGS_CONFIRM_CANCEL);

	(void)CoInitialize( NULL );

	// Path to extract
	if( (lpwszValue = GetTextConfigValue(pairs,CFG_INSTALLPATH)) != NULL )
		extractPath = lpwszValue;

	ReplaceVariables( extractPath );

#ifdef _SFX_USE_BEGINPROMPTTIMEOUT
	if( (lpwszValue = GetTextConfigValue( pairs, CFG_BEGINPROMPTTIMEOUT)) != NULL )
		BeginPromptTimeout = StringToLong( lpwszValue );
#endif // _SFX_USE_BEGINPROMPTTIMEOUT

#ifdef _SFX_USE_TEST
	if( TSD_Flags.IsEmpty() == false )
	{
		return TestSfxDialogs( pairs );
	}
	if( nTestModeType == TMT_DIALOGS_TO_STDOUT )
	{
		return TestSfxDialogsToStdout( pairs );
	}
#endif // _SFX_USE_TEST

#ifdef _SFX_USE_EARLY_PASSWORD
	if( (MiscFlags&MISCFLAGS_LATE_PASSWORD) == 0 && CSfxPassword::EarlyPassword(inStream) == FALSE )
	{
		SfxErrorDialog( FALSE, ERR_NON7Z_ARCHIVE );
		return ERRC_CANCELED;
	}
#endif // _SFX_USE_EARLY_PASSWORD

	// Begin prompt
Loc_BeginPrompt:
	if( (lpwszValue = GetTextConfigValue(pairs,CFG_BEGINPROMPT)) != NULL )
	{
		if( fAssumeYes == false )
		{
			if( SfxBeginPrompt( lpwszTitle, lpwszValue ) == FALSE )
			{
				return ERRC_CANCELED;
			}
			if( GetKeyState(VK_SHIFT)&0x8000 )
				fUseAutoInstall = fAssumeYes = true;
#ifdef _SFX_USE_BEGINPROMPTTIMEOUT
			if( CSfxDialog::IsTimedOut() != FALSE )
				GUIFlags &= ~GUIFLAGS_EXTRACT_PATH2;
#endif // _SFX_USE_BEGINPROMPTTIMEOUT
		}
	}

	// check autoinstall
	CSfxStringU	executeName;
	if( fUseAutoInstall != false )
	{
		LPCWSTR p1 = lpwszBatchInstall;
		do {
			executeName = CFG_AUTOINSTALL;
			executeName += *p1;
			if( GetTextConfigValue( pairs, executeName ) == NULL )
			{
				// auto install command not found
				SfxErrorDialog( FALSE, ERR_AUTOINSTALL_NOTFOUND, (LPCWSTR)executeName );
				return ERRC_BAD_AUTOINSTALL;
			}
			p1++;
		} while( (*p1 >= L'0' && *p1 <= L'9') ||
				 (*p1 >= L'a' && *p1 <= L'z') ||
				 (*p1 >= L'A' && *p1 <= L'Z') );
		executeName = CFG_AUTOINSTALL;
		executeName += *lpwszBatchInstall;
	}

	bool fUseExecuteFile = false;
	// check for 'ExecuteFile' (only if no 'autoinstall' commands)
	if( executeName.IsEmpty() != false && GetTextConfigValue( pairs, CFG_EXECUTEFILE ) != NULL )
	{
		executeName = CFG_EXECUTEFILE;
		fUseExecuteFile = true;
	}
	// if no 'ExecuteFile' - try 'RunProgram'
	if( executeName.IsEmpty() != false && GetTextConfigValue( pairs, CFG_RUNPROGRAM ) != NULL )
		executeName = CFG_RUNPROGRAM;

	// Path to extract
	if( fUseAutoInstall == false && fAssumeYes == false &&
			(GUIFlags&(GUIFLAGS_EXTRACT_PATH2|GUIFLAGS_EXTRACT_PATH1)) == GUIFLAGS_EXTRACT_PATH2 &&
				SfxExtractPathDialog( lpwszExtractPathTitle, lpwszExtractPathText ) == FALSE )
	{
		if( fUseBackward != false )
			goto Loc_BeginPrompt;
		return ERRC_CANCELED;
	}

	// 
	if( extractPath.IsEmpty() != false )
	{
		// create temp path
		extractPath = CreateTempName( L"7ZipSfx.%03x" );
		fUseInstallPath = false;
	}
	else
	{
		CSfxStringU tmp = extractPath;
		ReplaceVariables( tmp );
		if( tmp.IsEmpty() == false )
			extractPath = tmp;
		fUseInstallPath = true;
	}

	if( extractPath[extractPath.Len()-1] == L'\\' || extractPath[extractPath.Len()-1] == L'/' )
	{
		extractPath.ReleaseBuffer( extractPath.Len()-1 );
	}

	::SetCurrentDirectory( (LPCTSTR)extractPath );
	if( fAssumeYes != false )
		MiscFlags |= MISCFLAGS_NO_CHECK_DISK_FREE | MISCFLAGS_NO_CHECK_RAM;
#ifdef _SFX_USE_TEST
	HRESULT hrStatus;
	if( TSD_ExtractTimeout == 0 )
		hrStatus = ExtractArchive( inStreamSpec, extractPath );
	else
	{
		if( ExtractDialog() != FALSE )
			hrStatus = S_OK;
		else
			hrStatus = E_FAIL;
	}
#else
	HRESULT hrStatus = ExtractArchive( inStreamSpec, extractPath );
#endif // _SFX_USE_TEST

	// delete temporary folder
	if( hrStatus != S_OK )
	{
		SfxCleanup();
		return ERRC_EXTRACT;
	}

	//
	SetEnvironment();

	if( fNoRun == false || fUseInstallPath == false )
	{
		// execute program
		int	nExecuteIndex = 0;
		CSfxStringU	ustrDirPrefix;
		if( fUseAutoInstall == false )
			GetConfigDirectory( pairs, ustrDirPrefix );
		bool fUsedSetupExe = false;
		LPCWSTR p1 = lpwszBatchInstall;
		while( 1 )
		{
			#ifdef _SFX_USE_PREFIX_PLATFORM
				int	nPlatform = SFX_EXECUTE_PLATFORM_ANY;
			#endif // _SFX_USE_PREFIX_PLATFORM
#ifdef _SFX_USE_PREFIX_WAITALL
			bool	fUseWaitAll = false;
#endif // _SFX_USE_PREFIX_WAITALL
			CSfxStringU	ustrRunProgram;
			CSfxStringU	ustrConfigString;
			DWORD dwExecFlags = 0;

			if( executeName.IsEmpty() != false )
			{
				if( fUseInstallPath != false )
					break;
				lpwszValue = L"setup.exe";
				ustrRunProgram = extractPath + L"\\" + lpwszValue;
				if( ::GetFileAttributes( ustrRunProgram ) == (DWORD)-1 )
				{
					// nothing to execute
					SfxCleanup();
					SfxErrorDialog( FALSE, ERR_NO_SETUP_EXE );
					return ERRC_NOTHING_TO_EXECUTE;
				}
				fUsedSetupExe = true;
			}
			else
			{
				// 
				if( (lpwszValue = GetTextConfigValue( pairs, executeName, &nExecuteIndex )) == NULL )
				{
					p1++;
					if( (*p1 >= L'0' && *p1 <= L'9') ||
						(*p1 >= L'a' && *p1 <= L'z') ||
						(*p1 >= L'A' && *p1 <= L'Z') )
					{
						executeName = CFG_AUTOINSTALL;
						executeName += *p1;
						nExecuteIndex = 0;
						continue;
					}
					break;
				}
			}
			ustrConfigString = lpwszValue;
			ReplaceVariablesEx( ustrConfigString );
			lpwszValue = ustrConfigString;

			while( 1 )
			{
				LPCWSTR lpwszTmp;
#ifdef _SFX_USE_PREFIX_WAITALL
				if( (lpwszTmp = CheckPrefix( lpwszValue, L"waitall", CPF_NONE )) != NULL )
				{
					lpwszValue = lpwszTmp;
					fUseWaitAll = true;
					continue;
				}
#endif // _SFX_USE_PREFIX_WAITALL
#ifdef _SFX_USE_PREFIX_HIDCON
				if( (lpwszTmp = CheckPrefix( lpwszValue, L"hidcon", CPF_NONE )) != NULL )
				{
					lpwszValue = lpwszTmp;
					dwExecFlags |= SFXEXEC_HIDCON;
					continue;
				}
#endif // _SFX_USE_PREFIX_HIDCON
#ifdef _SFX_USE_PREFIX_NOWAIT
				if( (lpwszTmp = CheckPrefix( lpwszValue, L"nowait", CPF_NONE )) != NULL )
				{
					lpwszValue = lpwszTmp;
					// work only with InstallPath
					if( fUseInstallPath != false ) dwExecFlags |= SFXEXEC_NOWAIT;
					continue;
				}
#endif // _SFX_USE_PREFIX_NOWAIT
#ifdef _SFX_USE_PREFIX_FORCENOWAIT
				if( (lpwszTmp = CheckPrefix( lpwszValue, L"forcenowait", CPF_NONE )) != NULL )
				{
					lpwszValue = lpwszTmp;
					dwExecFlags |= SFXEXEC_NOWAIT;
					continue;
				}
#endif // _SFX_USE_PREFIX_FORCENOWAIT
				if( (lpwszTmp = CheckPrefix( lpwszValue, L"fm", CPF_NUMBER )) != NULL )
				{
					if( FinishMessage == -1 ) FinishMessage = StringToLong( lpwszValue+2 );
					lpwszValue = lpwszTmp;
					continue;
				}
				if( (lpwszTmp = CheckPrefix( lpwszValue, L"shc", CPF_SFX_INDEX )) != NULL )
				{
					ShortcutDefault = lpwszValue[3]-L'0';
					lpwszValue = lpwszTmp;
					continue;
				}
				if( (lpwszTmp = CheckPrefix( lpwszValue, L"del", CPF_SFX_INDEX )) != NULL )
				{
					DeleteDefault = lpwszValue[3]-L'0';
					lpwszValue = lpwszTmp;
					continue;
				}
#ifdef _SFX_USE_PREFIX_PLATFORM
				if( (lpwszTmp = CheckPrefix( lpwszValue, L"x86", CPF_NONE )) != NULL ||
						(lpwszTmp = CheckPrefix( lpwszValue, L"i386", CPF_NONE )) != NULL )
				{
					lpwszValue = lpwszTmp;
					nPlatform = SFX_EXECUTE_PLATFORM_I386;
					continue;
				}
				if( (lpwszTmp = CheckPrefix( lpwszValue, L"amd64", CPF_NONE )) != NULL ||
						(lpwszTmp = CheckPrefix( lpwszValue, L"x64", CPF_NONE )) != NULL )
				{
					lpwszValue = lpwszTmp;
					nPlatform = SFX_EXECUTE_PLATFORM_AMD64;
					continue;
				}
#endif // _SFX_USE_PREFIX_PLATFORM
				break;
			}
			
			if( fUseExecuteFile != false )
			{
				if( *lpwszValue != L'\"' )
				{
					ustrRunProgram = L"\""; ustrRunProgram += lpwszValue; ustrRunProgram += L"\"";
				}
				else
					ustrRunProgram = lpwszValue;
				if( (lpwszValue = GetTextConfigValue( pairs, CFG_EXECUTEPARAMETERS )) != NULL )
				{
					ustrRunProgram += L" "; ustrRunProgram += lpwszValue;
				}
			}
			else
			{
				// 'RunProgram' or 'AutoInstall'
				ustrRunProgram = ustrDirPrefix; ustrRunProgram += lpwszValue;
			}

			CSfxStringU filePath;
			CSfxStringU fileParams = LoadQuotedString( ustrRunProgram, filePath );
			ReplaceVariablesEx( filePath );
#ifdef _SFX_USE_TEST
			if( nTestModeType == 0 )
			{
#endif // _SFX_USE_TEST
#ifdef _SFX_USE_PREFIX_PLATFORM
			if( SfxPrepareExecute( nPlatform ) != FALSE )
			{
#endif // _SFX_USE_PREFIX_PLATFORM
				if( *str != L'\0' )
				{
//					fileParams += L' ';
					fileParams += str;
					while( *str != L'\0' ) str++;
				}
				ReplaceVariablesEx( fileParams );
				while( 1 )
				{
#ifdef _SFX_USE_PREFIX_WAITALL
					if( fUseWaitAll == false )
					{
#endif // _SFX_USE_PREFIX_WAITALL
						SetCurrentDirectory( (LPCTSTR)extractPath );
						CSfxStringU strExecuteString = L"\""+filePath + L"\" " + fileParams;
						if( SfxExecute( strExecuteString, dwExecFlags, extractPath ) != FALSE )
							break;
#ifdef _SFX_USE_PREFIX_WAITALL
					}
					else
					{
						DWORD dwExitCode = Parent_ExecuteSfxWaitAll( filePath, fileParams, dwExecFlags );
						if( dwExitCode == ERROR_SUCCESS )
							break;
						::SetLastError( dwExitCode );
					}
#endif // _SFX_USE_PREFIX_WAITALL
					SfxErrorDialog( TRUE, ERR_EXECUTE, (LPCWSTR)ustrRunProgram );
					SfxCleanup();
					return ERRC_EXECUTE;
				}
#ifdef _SFX_USE_PREFIX_PLATFORM
				SfxFinalizeExecute();
			}

#endif // _SFX_USE_PREFIX_PLATFORM
#ifdef _SFX_USE_TEST
			}
#endif // _SFX_USE_TEST
			if( fUseExecuteFile != false || fUsedSetupExe != false )
				break;
			ustrDirPrefix.Empty();
			nExecuteIndex++;
		}
	
#ifdef _SFX_USE_TEST
		if( nTestModeType == 0 )
		{
#endif // _SFX_USE_TEST
		// create shortcuts
#define SH_TEST
		SetEnvironment();
		ProcessPostExecuteSub( PostExecute_Shortcut, pairs, CFG_SHORTCUT, lpwszBatchInstall, ShortcutDefault );

		::SetCurrentDirectory( strSfxFolder );
		ProcessPostExecuteSub( PostExecute_Delete, pairs, CFG_DELETE, lpwszBatchInstall, DeleteDefault );

		SfxCleanup();
#ifdef _SFX_USE_TEST
		}
#endif // _SFX_USE_TEST
	}
	if( FinishMessage == -1 && fAssumeYes == false )
	{
			FinishMessage = 1;
	}
	if( FinishMessage > 0 && (lpwszValue = GetTextConfigValue( pairs, CFG_FINISHMESSAGE )) != NULL )
	{
		if( FinishMessage > FINISHMESSAGE_MAX_TIMEOUT )
			FinishMessage = FINISHMESSAGE_MAX_TIMEOUT;
		CSfxDialog_FinishMessage	dlg;
		dlg.Show( SD_OK|SD_ICONINFORMATION, lpwszTitle, lpwszValue );
	}

	// self delete
#ifdef _SFX_USE_TEST
	if( fNoRun == false && nTestModeType == 0 )
#else
	if( fNoRun == false )
#endif // _SFX_USE_TEST
	{
		switch( SelfDelete )
		{
		case 0:
			break;
		case 1:
			DeleteSFX( strModulePathName );
			break;
		default:
			if( (lpwszValue = GetTextConfigValue( pairs, CFG_SELFDELETE)) != NULL && lpwszValue[0] == L'1' )
				DeleteSFX( strModulePathName );
			break;
		}
	}
	
	return ERRC_NONE;
}

void DeleteSFX( CSfxStringU moduleName )
{
	WCHAR	szRoot[4];
	if( moduleName[1] == L':' && (moduleName[2] == L'\\' || moduleName[2] == L'/') )
	{
		szRoot[0] = moduleName[0];
		szRoot[1] = L':'; szRoot[2] = L'\\'; szRoot[3] = L'\0';
		if( ::GetDriveType(szRoot) != DRIVE_FIXED )
			return;
	}
	else
	{
		return;
	}

	CSfxStringU tmpPath = CreateTempName( L"7ZSfx%03x.cmd");

	HANDLE hfCmd = ::CreateFile( tmpPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hfCmd != INVALID_HANDLE_VALUE )
	{
		CSfxStringU cmdBody;
		cmdBody = L":Repeat\r\n";
		cmdBody += L"del \""; cmdBody += moduleName; cmdBody += L"\"\r\n";
		cmdBody += L"if exist \""; cmdBody += moduleName; cmdBody += L"\" goto Repeat\r\n";
		cmdBody += L"del \""; cmdBody += tmpPath; cmdBody += L"\"\r\n";
		DWORD dwWrited;
		BOOL fWrited = WriteFile( hfCmd, SfxUnicodeStringToMultiByte(cmdBody, CP_OEMCP), cmdBody.Len(), &dwWrited, NULL );
		::CloseHandle( hfCmd );
		if( fWrited == FALSE || dwWrited != (DWORD)cmdBody.Len() )
		{
			DeleteFileOrDirectoryAlways( tmpPath );
			return;
		}
		ClearFileAttributes( moduleName );
		// execute cmd
		ShellExecute( NULL, L"open", tmpPath, NULL, NULL, SW_HIDE );
	}
}
