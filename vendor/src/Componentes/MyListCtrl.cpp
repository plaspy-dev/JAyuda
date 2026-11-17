// MyListCtrl.cpp : implementation file
//

#include "stdafx.h"
# include "Componentes.h"
#include "MyListCtrl.h"
# include "xmlreader.h"
# include "EditFloat.h"
# include "draw.h"
# include "SortClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CListCtrlXP, CMFCListCtrl)

CListCtrlXP::CListCtrlXP()
{
	FUNCTION_STACK_TRACE("FFFFE07D");

	m_nMaxRow= 0;
	m_nRow = -1;
	m_nMaxCol= 0;
	m_nCol = -1;
	m_bScrolling= false;
	m_bCanClean= false;
	
	m_bOrdenable= false;
	m_bEditable= false;
	m_pParent= NULL;
	m_szVacia= _T("No hay elementos");
	m_nNoSort= -1;
	m_hMenuContextual= NULL;
	m_bCanShowMenu= false;
	m_bDropDown=false;
}

CListCtrlXP::~CListCtrlXP()
{
	FUNCTION_STACK_TRACE("FFFFE07E");
	CWnd *handle;
	for( int i=0; i<m_pArrayControles.GetCount(); i++) {

		handle= (CWnd *) m_pArrayControles.GetAt(i);
		delete handle;
	}
	if( m_hMenuContextual) 
		delete m_hMenuContextual;
}

BEGIN_MESSAGE_MAP(CListCtrlXP, CMFCListCtrl)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY_REFLECT(LVN_INSERTITEM, OnLvnInsertitem)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnLvnDeleteitem)
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnLvnColumnclick)
	ON_NOTIFY_REFLECT(LVN_DELETEALLITEMS, OnLvnDeleteallitems)
	ON_WM_PAINT()
	ON_MESSAGE(WM_REALUPDATECOMBO, OnRealUpdateCombo)
	ON_WM_SETFOCUS()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnSelAll)
	ON_MESSAGE(WM_CLEAR, OnClear)
	ON_MESSAGE(WM_COPY, OnCopy)
	ON_MESSAGE(WM_CUT, OnCopyColumn)
	ON_MESSAGE(WM_UNDO, OnCopyValue)
	ON_WM_RBUTTONDOWN()
	ON_WM_DRAWITEM()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

int	CListCtrlXP::AddItem(LPCTSTR lpszItem) {
	
	return InsertItem(-1, lpszItem, -1);
}

int CListCtrlXP::InsertItem(int nItem,LPCTSTR lpszItem) {
	return InsertItem(nItem, lpszItem, -1);
}

int CListCtrlXP::InsertItem(int nItem,LPCTSTR lpszItem,int nImage) {
	
	if(nItem==-1)
		nItem= GetItemCount();
	return CMFCListCtrl::InsertItem(nItem, lpszItem, nImage);
}

void CListCtrlXP::SetEditable(bool editable) {

	FUNCTION_STACK_TRACE("FFFFE07F");
	if( editable) {
	
		if( m_pArrayControles.IsEmpty())
			return;
	}
	m_bEditable= editable;
}

void  CListCtrlXP::GetCurrentPos(int &col, int &row) {

	FUNCTION_STACK_TRACE("FFFFE080");
	col= m_nCol;
	row= m_nRow;
}

LRESULT CListCtrlXP::OnRealUpdateCombo(WPARAM wParam, LPARAM lParam) {

	FUNCTION_STACK_TRACE("FFFFE081");
	if( !m_bEditable)
		return 0;
	CComboBoxXP *combo=(CComboBoxXP *)GetControl(m_nCol);
	if( combo) {
		if( m_pParent) {

			// nueva notificaion
			Notify(LVN_ITEMCHANGING, m_nRow, m_nCol);
			// obsoleta
			m_pParent->SendMessage(WM_LISTACOMBO, MAKELONG(m_nCol, m_nRow), lParam);
		}
	}
	return 0;
}

void CListCtrlXP::SetParent(CWnd *parent) {

	FUNCTION_STACK_TRACE("FFFFE082");
	m_pParent= parent;
}

void CListCtrlXP::ResetPos() {
    m_nCol=-1;
	m_nRow=-1;
	HideAll();
}

void CListCtrlXP::LoadFromXML( CString Archivo) {

	FUNCTION_STACK_TRACE("FFFFE083");
	XmlReader r;
	r.Open(Archivo);
	int i=0;

	while( r.ReadString()) {

		switch( r.GetNodeType()) {
			case NODETYPE_ATTRIB:
				{
				CString szName, szValue;
				r.GetAttrib(szName, szValue);
				szName = GetUnescapedXmlString(szName);
				szValue = GetUnescapedXmlString(szValue);

				CString szCurrentElement;
				r.GetNodeName(szCurrentElement);
				if( szCurrentElement.CompareNoCase( "item")==0) {

					if(szName.CompareNoCase("nombre")==0) {

						InsertItem( i, szValue);
						i++;
						}
				}
				}
		}
	}
	r.Close();
}

BOOL CListCtrlXP::GetItemRect(_In_ int nItem, _Out_ LPRECT lpRect, _In_ UINT nCode) const
{
	return CListCtrl::GetItemRect(nItem, lpRect, nCode);
}

