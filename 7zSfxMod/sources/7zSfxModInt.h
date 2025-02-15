/*---------------------------------------------------------------------------*/
/* File:        7zSfxModInt.h                                                */
/* Created:     Wed, 25 Jul 2007 09:54:00 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Last update: Tue, 29 Dec 2015 22:57:44 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Revision:    3080                                                         */
/*---------------------------------------------------------------------------*/
/* Revision:    1641                                                         */
/* Updated:     Fri, 20 Jan 2012 22:47:56 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Description: Add 'safe_SHBrowseForFolder' that ignore                     */
/*              RPC_S_SERVER_UNAVAILABLE exception on x64 platform           */
/*---------------------------------------------------------------------------*/
/* Revision:    1067                                                         */
/* Updated:     Sat, 26 Jun 2010 04:22:23 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Description: New prefix 'waitall' stuf                                    */
/*---------------------------------------------------------------------------*/
/* Revision:    1047                                                         */
/* Updated:     Sun, 06 Jun 2010 07:47:34 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Description: Add warnings stuff                                           */
/*---------------------------------------------------------------------------*/
/* Revision:    972                                                          */
/* Updated:     Mon, 22 Mar 2010 11:08:53 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Description: New file stamp.                                              */
/*---------------------------------------------------------------------------*/
#ifndef _7ZSFXMODINT_H_INCLUDED_
#define _7ZSFXMODINT_H_INCLUDED_

#include "7zSfxMod.h"
#include "langstrs.h"
#include "resource.h"
#include "version.h"
#include "7Zip/Archive/7z/7zHandler.h"

class CSfxStringA : public AString
{
public:
	CSfxStringA() : AString() {};
	CSfxStringA(char c) : AString(c) {};
	operator const char *() const { return Ptr(); }
	CSfxStringA &operator=(const char *s) { *((AString *)this)=s; return *this; };
	int Length() const { return (int)(AString::Len()); }
	char * GetBuffer(unsigned minLen)
	{
		CSfxStringA tmp = *this;
		strncpy( GetBuf(minLen), tmp.Ptr(), tmp.Len() );
		return (char *)Ptr();
	};

	void ReleaseBuffer() { ReleaseBuffer( MyStringLen(Ptr()) ); };
	void ReleaseBuffer(unsigned newLen) { ReleaseBuf_SetEnd(newLen); };
};

class CSfxStringU : public UString
{
public:
	CSfxStringU::CSfxStringU() : UString() {};
	CSfxStringU::CSfxStringU(const wchar_t *s) : UString(s){};
	int Length() const { return (int)(UString::Len()); }
	CSfxStringU &operator=(const wchar_t *s) { *((UString *)this)=s; return *this; };
	void ReleaseBuffer() { ReleaseBuffer( MyStringLen(Ptr()) ); };
	void ReleaseBuffer(unsigned newLen) { ReleaseBuf_SetEnd(newLen); };
	wchar_t * GetBuffer(unsigned minLen)
	{
		CSfxStringU tmp = *this;
		wcsncpy( GetBuf(minLen), tmp.Ptr(), tmp.Len() );
		return (wchar_t *)Ptr();
	};
};

struct CTextConfigPair
{
	CSfxStringU ID;
	CSfxStringU String;
};

namespace SfxErrors
{
	enum
	{
		sePropVariant1 = 100,
		sePropVariant2,
		sePropVariant3,
		seAnti,
		seCreateFolder,
		seOverwrite,
		seCreateFile,
#ifdef SFX_CRYPTO
		seNoPassword,
#endif // SFX_CRYPTO
	};
}

extern char kSignatureConfigStart[];
extern char kSignatureConfigEnd[];
extern const UInt64 kMaxCheckStartPosition;

extern LPCWSTR	lpwszTitle;
extern LPCWSTR	lpwszErrorTitle;
extern LPCWSTR	lpwszCancelText;
extern LPCWSTR	lpwszExtractTitle;
extern LPCWSTR	lpwszExtractPathTitle;
extern LPCWSTR	lpwszExtractPathText;
extern LPCWSTR	lpwszExtractDialogText;
extern LPCWSTR	lpwszCancelPrompt;
extern int		GUIMode;
extern int		GUIFlags;
extern int		MiscFlags;
extern LANGID	idSfxLang;
extern int		ExtractDialogWidth;
extern int		ExtractPathWidth;
extern HWND		hwndExtractDlg;
extern int		FinishMessage;
extern bool		fUseBackward;
#ifdef _SFX_USE_BEGINPROMPTTIMEOUT
	extern int		BeginPromptTimeout;
#endif // _SFX_USE_BEGINPROMPTTIMEOUT
#ifdef SFX_CRYPTO
	extern LPCWSTR	lpwszPasswordTitle;
	extern LPCWSTR	lpwszPasswordText;
#endif // SFX_CRYPTO
#ifdef _SFX_USE_VOLUME_NAME_STYLE
	extern int nVolumeNameStyle;
