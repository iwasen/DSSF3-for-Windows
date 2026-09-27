#pragma once

#include "FileIO.h"

class CSasDoc : public CDocument
{
public:
	long m_nFolderID;
	int m_nType;

	void SelectSystem();
	void SelectFolder(int folderID);
	void SelectDB(char type);
	void DeleteItem(long nItemID);
	void ChangeTitle(long nItemID, LPCTSTR pTitle);
	void Export(long *pIDs, int nItem, LPCTSTR pItemName) const;
	void Import(long nID);

protected: // シリアライズ機能のみから作成します。
	CSasDoc();
	DECLARE_DYNCREATE(CSasDoc)

	CString m_sFolderName;

	void ParamCalc(int nCalcData);
	void CalcIR(int nCalcData);
	void CalcAcf(int nCalcData);
	void CalcNms(int nCalcData);
	static BOOL AbortCheck(int nPercent);
	void DeleteFolder(long nFolderID);
	void DeleteData(long nDataID);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnNewDocument();
	afx_msg void OnViewUpdate();
	afx_msg void OnParamCalc();
	afx_msg void OnUpdateParamCalc(CCmdUI* pCmdUI);
	afx_msg void OnParamOutput();
	afx_msg void OnUpdateParamOutput(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnTemplate();
	afx_msg void OnUpdateTemplate(CCmdUI* pCmdUI);
	afx_msg void OnParamCalc2();
};