BOOL CListCtrlXP::GetSubItemRect(_In_ int iItem, _In_ int iSubItem, _In_ int nArea, _Out_ CRect& ref) const {

	return CListCtrl::GetSubItemRect(iItem, iSubItem, nArea, ref);
}

CComboBoxXP	*CListCtrlXP::SetCombo( unsigned int col) {

	FUNCTION_STACK_TRACE("FFFFE084");
	CComboBoxXP	*pComboBox = new CComboBoxXP();
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_CLIPSIBLINGS|WS_BORDER|CBS_DROPDOWN|WS_VSCROLL|WS_HSCROLL|CBS_NOINTEGRALHEIGHT | CBS_SORT;
	CRect rect;
	GetSubItemRect(0,col,LVIR_LABEL,rect);
	pComboBox->Create(dwStyle,rect,this,1000+col);
	pComboBox->ValidateScroll(false);
	pComboBox->ShowWindow(SW_HIDE);
	m_pArrayControles.Add(pComboBox);
	m_pArrayColumnas.Add(col);
	if( m_nMaxCol< col)
		m_nMaxCol=col;
	if(!m_pParent)
		m_pParent=GetParent();
	m_bEditable= true;
	return pComboBox;
}

CDateTimeCtrl * CListCtrlXP::SetDateTimePicker( unsigned int col, bool shownone/*=false*/) {

	FUNCTION_STACK_TRACE("FFFFE085");
	CDateTimeCtrl *pDate= new CDateTimeCtrl();
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_TABSTOP|WS_BORDER | DTS_RIGHTALIGN;
	if(shownone)
		dwStyle|= DTS_SHOWNONE;
	CRect rect;
	GetSubItemRect(0,col,LVIR_LABEL,rect);
	pDate->Create(dwStyle,rect,this,1010+col);
	pDate->ShowWindow(SW_HIDE);
	// pDate->SetFont(GetFont());
	m_pArrayControles.Add(pDate);
	m_pArrayColumnas.Add(col);
	if( m_nMaxCol< col)
		m_nMaxCol=col;
	if(!m_pParent)
		m_pParent=GetParent();
	m_bEditable= true;
	return pDate;
}

CEdit *CListCtrlXP::SetEdit( unsigned int col, int tipo/*=0*/) {

	FUNCTION_STACK_TRACE("FFFFE085");
	CEdit *pEdit=NULL;
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_TABSTOP|WS_BORDER|ES_LEFT;

	if(tipo==tipocol_double) {
		dwStyle|= ES_RIGHT | ES_AUTOHSCROLL;
		pEdit= new CEditFloat();
	} else {
		if(tipo==tipocol_textomultiline) {
			dwStyle|= ES_MULTILINE | ES_AUTOVSCROLL;
			pEdit = new CEditXP();
		} else {
			dwStyle|=ES_AUTOHSCROLL;
			if(tipo==tipocol_numero) 
				dwStyle|= ES_RIGHT | ES_NUMBER;
			pEdit = new CmEdit();
		}
	}

	CRect rect;
	GetSubItemRect(0,col,LVIR_LABEL,rect);
	pEdit->Create(dwStyle,rect,this,1010+col);
	pEdit->ShowWindow(SW_HIDE);
	pEdit->SetFont(GetFont());
	m_pArrayControles.Add(pEdit);
	m_pArrayColumnas.Add(col);
	if( m_nMaxCol< col)
		m_nMaxCol=col;
	if(!m_pParent)
		m_pParent=GetParent();
	m_bEditable= true;
	return pEdit;
}

void CListCtrlXP::HideAll() {

	FUNCTION_STACK_TRACE("FFFFE086");
	for( int i=0; i<m_pArrayControles.GetCount(); i++) 

		((CWnd *)m_pArrayControles.GetAt(i))->ShowWindow(SW_HIDE);
}

CWnd *CListCtrlXP::GetControl(int col) {
	
	FUNCTION_STACK_TRACE("FFFFE087");
	CWnd *info=NULL;
	for( int i=0; i<m_pArrayColumnas.GetCount(); i++) {

		if( m_pArrayColumnas.GetAt(i)==col) {

			info= (CWnd *) m_pArrayControles.GetAt(i);
			break;
		}
	}
	return info;
}

int	CListCtrlXP::GetTipoControl(unsigned int col) {

	FUNCTION_STACK_TRACE("FFFFE088");
	CWnd *handle= GetControl(col);
	if( handle) {
		if( handle->IsKindOf(RUNTIME_CLASS(CEditXP)))
			return tipo_edit_edit;
		else if(handle->IsKindOf(RUNTIME_CLASS(CComboBox)))
			return tipo_edit_combo;
		return tipo_edit_datetime;
	}
	return 0;
}

