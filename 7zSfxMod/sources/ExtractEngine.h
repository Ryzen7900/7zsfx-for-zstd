/*---------------------------------------------------------------------------*/
/* File:        ExtractEngine.h                                              */
/* Created:     Wed, 05 Oct 2005 17:35:00 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Last update: Tue, 29 Dec 2015 05:20:34 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Revision:    3737                                                         */
/*---------------------------------------------------------------------------*/
/* Revision:    1629                                                         */
/* Updated:     Mon, 22 Mar 2010 11:15:14 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Description: New file stamp.                                              */
/*---------------------------------------------------------------------------*/
#ifndef _EXTRACTENGINE_H_INCLUDED_
#define _EXTRACTENGINE_H_INCLUDED_

#ifdef SFX_CRYPTO
class CSfxPassword :
	public IArchiveOpenCallback,
	public ICryptoGetTextPassword,
	public CMyUnknownImp
{
public:
	MY_UNKNOWN_IMP1(ICryptoGetTextPassword)
	INTERFACE_IArchiveOpenCallback(;)

	static void Set( LPCWSTR lpwszPassword ) { m_strPassword = lpwszPassword; m_fPasswordDefined = true; }
	static LPCWSTR Get() { return m_strPassword; }
	static BOOL SetUI();
	static bool IsDefined() { return m_fPasswordDefined; }
#ifdef _SFX_USE_EARLY_PASSWORD
	static BOOL EarlyPassword(IInStream * inStream);
#endif // _SFX_USE_EARLY_PASSWORD

//	STDMETHOD(SetTotal)(const UInt64 *files, const UInt64 *bytes) { return S_OK; };
//	STDMETHOD(SetCompleted)(const UInt64 *files, const UInt64 *bytes) { return S_OK; };
	STDMETHOD(CryptoGetTextPassword)(BSTR *password);

private:
#ifdef SFX_CRYPTO
	static bool				m_fPasswordDefined;
	static CSfxStringU		m_strPassword;
#endif // SFX_CRYPTO
};
#endif // SFX_CRYPTO

class CSfxExtractEngine:
	public IArchiveExtractCallback,
#ifdef SFX_CRYPTO
	public ICryptoGetTextPassword,
#endif // SFX_CRYPTO
	public CMyUnknownImp
{
public:
	MY_UNKNOWN_IMP1(ICryptoGetTextPassword)

	// IProgress
	STDMETHOD(SetTotal)(UInt64 size);
	STDMETHOD(SetCompleted)(const UInt64 *completeValue);

	// IExtractCallback
	STDMETHOD(GetStream)(UInt32 index, ISequentialOutStream **outStream, Int32 askExtractMode);
	STDMETHOD(PrepareOperation)(Int32 askExtractMode);
	STDMETHOD(SetOperationResult)(Int32 resultEOperationResult);

	HANDLE	GetExtractThread() { return m_hExtractThread; };
	Int32	GetErrorCode() { return m_ErrorCode; };
	STDMETHOD(SayInternalError)( Int32 errc, DWORD dwLastError );

#ifdef SFX_CRYPTO
	STDMETHOD(CryptoGetTextPassword)(BSTR *password);
#endif // SFX_CRYPTO

private:
	
	CMyComPtr<IInArchive>	m_archiveHandler;
	CSfxStringU				m_directoryPath;
	BOOL					m_extractMode;
	COutFileStream *		m_outFileStreamSpec;
	CMyComPtr<ISequentialOutStream> m_outFileStream;
	CSfxStringU				m_diskFilePath;
	static HANDLE			m_hExtractThread;
	static Int32			m_ErrorCode;

	struct CProcessedFileInfo
	{
		FILETIME	UTCLastWriteTime;
		BOOL		IsDirectory;
		UInt32		Attributes;
	} m_processedFileInfo;

public:
	static HRESULT WINAPI ExtractThread( CSfxExtractEngine * pThis );
	HRESULT Extract( IInArchive * archive, LPCWSTR lpwszFolderName );
};

#endif // _EXTRACTENGINE_H_INCLUDED_
