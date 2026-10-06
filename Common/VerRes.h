#pragma once

#pragma comment(linker,"/DEFAULTLIB:version.lib")

#define		DefineStringResource(name)											\
CString		Get##name(void) const {												\
	CString		key;													\
	key.Format(_T("\\StringFileInfo\\%04X%04X\\")#name,*m_pLangID,*(m_pLangID+1));						\
	TCHAR		*p;													\
	UINT		tmp;													\
	if (::VerQueryValue(m_pBlock,(LPSTR)(LPCSTR)key,(void**)&p,&tmp)) {							\
		CString		rc(p,tmp);											\
		rc.ReleaseBuffer();												\
		return rc;													\
	}															\
	return _T("");														\
}

class CVerRes {
protected:
	void			*m_pBlock;
	VS_FIXEDFILEINFO	*m_pFixedFileInfo;
	WORD			*m_pLangID;
	BOOL		Create(LPCSTR lpszFileName) {
				DWORD		dwSize=::GetFileVersionInfoSize((LPSTR)lpszFileName,0);
				m_pBlock=new char[dwSize];
				UINT		tmp,tmp2;
				if (!::GetFileVersionInfo((LPSTR)lpszFileName,0,dwSize,m_pBlock)||
						!::VerQueryValue(m_pBlock,_T("\\"),(void**)&m_pFixedFileInfo,&tmp)||
						!::VerQueryValue(m_pBlock,_T("\\VarFileInfo\\Translation"),(void**)&m_pLangID,&tmp2)) {
					delete [] m_pBlock;
					m_pBlock=NULL;
					return FALSE;
				}
				return TRUE;
			}
public:
			CVerRes(HINSTANCE hInstance=AfxGetApp()->m_hInstance) {
				TCHAR		lpszFileName[MAX_PATH];
				::GetModuleFileName(hInstance,lpszFileName,sizeof(lpszFileName));
				Create(lpszFileName);
			}
			CVerRes(LPCSTR lpszFileName) { Create(lpszFileName); }
	virtual		~CVerRes(void) { delete [] m_pBlock; }
	DefineStringResource(FileVersion);
	DefineStringResource(ProductVersion);
	DefineStringResource(Comments);
	DefineStringResource(CompanyName);
	DefineStringResource(FileDescription);
	DefineStringResource(InternalName);
	DefineStringResource(LegalCopyright);
	DefineStringResource(LegalTrademarks);
	DefineStringResource(OriginalFilename);
	DefineStringResource(PrivateBuild);
	DefineStringResource(ProductName);
	DefineStringResource(SpecialBuild);
};