BOOL CListCtrlXP::PreTranslateMessage(MSG* pMsg) 
{

	if( m_bEditable && GetItemCount()) {
		if(pMsg->message == WM_KEYDOWN)
		{
			if( pMsg->wParam== VK_TAB || pMsg->wParam== VK_RETURN) {
			
				OnSetText();
				if(GetKeyState(VK_SHIFT)>>8 == -1) 
				{
					m_nCol--;
					if(m_nCol < 0)
					{
						m_nCol = m_nMaxCol;
						m_nRow--;
						if(m_nRow < 0)
						{
							m_nCol=-1;
							m_nRow=-1;
							HideAll();
							return CMFCListCtrl::PreTranslateMessage(pMsg);
						}
					}
				}
				else 
				{
					m_nCol++;
					if(m_nCol > m_nMaxCol )
					{
						m_nCol = 0;
						m_nRow++;
						if(m_nRow > m_nMaxRow-1)
						{
							m_nCol = m_nMaxCol;
							m_nRow = m_nMaxRow-1;
							HideAll();
							m_nCol=-1;
							m_nRow=-1;
							return CMFCListCtrl::PreTranslateMessage(pMsg);
						}
		
					}
				}
				if(GetItemCount())
					ShowControl();
				return 1;
			} else if( pMsg->wParam== VK_ESCAPE ) {

				HideAll();
				m_nCol = m_nMaxCol; 
				m_nRow = m_nMaxRow;
			}
		}
	} else if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN) {
		if(!m_pParent)
			m_pParent= GetParent();
		if(m_pParent) {
			int index=GetNextItem(-1, LVIS_SELECTED);
			if(index!=-1) {

				NMLISTVIEW nmhdr;
				nmhdr.hdr.code=NM_DBLCLK;
				nmhdr.hdr.hwndFrom= GetSafeHwnd();
				nmhdr.hdr.idFrom= GetDlgCtrlID();
				nmhdr.iItem= index;
				nmhdr.iSubItem=0;
				m_pParent->SendMessage(WM_NOTIFY,GetDlgCtrlID(), (LPARAM)&nmhdr);
			}
		}
	}
	return CMFCListCtrl::PreTranslateMessage(pMsg);
}

void CListCtrlXP::OnSetText(bool notify/*=true*/)
{
	FUNCTION_STACK_TRACE("FFFFE08C");
	if( !m_bEditable)
		return;
	CWnd *handle= GetControl(m_nCol);
	if( handle && handle->IsWindowVisible()) {

		CString szText;
		handle->GetWindowText(szText);
		if(handle->IsKindOf(RUNTIME_CLASS(CDateTimeCtrl))) {
			CMyTime tiempo;
			((CDateTimeCtrl *)handle)->GetTime(tiempo);
			szText= tiempo.GetFecha();
		} else if(handle->GetRuntimeClass()==RUNTIME_CLASS(CEditFloat)) {
			CString otro= ((CEditFloat *)handle)->Text;
			CString otro1= ((CEditFloat *)handle)->Number;
			CString temp= CEditFloat::GetNormal(szText);
			if(atof(temp))
				szText= CEditFloat::GetFormatNumber(temp);
			else szText="";
		}
		SetItemText(m_nRow,m_nCol,szText);
		if( notify && m_pParent && m_nRow!=-1 && m_nCol!=-1) {
			
			// reemplazar por notificacion
			Notify(LVN_ENDLABELEDIT, m_nRow, m_nCol);
			// obsoleta 
			m_pParent->SendMessage(WM_LISTANOTIFICACION, m_nRow, m_nCol);
			m_pParent->SendMessage(WM_LISTANOTIFICACIONEX, MAKELONG(m_nCol, m_nRow), GetDlgCtrlID());
		}
	}
}

void CListCtrlXP::SetCanClean(bool canclean) {

	m_bCanClean= canclean;
}

BOOL CListCtrlXP::EnsureColumnVisible(int nCol, bool bPartialOK)
{
	if (nCol < 0 || nCol >= GetHeaderCtrl().GetItemCount())
		return FALSE;

	CRect rcHeader;
	if (GetHeaderCtrl().GetItemRect(nCol, rcHeader)==FALSE)
		return FALSE;

	CRect rcClient;
	GetClientRect(&rcClient);

	int nOffset = GetScrollPos(SB_HORZ);

	if(bPartialOK)
	{
		if((rcHeader.left - nOffset < rcClient.right)
			&& (rcHeader.right - nOffset > 0) )
		{
			return TRUE;
		}
	}

	int nScrollX = 0;

	if((rcHeader.Width() > rcClient.Width()) || (rcHeader.left - nOffset < 0))
	{
		nScrollX = rcHeader.left - nOffset;
	}
	else if(rcHeader.right - nOffset > rcClient.right)
	{
		nScrollX = rcHeader.right - nOffset - rcClient.right;
	}

	if(nScrollX != 0)
	{
		CSize size(nScrollX, 0);
		if (Scroll(size)==FALSE)
			return FALSE;
	}

	return TRUE;

}