#endif // _SFX_USE_VOLUME_NAME_STYLE


#ifdef _SFX_USE_PREFIX_PLATFORM
	extern CSfxStringU	strOSPlatform;
#endif // _SFX_USE_PREFIX_PLATFORM
extern CSfxStringU	strModulePathName;
extern CSfxStringU	extractPath;
extern int			OverwriteMode, OverwriteFlags;

// ExtractDlg
extern HWND		hwndExtractDlg;
extern BOOL		fCancelExtract;

class CSfxExtractEngine;
extern CSfxExtractEngine * SfxExtractEngine;
extern CObjectVector<CTextConfigPair> Variables;
extern HMODULE	hKernel32;

#ifdef _SFX_USE_WARNINGS
	extern LPCWSTR lpwszWarningTitle;
	INT_PTR ShowSfxWarningDialog( LPCWSTR lpwszMessage );
#endif // _SFX_USE_WARNINGS
void	ShowSfxErrorDialog( LPCWSTR lpwszMessage );
void	SfxErrorDialog( BOOL fUseLastError, UINT idFormat, ... );
BOOL	DeleteFileOrDirectoryAlways( LPCWSTR lpwszPathName );
BOOL	SfxCreateDirectory( LPCWSTR lpwszPath );

BOOL	CreateFolderTree( LPCWSTR lpwszPath );
BOOL	CreateShortcut( LPCTSTR lpszShortcutData );
void	DisableXPStyles( HWND hwnd );
void	GetDlgItemRect( HWND hwndDlg, int nIDItem, LPRECT rc );
void	ReplaceWithExtractPath( CSfxStringU& str, CSfxStringU &extractPath );
void	ReplaceWithArchivePath( CSfxStringU& str, CSfxStringU &archivePath );
void	ReplaceWithArchiveName( CSfxStringU& str, CSfxStringU &archiveName );
void	HookForExtractPath( HWND hwnd, BOOL fFromBeginPrompt );

bool	GetTextConfig( const CSfxStringA &string, CObjectVector<CTextConfigPair> &pairs, bool fromCmdLine );
LPCWSTR GetTextConfigValue( const CObjectVector<CTextConfigPair> &pairs, LPCWSTR id, int * pFrom = NULL );
BOOL	SfxExtractPathDialog( LPCWSTR lpwszTitle, LPCWSTR lpwszText );
void	ExpandEnvironmentStrings( CSfxStringU & ustr );
int		GetOverwriteMode( LPCWSTR lpwszPath, FILETIME * fileTime );
int		MyStrincmp( LPCWSTR str1, LPCWSTR str2, int nLength );
CSfxStringU GetWindowStringU( HWND hwnd );
BOOL	ReplaceVariablesInWindow( HWND hwnd );
BOOL	GetChildRect( HWND hwnd, LPRECT rc );
void	ReplaceHexChars( CSfxStringU& str );
#ifdef _SFX_USE_RTF_CONTROL
	HWND RecreateAsRichEdit( HWND hwndStatic );
#endif // _SFX_USE_RTF_CONTROL
#ifdef _SFX_USE_IMAGES
	BOOL SetDlgControlImage( HWND hwndControl );
#endif // _SFX_USE_IMAGES

#ifdef _SFX_USE_ENVIRONMENT_OLD_VARS
	void SfxAddEnvironmentVarInternal( LPCWSTR lpwszName, LPCWSTR lpwszValue, BOOL fUseAlias );
	#define SfxAddEnvironmentVar(a,b)			SfxAddEnvironmentVarInternal((a),(b),FALSE)
	#define SfxAddEnvironmentVarWithAlias(a,b)	SfxAddEnvironmentVarInternal((a),(b),TRUE)
#else
	void SfxAddEnvironmentVar( LPCWSTR lpwszName, LPCWSTR lpwszValue );
	#define SfxAddEnvironmentVarWithAlias	SfxAddEnvironmentVar
#endif // _SFX_USE_ENVIRONMENT_OLD_VARS

//long	StringToLong( LPCWSTR nptr );
#define StringToLong	_wtol

CSfxStringU SfxMultiByteToUnicodeString( const CSfxStringA &srcString, UINT codePage );
CSfxStringA SfxUnicodeStringToMultiByte( const CSfxStringU &srcString, UINT codePage );

BOOL ExtractDialog();

#define SFX_OM_ERROR		-1
#define SFX_OM_OVERWRITE	0
#define SFX_OM_SKIP			1

#define ClearFileAttributes(path)		SetFileAttributes(path,0)

void SfxDialog_InitHooks();
UINT SfxDialog( LPCWSTR lpwszCaption, LPCWSTR lpwszText, UINT uType,
				WNDPROC WindowProc = NULL, HWND hwndOwner = NULL, int nIconFlag = GUIFLAGS_USEICON );
