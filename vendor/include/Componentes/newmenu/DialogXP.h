#pragma once
# include <afxdialogex.h>

class  __COMPONENTES_EXPORT__ CDialogXP : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogXP)
	
	CDialogXP(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	CDialogXP(UINT nIDTemplate, CWnd* pParentWnd = NULL);

public:
	CDialogXP(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogXP();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	// nuevo para el portapapeles
	afx_msg void OnEditPaste();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditUndo();
	afx_msg void OnEditClear();
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditClear(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditSelectAll(CCmdUI *pCmdUI);
};