void CListCtrlXP::ShowControl() {

	FUNCTION_STACK_TRACE("FFFFE08D");
	CRect rect;
	CString temp;
	CWnd *handle=GetControl(m_nCol);
	if( !handle || !m_bEditable) {

		PostMessage( WM_KEYDOWN, VK_TAB);
		return;
	}
	HideAll();
	m_bScrolling= true;
	if( m_nRow<0 || m_nRow > m_nMaxRow-1) 
		m_nRow=0;
	
	ASSERT( handle);
	EnsureVisible(m_nRow,true);
	EnsureColumnVisible(m_nCol, false);
	GetSubItemRect(m_nRow,m_nCol,LVIR_BOUNDS,rect);

		
	GetSubItemRect(m_nRow,m_nCol,LVIR_LABEL,rect);
	temp= GetItemText( m_nRow, m_nCol);
	handle->SetWindowText(temp);
	handle->MoveWindow( rect.left, rect.top, rect.Width(), rect.Height());
	handle->ShowWindow(SW_NORMAL);

	if( handle->GetRuntimeClass()==RUNTIME_CLASS(CEditFloat)) {
		((CEditFloat *)handle)->SetFormatNumero(temp);
		((CEditFloat *)handle)->SetSel(0, ((CEditFloat *)handle)->GetWindowTextLength());
	} else if( handle->IsKindOf(RUNTIME_CLASS(CEdit))) {
		((CEdit *)handle)->SetSel(0, temp.GetLength());
	} else if(handle->IsKindOf(RUNTIME_CLASS(CComboBoxXP))) 
		((CComboBoxXP *)handle)->SetEditSel(0, temp.GetLength());
	else if(handle->IsKindOf(RUNTIME_CLASS(CDateTimeCtrl))) {
		COleDateTime tiempo;
		if(!tiempo.ParseDateTime(temp))
			tiempo.m_status= COleDateTime::error;
		((CDateTimeCtrl *)handle)->SetTime(tiempo);
	}
	handle->SetFocus();
	if( m_pParent) {

		// notifiacion
		Notify(LVN_BEGINLABELEDIT, m_nRow, m_nCol);
		// obsoleto
		m_pParent->SendMessage(WM_LISTANOTIFICACION, m_nRow, m_nCol);
		m_pParent->SendMessage(WM_LISTANOTIFICACIONEX, MAKELONG(m_nCol, m_nRow), GetDlgCtrlID());
	}
	m_bScrolling= false;
}

void CListCtrlXP::EditCampo( unsigned int col, unsigned int row) {

	FUNCTION_STACK_TRACE("FFFFE08F");
	if( !m_bEditable) 
		return;
	m_nRow= row;
	m_nCol= col;
	ShowControl();
}

void CListCtrlXP::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	FUNCTION_STACK_TRACE("FFFFE090");
	OnSetText();
	HideAll();
	CMFCListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CListCtrlXP::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	FUNCTION_STACK_TRACE("FFFFE081");
	OnSetText();
	HideAll();
	CMFCListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CListCtrlXP::OnLvnInsertitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	FUNCTION_STACK_TRACE("FFFFE083");
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	m_nMaxRow= GetItemCount();
	HideAll();
	if( m_nMaxRow==1) {
		
		ModifyStyle(0, LVS_OWNERDRAWFIXED);
		Invalidate();
	}
	*pResult = 0;
}

void CListCtrlXP::OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	FUNCTION_STACK_TRACE("FFFFE084");
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	m_nMaxRow= GetItemCount()-1;
	while( m_nRow> m_nMaxRow-1) {
		m_nRow--;
		m_nCol=-1;
	}
	if( !m_nMaxRow)
		Invalidate();
	HideAll();
	*pResult = 0;
}

void CListCtrlXP::ResetTipos() {

	m_nColumType.RemoveAll();
}

void CListCtrlXP::SetType(int columna, int tipo) {

	while(m_nColumType.GetCount()<=columna)
		m_nColumType.Add(1);
	m_nColumType.SetAt(columna, tipo+1);
}

int	CListCtrlXP::GetColumnType(int columna) {
    
	int tipo=1;
	if( columna<m_nColumType.GetCount())
		tipo= m_nColumType.GetAt(columna);
	return tipo-1;
}

//////////////////////////////////////////////////////////////////////
// GetColumnCount()
// Parameters:  None.
// Action:
// Returns:     The number of columns in the list control
//////////////////////////////////////////////////////////////////////
int CListCtrlXP::GetColumnCount() const
{
    return m_wndHeader.GetItemCount();
}

void CListCtrlXP::SetOrdenable( bool ordenable) {
	
	m_bOrdenable= ordenable;
}

bool CListCtrlXP::GetSelItem(int &x, int &y) {

	CPoint punto(x, y);
    int pos= HitTest( punto);
	if( pos!=-1) {

		CRect rect;
		x=-1;
		for( int i=0; i<GetColumnCount(); i++) {
			GetSubItemRect( pos, i, LVIR_LABEL, rect);
			if( rect.PtInRect( punto)) {
				x= i;
				break;
			}
		}
		y= pos;
		return true;
	}
	return false;
}

void CListCtrlXP::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	FUNCTION_STACK_TRACE("FFFFE088");
	CMFCListCtrl::OnLButtonDblClk(nFlags, point);

	if( !m_bEditable)
		return;
	int x= point.x, y= point.y;
	if( GetSelItem(x, y)) {
		m_nRow= y;
		m_nCol= x==-1 ? 0 : x;
		ShowControl();
	}
}

int	CListCtrlXP::AddColumn(CString texto, int tipo/*=0*/) {

	int numero= InsertColumn(m_wndHeader.GetItemCount(), texto, (tipo && tipo!=tipocol_textomultiline) ? LVCFMT_RIGHT : LVCFMT_LEFT, 100);
	SetType(numero, tipo);
	if(numero==0 && tipo && tipo!=tipocol_textomultiline) { // bug en la primera columna agregada

		LVCOLUMN col;
		col.mask=LVCF_FMT;
		GetColumn(0, &col);
		col.fmt|=LVCFMT_RIGHT;
		SetColumn(0, &col);
	}
	return numero;
}