BOOL SfxBeginPrompt( LPCWSTR lpwszCaption, LPCWSTR lpwszText );
void SetFontFromDialog( HWND hwndTarged, HWND hwndDialog );
BOOL CancelInstall( HWND hwnd );

#define WM_7ZSFX_SETTOTAL		(WM_APP+1)
#define WM_7ZSFX_SETCOMPLETED	(WM_APP+2)

#define FINISHMESSAGE_MAX_TIMEOUT	999

#ifdef _SFX_USE_TEST
	
	extern	CSfxStringU	TSD_Flags;
	extern	DWORD		TSD_ExtractTimeout;
	extern	int			nTestModeType;

	int TestSfxDialogs( CObjectVector<CTextConfigPair>& config );
	int TestSfxDialogsToStdout( CObjectVector<CTextConfigPair>& config );
	void WriteStdoutChar( WCHAR wc );

#endif // _SFX_USE_TEST

BOOL IsRunAsAdmin();
int GetDirectorySeparatorPos( CSfxStringU& ustrPath );
void CreateDummyWindow();
void SfxCleanup();
void ReplaceVariablesEx( CSfxStringU& str );
CSfxStringU MyGetEnvironmentVariable( LPCWSTR lpwszName );
#ifdef _SFX_USE_LANG
	UINT GetUILanguage();
#endif // _SFX_USE_LANG
#ifdef _SFX_USE_PREFIX_PLATFORM
	BOOL SfxPrepareExecute( int nPlatform );
	void ReplaceWithPlatform( CSfxStringU& str );
	#if defined(_WIN64) && defined(_M_X64)
		#define SfxFinalizeExecute()
	#else
		void SfxFinalizeExecute();
	#endif // defined(_WIN64) && defined(_M_X64)
#endif // _SFX_USE_PREFIX_PLATFORM

void CreateConfigSignature(
#ifdef _SFX_USE_LANG
			DWORD dwLangId,
#endif // _SFX_USE_LANG
#ifdef _SFX_USE_CONFIG_PLATFORM
			LPCSTR lpszPlatformName,
#endif // _SFX_USE_CONFIG_PLATFORM
			CSfxStringA& strBegin, CSfxStringA& strEnd );
bool LoadConfigs( IInStream * inStream, CSfxStringA& result );


#define SFXEXEC_HIDCON		0x01
#define SFXEXEC_RUNAS		0x02
#define SFXEXEC_EXT_MASC	0x07
#define SFXEXEC_NOWAIT		0x10000

#define SetLastWriteTime	SetMTime

#ifdef _SFX_USE_PREFIX_PLATFORM
	#define SFX_EXECUTE_PLATFORM_ANY		0
	#define SFX_EXECUTE_PLATFORM_I386		1
	#define SFX_EXECUTE_PLATFORM_AMD64		2
#endif // _SFX_USE_PREFIX_PLATFORM

#if defined(_SFX_USE_PREFIX_PLATFORM) || defined(_SFX_USE_ENVIRONMENT_VARS)
	LPCWSTR GetPlatformName();
#endif // _SFX_USE_PREFIX_PLATFORM || _SFX_USE_ENVIRONMENT_VARS

#define CMDLINE_SFXWAITALL			_CFG_PARAM_TYPE"sfxwaitall"
#define CMDLINE_SFXELEVATION		_CFG_PARAM_TYPE"sfxelevation"

#if !defined(SFX_VOLUMES) && !defined(SFX_PROTECT)
	#define CSfxInStream CInFileStream
#else
	#include "7zip/Archive/Common/MultiStream.h"
	class CSfxInStream : public CMultiStream
	{
	public:
#ifdef SFX_VOLUMES
		bool Open(LPCTSTR fileName);
		bool InitVolumes();
		IInStream * GetVolumesStream();

	private:
		HRESULT OpenSubStream( LPCTSTR fileName, CSubStreamInfo * subStream );
#endif // SFX_VOLUMES
	};
#endif // !defined(SFX_VOLUMES) && !defined(SFX_PROTECT)

#ifdef _SFX_USE_CUSTOM_EXCEPTIONS
	extern UINT_PTR uSfxExceptionText;
	#define SFX_EXCEPTION_HANDLER_BEGIN(n)	uSfxExceptionText=(UINT_PTR)(n);
	#define SFX_EXCEPTION_HANDLER_END		uSfxExceptionText=0;
#else
	#define SFX_EXCEPTION_HANDLER_BEGIN(n)
	#define SFX_EXCEPTION_HANDLER_END
#endif // _SFX_USE_CUSTOM_EXCEPTIONS

#if defined(_SFX_USE_CUSTOM_EXCEPTIONS) && defined(_WIN64)
	LPITEMIDLIST safe_SHBrowseForFolder( LPBROWSEINFO lpbi );
#else
	#define safe_SHBrowseForFolder	SHBrowseForFolder
#endif // defined(_SFX_USE_CUSTOM_EXCEPTIONS) && defined(_WIN64)

#endif // _7ZSFXMODINT_H_INCLUDED_
