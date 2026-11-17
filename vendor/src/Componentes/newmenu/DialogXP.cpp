// DialogXP.cpp : implementation file
//

#include "stdafx.h"
#include "newmenu/DialogXP.h"

// CDialogXP dialog

IMPLEMENT_DYNAMIC(CDialogXP, CDialogEx)
CDialogXP::CDialogXP(CWnd* pParent /*=NULL*/)
{
}

CDialogXP::~CDialogXP()
{
}

void CDialogXP::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

CDialogXP::CDialogXP(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
  ASSERT(IS_INTRESOURCE(lpszTemplateName) || AfxIsValidString(lpszTemplateName));

  m_pParentWnd = pParentWnd;
  m_lpszTemplateName = lpszTemplateName;
  if (IS_INTRESOURCE(m_lpszTemplateName))
    m_nIDHelp = LOWORD((DWORD_PTR)m_lpszTemplateName);
}

CDialogXP::CDialogXP(UINT nIDTemplate, CWnd* pParentWnd)
{
	m_pParentWnd = pParentWnd;
	m_lpszTemplateName = MAKEINTRESOURCE(nIDTemplate);
	m_nIDHelp = nIDTemplate;
}


BEGIN_MESSAGE_MAP(CDialogXP, CDialogEx)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
END_MESSAGE_MAP()

// CDialogXP message handlers
void CDialogXP::OnEditPaste()
{
  GetFocus()->SendMessage(WM_PASTE);
}

void CDialogXP::OnEditCopy()
{
  GetFocus()->SendMessage(WM_COPY);
}

void CDialogXP::OnEditCut()
{
  GetFocus()->SendMessage(WM_CUT);
}

void CDialogXP::OnEditUndo()
{
  GetFocus()->SendMessage(WM_UNDO);
}

void CDialogXP::OnEditClear()
{
  GetFocus()->SendMessage(WM_CLEAR);
}

void CDialogXP::OnEditSelectAll()
{
  CWnd *focus= GetFocus();
  if( focus) {
	  if( focus->IsKindOf(RUNTIME_CLASS(CEdit))) 
		  ((CEdit *)focus)->SetSel(0, focus->GetWindowTextLength());
	  else  if( focus->IsKindOf(RUNTIME_CLASS(CRichEditCtrl))) 
		  ((CRichEditCtrl*) focus)->SetSel(0, focus->GetWindowTextLength());
	  else focus->SendMessage(WM_COMMAND, ID_EDIT_SELECT_ALL,0);
  }
}

void CDialogXP::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
  CWnd *focus=GetFocus();
  bool enable=false;
  if( focus && focus->IsKindOf(RUNTIME_CLASS(CEdit))) {
	  if(!(focus->GetStyle()&ES_READONLY)) {
		  COleDataObject	obj;
		  if (obj.AttachClipboard() && obj.IsDataAvailable(CF_TEXT))
			  enable=true;
	  }
  } else enable=true;
  pCmdUI->Enable( enable);
}

void CDialogXP::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
  bool enable= false;
  CWnd *focus= GetFocus();
  if( focus) 
	  if(focus->IsKindOf(RUNTIME_CLASS(CEdit))) 
		  enable= ((CEdit *)focus)->CanUndo();
	  else if( focus->IsKindOf(RUNTIME_CLASS(CListCtrl)))
		  enable= ((CListCtrl*)focus)->GetItemCount()!=0;
	  else enable=true;
	  pCmdUI->Enable(enable);
}

void CDialogXP::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
  CWnd *focus= GetFocus();
  int x=0, y=0;
  if( focus) 
	  if(focus->IsKindOf(RUNTIME_CLASS(CEdit)) || focus->IsKindOf(RUNTIME_CLASS(CRichEditCtrl)))
		  ((CEdit *)focus)->GetSel(x,y);
	  else if( focus->IsKindOf(RUNTIME_CLASS(CListCtrl)))
		  y= ((CListCtrl *)focus)->GetItemCount();
	  else y= 1;
	  pCmdUI->Enable(x!=y);
}

void CDialogXP::OnUpdateEditCut(CCmdUI *pCmdUI)
{
  CWnd *focus= GetFocus();
  int x=0, y=0;
  if( focus) {
	  if(focus->IsKindOf(RUNTIME_CLASS(CEdit)) || focus->IsKindOf(RUNTIME_CLASS(CRichEditCtrl))) {
		  if(!(focus->GetStyle()&ES_READONLY)) {
			  ((CEdit *)focus)->GetSel(x,y);
		  }
	  } else if( focus->IsKindOf(RUNTIME_CLASS(CListCtrl))) {
		  y=((CListCtrl *)focus)->GetItemCount();
	  } else y=1;
  }
  pCmdUI->Enable(x!=y);
}

void CDialogXP::OnUpdateEditClear(CCmdUI *pCmdUI)
{
  bool enable=false;
  CWnd *focus= GetFocus();
  if( focus) {
	  if( focus->IsKindOf(RUNTIME_CLASS(CEdit))) {
		  if(!(focus->GetStyle()&ES_READONLY)) {
			  enable= focus->GetWindowTextLength()>0;
		  }
	  } else if( focus->IsKindOf(RUNTIME_CLASS(CListCtrl))) {
		  enable= ((CListCtrl *)focus)->GetItemCount()!=0;
	  } else enable=true;
  }
  pCmdUI->Enable(enable);
}

void CDialogXP::OnUpdateEditSelectAll(CCmdUI *pCmdUI)
{
  bool enable=false;
  CWnd *focus= GetFocus();
  if( focus) {
	  if(focus->IsKindOf(RUNTIME_CLASS(CEdit)))
		  enable= focus->GetWindowTextLength()>0;
	  else if (focus->IsKindOf(RUNTIME_CLASS(CListCtrl)))
		  enable= ((CListCtrl *)focus)->GetItemCount()!=0;
	  else if( focus->IsKindOf(RUNTIME_CLASS(CRichEditCtrl)))
		  enable= focus->GetWindowTextLength()>0;
	  else enable=true;
  }
  pCmdUI->Enable(enable);
}