int CListCtrlXP::GetSortedColumn() {

	return GetHeaderCtrl().GetColumnState(m_iSortedColumn)==0 ? -1 : m_iSortedColumn;
}

bool CListCtrlXP::GetSortedColumnOrder() {

	BOOL bAscending = true;
	int nColumnState = GetHeaderCtrl().GetColumnState(m_iSortedColumn);
	if (nColumnState != 0)
		bAscending = nColumnState <= 0;
	return bAscending;
}

void CListCtrlXP::SortList(const int nColumn, const bool bAscending) {

	NMLISTVIEW pNMLV;
	LRESULT resultado;
	
	GetHeaderCtrl().SetSortColumn(nColumn, !bAscending, false);
	pNMLV.iSubItem= nColumn;

	OnLvnColumnclick((NMHDR *)&pNMLV, &resultado);
}

void CListCtrlXP::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	FUNCTION_STACK_TRACE("FFFFE089");
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// aqui es donde toca llamar al padre
	if( !m_bOrdenable)
		return;
	CWaitCursor wait;

	m_iSortedColumn=pNMLV->iSubItem;
	CSortClass ordenar(this, m_iSortedColumn);
	BOOL bAscending = true;
	int nColumnState = GetHeaderCtrl().GetColumnState(m_iSortedColumn);
	if (nColumnState != 0)
		bAscending = nColumnState <= 0;
	GetHeaderCtrl().SetSortColumn(m_iSortedColumn, bAscending, false);
	ordenar.Sort(bAscending,GetColumnType(m_iSortedColumn)+1);

	int index= GetNextItem(-1, LVIS_SELECTED);
	if( index!=-1)
		EnsureVisible(index, true);

	// obsoleto no usar
	GetParent()->SendMessage(WM_LISTANOTIFICACION, -1, m_iSortedColumn);
	GetParent()->SendMessage(WM_LISTANOTIFICACIONEX, MAKELONG(m_iSortedColumn, -1), GetDlgCtrlID());

	*pResult = 0;
}

void CListCtrlXP::OnLvnDeleteallitems(NMHDR *pNMHDR, LRESULT *pResult)
{
	FUNCTION_STACK_TRACE("FFFFE08A");
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	m_nNoSort= -1;
	RemoveSortColumn(m_iSortedColumn);
	*pResult = 0;
}

void CListCtrlXP::OnPaint()
{
	Default();
    if (GetItemCount() <= 0)
    {
        COLORREF clrText = ::GetSysColor(COLOR_HIGHLIGHT);
        COLORREF clrTextBk = ::GetSysColor(COLOR_WINDOW);

        CDC* pDC = GetDC();
		if(!pDC || !pDC->m_hDC)
			return ;
        // Save dc state
        int nSavedDC = pDC->SaveDC();

        CRect rc;
        GetWindowRect(&rc);
        ScreenToClient(&rc);
		pDC->FillRect(rc, &CBrush(clrTextBk));

        CRect rcH;
		m_wndHeader.GetClientRect(rcH);
        rc.top += rcH.bottom+5;
			
        pDC->SetTextColor(clrText);
        pDC->SetBkColor(clrTextBk);
        pDC->SelectStockObject(ANSI_VAR_FONT);
		pDC->DrawText(m_szVacia, rc, DT_CENTER| DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP);
        pDC->RestoreDC(nSavedDC);
        ReleaseDC(pDC);
		m_wndHeader.Invalidate();
    } else if ((GetStyle() & LVS_TYPEMASK) == LVS_REPORT ) {
		CRect rcBounds;
		GetItemRect(0, rcBounds, LVIR_BOUNDS);

		CRect rcClient;
		GetClientRect(&rcClient);
		if(rcBounds.right < rcClient.right)
		{
			CPaintDC dc(this);

			if(!dc.m_hDC)
				return ;

			CRect rcClip;
			dc.GetClipBox(rcClip);

			rcClip.left = min(rcBounds.right-1, rcClip.left);
			rcClip.right = rcClient.right;

			InvalidateRect(rcClip, FALSE);
		}
	}
}

void CListCtrlXP::SetMessageLista(const CString &message) {

	m_szVacia= message;
	if(GetItemCount()==0)
		Invalidate();
}

LRESULT CListCtrlXP::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message==WM_COMMAND) {
		if(HIWORD(wParam)==CBN_KILLFOCUS || HIWORD(wParam)==EN_KILLFOCUS) {
			if( !m_bScrolling) {
				if( (::GetWindowLong((HWND)lParam,GWL_STYLE) &WS_VISIBLE)) {
					OnSetText();
				}
				HideAll();
			}
		}
	} else if(message==WM_NOTIFY) {
		switch(((LPNMHDR)lParam)->code) {
			case DTN_DROPDOWN:
				m_bDropDown=true;
				break;
			case NM_KILLFOCUS:
				if(m_bDropDown)
					break;
			case DTN_CLOSEUP:
				m_bDropDown=false;
				if( !m_bScrolling) {
					NMHDR *lpNmhdr= (NMHDR *)lParam;
					if((::GetWindowLong(lpNmhdr->hwndFrom,GWL_STYLE) &WS_VISIBLE)) 
						OnSetText();
					HideAll();
				}
				break;
		}
	}
	return CMFCListCtrl::WindowProc(message, wParam, lParam);
}

void CListCtrlXP::StoreState(CString nombre) {

	FUNCTION_STACK_TRACE("FFFFE08B");
	CWinApp* pApp = AfxGetApp();
	CString profile;
	profile.Format("%s-Lista%d", nombre, GetDlgCtrlID());
	CString ancho;
	for( int i=0; i<GetColumnCount(); i++) {
		ancho.Format("columw%d", i);
		pApp->WriteProfileInt(profile, ancho, GetColumnWidth(i));
	}
}

void CListCtrlXP::RestoreState(CString nombre) {

	FUNCTION_STACK_TRACE("FFFFE08C");
	CWinApp* pApp = AfxGetApp();
	CString profile;
	profile.Format("%s-Lista%d", nombre, GetDlgCtrlID());
	int valor;

	CString ancho;
	for( int i=0; i<GetColumnCount(); i++) {
		ancho.Format("columw%d", i);
		if( (valor= pApp->GetProfileInt(profile, ancho, -1))!=-1) 
			SetColumnWidth(i, valor);
	}
}

void CListCtrlXP::OnSetFocus(CWnd* pOldWnd)
{
	CMFCListCtrl::OnSetFocus(pOldWnd);
	if(GetParent()->GetNextDlgTabItem(this, true)==pOldWnd) {
		m_nRow = -1;
		m_nCol = -1;
	}
	if(pOldWnd!=NULL && pOldWnd->GetParent()==this)
		return;
	if((GetStyle() & LVS_TYPEMASK)==LVS_REPORT)
		RepaintSelectedItems();
}

void CListCtrlXP::AutoSizeColumns(int delta)
{
	CWaitCursor wait;
	for (int i=0; i < m_wndHeader.GetItemCount(); i++) 
		SetColumnWidth(i, delta==1 ? LVSCW_AUTOSIZE : LVSCW_AUTOSIZE_USEHEADER);
}

inline CString GetShortMenu1(UINT id)
{
	FUNCTION_STACK_TRACE("FFFFE043");
	CString str;
	str.LoadString(id);
	int nIndex = str.ReverseFind(_T('\n'));
	if(nIndex!=-1)
	{
		str=str.Mid(nIndex+1);
	}
	return str;
}

CMenu *CListCtrlXP::GetMenuContextual(CMenu *menu) {

	ASSERT(menu);
	if( GetSelectedCount()) {

		menu->AppendMenu(MF_STRING, ID_EDIT_COPY, GetSelectedCount()>1 ? "Copiar Filas" : "Copiar Fila");
		menu->AppendMenu(MF_STRING, ID_EDIT_UNDO, "Copiar Valor");
		menu->AppendMenu(MF_STRING, ID_EDIT_CUT, "Copiar Columna");
	}
	if( m_bCanClean)
		menu->AppendMenu(MF_STRING, ID_EDIT_CLEAR, GetShortMenu1(ID_EDIT_CLEAR));
	if( !(GetStyle()& LVS_SINGLESEL)) {
		if( GetSelectedCount()) 
			menu->AppendMenu(MF_SEPARATOR);
		menu->AppendMenu(MF_STRING, ID_EDIT_SELECT_ALL,GetShortMenu1(ID_EDIT_SELECT_ALL));
	}
	return menu;
}

CMenu *CListCtrlXP::GetMenuContextual() {

	if(m_hMenuContextual)
		delete m_hMenuContextual;

	m_hMenuContextual= new CMenu();
	m_hMenuContextual->CreatePopupMenu();

	return GetMenuContextual(m_hMenuContextual);
}

void CListCtrlXP::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	if( !m_bCanShowMenu)
		return;
	if(point.x==-1 && point.y==-1)
	{
		CRect rect;
		GetWindowRect(rect);
		m_pPoint= rect.TopLeft();
		point = m_pPoint+CPoint(10,10);
	}
	GetCursorPos(&m_pPoint);
	ScreenToClient(&m_pPoint);
	SetFocus();
	
	CMenu *menu= GetMenuContextual();
	menu->TrackPopupMenu( TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
}

void CListCtrlXP::OnSelAll() {
	for( int i=0; i<GetItemCount(); i++)
		SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
}

LRESULT CListCtrlXP::OnClear(WPARAM wParam, LPARAM lParam) {
	
	ASSERT(m_bCanClean);
	return m_bCanClean ? DeleteAllItems() : 0;
}

LRESULT CListCtrlXP::OnCopyValue(WPARAM wParam, LPARAM lParam) {

	int x= m_pPoint.x, y= m_pPoint.y;
	if( GetSelItem(x, y) && x!=-1) {
		CString copy;
		copy=GetItemText(y, x);
		return copy.IsEmpty() ? false : CopyToClipBoard(copy);
	}
	return 0;
}

LRESULT CListCtrlXP::OnCopyColumn(WPARAM wParam, LPARAM lParam) {

	int x= m_pPoint.x, y= m_pPoint.y;
	if( GetSelItem(x, y) && x!=-1) {
		CString copy;
		for( int i=0; i<GetItemCount(); i++) 
			copy+=GetItemText(i, x) + "\r\n";
		return copy.IsEmpty() ? false : CopyToClipBoard(copy);
	}
	return 0;
}

bool CListCtrlXP::CopyToClipBoard(CString &valor) {

	HGLOBAL hglbCopy;
	if (!OpenClipboard()) 
        return FALSE;
	EmptyClipboard(); 
	hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (valor.GetLength() + 1) * sizeof(TCHAR));
    if (hglbCopy == NULL) 
    {
        CloseClipboard(); 
        return FALSE; 
    }
	LPTSTR  lptstrCopy;
	lptstrCopy = (LPTSTR) GlobalLock(hglbCopy); 
	memcpy(lptstrCopy, valor, valor.GetLength() * sizeof(TCHAR));
	lptstrCopy[valor.GetLength()] = (TCHAR) 0;
    GlobalUnlock(hglbCopy);
 
    SetClipboardData(CF_TEXT, hglbCopy);
	CloseClipboard();
	return true;
}

LRESULT CListCtrlXP::OnCopy(WPARAM wParam, LPARAM lParam) {
	
	CWaitCursor wait;
	CString temp;
	int index=-1;
	while( (index=GetNextItem(index, LVIS_SELECTED))!=-1) {
		for( int i=0; i<GetColumnCount(); i++) 
			temp+= GetItemText(index, i) + "\t";
		temp+="\r\n";
	}
	if( temp.IsEmpty())
		return false;
	return CopyToClipBoard(temp);
}

bool CListCtrlXP::SetCanShowMenu(bool can) {

	bool old= m_bCanShowMenu;
	m_bCanShowMenu=can;
	return old;
}

void CListCtrlXP::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_pPoint= point;
	CMFCListCtrl::OnRButtonDown(nFlags, point);
}

int	CListCtrlXP::GetNoSort() {

	return m_nNoSort;
}

void CListCtrlXP::OnKillFocus(CWnd* pNewWnd)
{
	CMFCListCtrl::OnKillFocus(pNewWnd);
	if(pNewWnd != NULL && pNewWnd->GetParent() == this)
		return;

	// repaint items that should change appearance
	if((GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
		RepaintSelectedItems();
}

void CListCtrlXP::RepaintSelectedItems()
{
	CRect rcBounds, rcLabel;

	// Invalidate focused item so it can repaint 
	int nItem = GetNextItem(-1, LVNI_FOCUSED);

	if(nItem != -1)
	{
		GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
		GetItemRect(nItem, rcLabel, LVIR_LABEL);
		rcBounds.left = rcLabel.left;

		InvalidateRect(rcBounds, FALSE);
	}

	// Invalidate selected items depending on LVS_SHOWSELALWAYS
	if(!(GetStyle() & LVS_SHOWSELALWAYS))
	{
		for(nItem = GetNextItem(-1, LVNI_SELECTED);
			nItem != -1; nItem = GetNextItem(nItem, LVNI_SELECTED))
		{
			GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
			GetItemRect(nItem, rcLabel, LVIR_LABEL);
			rcBounds.left = rcLabel.left;

			InvalidateRect(rcBounds, FALSE);
		}
	}

	UpdateWindow();
}

void CListCtrlXP::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem);
	int nItem = lpDrawItemStruct->itemID;
	CImageList* pImageList;

	// Save dc state
	int nSavedDC = pDC->SaveDC();

	// Get item image and state info
	LV_ITEM lvi;
	lvi.mask = LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;
	GetItem(&lvi);

	// Should the item be highlighted
	BOOL bHighlight =((lvi.state & LVIS_DROPHILITED)
				|| ( (lvi.state & LVIS_SELECTED)
					&& ((GetFocus() == this)
						|| (GetStyle() & LVS_SHOWSELALWAYS)
						)
					)
				);


	// Get rectangles for drawing
	CRect rcBounds, rcLabel, rcIcon;
	GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
	GetItemRect(nItem, rcLabel, LVIR_LABEL);
	GetItemRect(nItem, rcIcon, LVIR_ICON);
	CRect rcCol( rcBounds ); 


	CString sLabel = GetItemText( nItem, 0 );

	// Labels are offset by a certain amount  
	// This offset is related to the width of a space character
	int offset = pDC->GetTextExtent(_T(" "), 1 ).cx*2;

	CRect rcHighlight=rcItem;
	CRect rcWnd;

	// Draw the background color
	if( bHighlight )
	{
		pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->FillSolidRect(rcHighlight, HLS_TRANSFORM( ::GetSysColor(COLOR_HIGHLIGHT),  +80, -66));
	}
	else
		pDC->FillSolidRect(rcHighlight, ::GetSysColor(COLOR_WINDOW));

	// Set clip region
	rcCol.right = rcCol.left + GetColumnWidth(0);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rcCol);
	pDC->SelectClipRgn(&rgn);
	rgn.DeleteObject();

	// Draw state icon
	if (lvi.state & LVIS_STATEIMAGEMASK)
	{
		int nImage = ((lvi.state & LVIS_STATEIMAGEMASK)>>12) - 1 ;
		pImageList = GetImageList(LVSIL_STATE);
		if (pImageList)
		{
			pImageList->Draw(pDC, nImage,
				CPoint(rcCol.left, rcCol.top), ILD_TRANSPARENT);
		}
	}
	
	// Draw normal and overlay icon
	pImageList = GetImageList(LVSIL_SMALL);
	if (pImageList)
	{
		UINT nOvlImageMask=lvi.state & LVIS_OVERLAYMASK;
		pImageList->Draw(pDC, lvi.iImage, 
			CPoint(rcIcon.left, rcIcon.top),
			(bHighlight?ILD_BLEND50:0) | ILD_TRANSPARENT | nOvlImageMask );
	}

	
	
	// Draw item label - Column 0
	rcLabel.left += offset/2;
	rcLabel.right -= offset;

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;
	GetColumn(0, &lvc);

	UINT nJustify = DT_LEFT;
	switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
	{
		case LVCFMT_RIGHT:
			nJustify = DT_RIGHT;
			break;
		case LVCFMT_CENTER:
			nJustify = DT_CENTER;
			break;
		default:
			break;
	}
	if(rcLabel.Height()>25) // es una linea multiple
		nJustify|=DT_WORDBREAK | DT_END_ELLIPSIS;
	else nJustify|=DT_SINGLELINE;

	pDC->DrawText(sLabel,-1,rcLabel,nJustify | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);


	// Draw labels for remaining columns
	

	rcBounds.right = rcHighlight.right > rcBounds.right ? rcHighlight.right :
							rcBounds.right;
	rgn.CreateRectRgnIndirect(&rcBounds);
	pDC->SelectClipRgn(&rgn);
	
	for(int nColumn = 1; GetColumn(nColumn, &lvc); nColumn++)
	{
		rcCol.left = rcCol.right;
		rcCol.right += lvc.cx;

		sLabel = GetItemText(nItem, nColumn);
		if (sLabel.GetLength() == 0)
			continue;

		// Get the text justification
		UINT nJustify = DT_LEFT;
		switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
		{
			case LVCFMT_RIGHT:
				nJustify = DT_RIGHT;
				break;
			case LVCFMT_CENTER:
				nJustify = DT_CENTER;
				break;
			default:
				break;
		}
		if(rcLabel.Height()>25) // es una linea multiple
			nJustify|=DT_WORDBREAK;
		else nJustify|=DT_SINGLELINE;

		rcLabel = rcCol;
		rcLabel.left += offset;
		rcLabel.right -= offset;

		pDC->DrawText(sLabel, -1, rcLabel, nJustify | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);
	}

	if (lvi.state & LVIS_FOCUSED && (GetFocus() == this)) {

		CPen pen, *oldpen;
		pen.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_HIGHLIGHT));
		oldpen= pDC->SelectObject(&pen);
		pDC->SelectObject(::GetStockObject (NULL_BRUSH));
		rcHighlight.DeflateRect(0,0,0,1);
		pDC->Rectangle(rcHighlight);
		pDC->SelectObject(oldpen);
	}

	pDC->RestoreDC( nSavedDC );
}

void CListCtrlXP::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CMFCListCtrl::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

int CListCtrlXP::HitTest(_In_ CPoint pt, _In_opt_ UINT* pFlags /*= NULL*/) const {

	return CMFCListCtrl::HitTest(pt, pFlags);
}

void CListCtrlXP::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( GetExtendedStyle()&LVS_EX_CHECKBOXES) {
		UINT flags;
		int index= HitTest(point, &flags);
		if( index!=-1 && flags&LVHT_ONITEMSTATEICON) {
			CRect rect;
			if(GetItemRect(index, rect, LVIR_ICON)) {
				rect.left= 0;
				rect.right=GetSystemMetrics(SM_CXMENUCHECK)+1;
				if( rect.PtInRect(point))
					SetCheck(index, !GetCheck(index));
			}
		}
	}
	CMFCListCtrl::OnLButtonDown(nFlags, point);
}

void CListCtrlXP::SetText(int row, int col,const CString &mensaje) {

	if(GetItemCount()<=row) {
		InsertItem(row, "");
		ASSERT(GetItemCount()-1==row);
	}
	SetItemText(row, col, mensaje);
}

CString  CListCtrlXP::GetText(int row, int col) {

	return GetItemText(row, col);
}

CString CListCtrlXP::GetMensaje() {

	return m_szVacia;
}

void CListCtrlXP::SetSelectedIndex(int index) {

	SetItemState(index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED |LVIS_FOCUSED );
}

int CListCtrlXP::GetSelectedIndex() {

	return GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
}

BOOL CListCtrlXP::SetItemText(int nItem,int nSubItem,LPCTSTR lpszText) {

	return CMFCListCtrl::SetItemText(nItem, nSubItem, lpszText);
}

LRESULT	CListCtrlXP::Notify( DWORD dwMessage, int nItem, int nCol ) {

	NMITEMACTIVATE nm;

	nm.hdr.hwndFrom = GetSafeHwnd();
	nm.hdr.idFrom	= GetDlgCtrlID();
	nm.hdr.code		= dwMessage;

	nm.iItem = nItem;
	nm.iSubItem	= nCol;

	CWnd* pWnd		= GetParent();
	LRESULT lResult	= 0;

	if( pWnd == NULL ) {
		lResult = 0;
	} else {
		lResult = pWnd->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nm );
	}

	return lResult;	
}
