#include "StdAfx.h"
#include "listctrlsize.h"
# include "Componentes.h"

# define TIMER_UPDATE	15

CListCtrlSize::CListCtrlSize(void)
{
	m_sizeMax= CSize(0,0);
	m_sizeMin= CSize(0,0);
	m_sizeDelta= CSize(0,0);
	m_pFin= m_pLabel= NULL;
	m_nHeigth=0;
}

CListCtrlSize::~CListCtrlSize(void)
{
}

BEGIN_MESSAGE_MAP(CListCtrlSize, CListCtrlXP)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


void CListCtrlSize::SetMinSize(CSize size) {
    
	m_sizeMin= size;
}

void CListCtrlSize::SetMaxSize(CSize size) {

	m_sizeMax= size;
}

void CListCtrlSize::SetLabel(CWnd *label) {

	m_pLabel= label;
}

void CListCtrlSize::SetFin(CWnd *fin) {

	m_pFin= fin;
}

void CListCtrlSize::Init() {

	CRect rect;
	GetWindowRect(rect);
	SetMinSize(CSize (rect.Width(), rect.Height()));
	SetMaxSize(CSize(-1,-1));
}

void CListCtrlSize::AutoSizeColumns(int delta)
{
	CWaitCursor wait;
	for (int i=0; i < m_wndHeader.GetItemCount(); i++) {
		SetColumnWidth(i, delta==1 ? LVSCW_AUTOSIZE : LVSCW_AUTOSIZE_USEHEADER);
		int maximo=m_pTamanos[(void *)i];
		if(maximo && GetColumnWidth(i)>maximo) {
			SetColumnWidth(i, maximo); // corregimos el ancho, queda faltando que corrija el alto
		}
	}
}

void CListCtrlSize::SetMaxColumnWidth(int col, int maximo/*=120*/) {

	m_pTamanos.SetAt((void *)col, maximo);
}

void CListCtrlSize::RealUpdateSize() {
	
	CRect rect,rectthis;
	GetWindowRect(&rectthis);
	GetParent()->ScreenToClient(&rectthis);
	m_sizeDelta.cy= rectthis.bottom;
	m_sizeDelta.cx= rectthis.right;

	SetRedraw(false);
	GetParent()->SetRedraw(false);
	MoveWindow(rectthis.left, rectthis.top,m_sizeMin.cx, m_sizeMin.cy);
	AutoSizeColumns();

	if( GetItemCount()) {
		GetItemRect(0, &rect, LVIR_BOUNDS);
		int count= GetItemCount()+1;

		count*= rect.Height()+1;
		count+=10;
		if(m_sizeMax.cy!=-1 && count>m_sizeMax.cy) 
			count= m_sizeMax.cy;
		if( count< m_sizeMin.cy)
			count= m_sizeMin.cy;
		int ancho=rect.Width()+10;

		if( m_sizeMax.cx!=-1 && ancho>m_sizeMax.cx) 
			ancho= m_sizeMax.cx;
		if( ancho< m_sizeMin.cx)
			ancho= m_sizeMin.cx;
		rectthis.bottom= rectthis.top+count;
		rectthis.right= rectthis.left+ancho;
	} else {
		rectthis.bottom= rectthis.top + m_sizeMin.cy;
		rectthis.right= rectthis.left+ m_sizeMin.cx;
	}
	m_sizeDelta.cy-= rectthis.bottom;
	m_sizeDelta.cx-= rectthis.right;

	MoveWindow(rectthis);
	SetRedraw(true);
	GetParent()->SetRedraw(true);
	UpdateCtrlItems();
	GetParent()->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE  | RDW_ALLCHILDREN);

	for(int i=0; i<m_pArrayControles.GetCount(); i++) {
		CWnd *control= (CWnd *) m_pArrayControles[i];
		if( control && ::IsWindow(*control) && control->IsWindowVisible()) {
			int row, col;
			GetCurrentPos(col, row);
			CRect rect;
			EnsureVisible(row,true);
			GetSubItemRect(row,col,LVIR_LABEL,rect);
			control->MoveWindow( rect.left, rect.top, rect.Width(), rect.Height());
		}
	}
	GetParent()->RedrawWindow();
}

void CListCtrlSize::UpdateSize() {

	KillTimer(TIMER_UPDATE);
	SetTimer(TIMER_UPDATE, AfxGetApp()->GetProfileInt("listctrl", "update", 50), NULL);
}

void CListCtrlSize::UpdateCtrlItems() {

	if( m_sizeDelta.cx==0 && m_sizeDelta.cy==0)
		return;

	CRect rectthis, rect;
	CWnd *parent= GetParent();
	CWnd *item= parent->GetWindow(GW_CHILD);
	GetWindowRect(&rectthis);
	parent->ScreenToClient(&rectthis);

	// establecemos las coordenadas en las antiguas
	rectthis.bottom+=m_sizeDelta.cy;
	rectthis.right+= m_sizeDelta.cx;

	while(item) {

		if( item==this) {
			item= item->GetWindow(GW_HWNDNEXT);
			continue;
		}
		
		item->GetWindowRect(rect);
		parent->ScreenToClient(rect);

		// verificamos si el elemento esta contenido en algo.
		if(rect.PtInRect(rectthis.BottomRight()) && rect.PtInRect(rectthis.TopLeft())) { // el componente esta dentro de este

			rect.bottom-= m_sizeDelta.cy;
			rect.right-=m_sizeDelta.cx;
			item->MoveWindow(rect);
			item= item->GetWindow(GW_HWNDNEXT);
			continue;
		}
		if( rect.top < rectthis.top) {
			item= item->GetWindow(GW_HWNDNEXT);
			continue;
		}
		if( m_pFin==item) 
			rect.MoveToXY(rect.left- m_sizeDelta.cx, rect.top-m_sizeDelta.cy);
		else rect.MoveToY(rect.top-m_sizeDelta.cy);
		item->MoveWindow(rect);
		item= item->GetWindow(GW_HWNDNEXT);
	}

	parent->RedrawWindow();
	if(parent->IsKindOf(RUNTIME_CLASS(CFormView))) {
		RecalculateScroll((CFormView *)parent);
	} else if(parent->IsKindOf(RUNTIME_CLASS(CDialog))) {
		parent->GetWindowRect(rect);
		rect.bottom-=m_sizeDelta.cy;
		rect.right-=m_sizeDelta.cx;
		parent->MoveWindow(rect);
	}
}


void CListCtrlSize::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {

	CListCtrlXP::DrawItem(lpDrawItemStruct);
}

BOOL CListCtrlSize::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message==WM_MOUSEWHEEL) {
		
		/*SCROLLINFO si;
		ZeroMemory(&si,sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		GetScrollInfo(SB_HORZ, &si);*/
		// if(si.nMax>0) {
		GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
		// return TRUE;
		// }
	}
	return CListCtrlXP::PreTranslateMessage(pMsg);
}


BOOL CListCtrlSize::DeleteItem(int nItem) {
	
	CWaitCursor wait;
	BOOL retorno= CListCtrlXP::DeleteItem(nItem);
	if(!m_pTamanos.IsEmpty()) { // calculamos el tamaño optimo de la lista
		
		if(GetItemCount()) {
			m_nHeigth=0;
			for(int i=0; i<GetItemCount(); i++) {
				for(int j=0; j<GetColumnCount(); j++) {
					UpdateHeight(GetItemText(i, j), j);
				}
			}
		} else {
			UpdateHeight();
		}
	}
	UpdateSize();
	return retorno;
}

BOOL CListCtrlSize::DeleteAllItems() {

	BOOL retorno= CListCtrlXP::DeleteAllItems();
	UpdateHeight();
	UpdateSize();
	return retorno;
}

BOOL CListCtrlSize::SetItemText(int nItem,int nSubItem,LPCTSTR lpszText) {

	BOOL retorno= CListCtrlXP::SetItemText(nItem,nSubItem,lpszText);
	UpdateHeight(lpszText, nSubItem);
	UpdateSize();
	return retorno;
}

int CListCtrlSize::InsertItem(int nItem,LPCTSTR lpszItem) {

	int retorno= CListCtrlXP::InsertItem(nItem,lpszItem);
	UpdateHeight(lpszItem, 0);
	UpdateSize();
	return retorno;
}

int CListCtrlSize::InsertItem(int nItem,LPCTSTR lpszItem,int nImage) {

	int retorno= CListCtrlXP::InsertItem(nItem,lpszItem,nImage);
	UpdateHeight(lpszItem, 0);
	UpdateSize();
	return retorno;
}

void CListCtrlSize::UpdateHeight(LPCTSTR lpszText, int col) {

	if(col==-1) { // reseteamos el valor inicial
		m_nHeigth=20;
		CImageList imageList;
		imageList.Create(1,m_nHeigth,ILC_COLOR4,1,1);
		SetImageList(&imageList, LVSIL_SMALL);
		return; 
	}
	int tamano=m_pTamanos[(void *)col];
	if(tamano) {
		CDC* pDC = GetDC();

		CRect rect(0,0,0,0);
		rect.left=0;
		rect.right=tamano;

		CFont *fuente= pDC->SelectObject(GetFont());
		pDC->DrawText(lpszText, -1, rect, DT_WORDBREAK | DT_END_ELLIPSIS | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_CALCRECT);
		pDC->SelectObject(fuente);

		if(m_nHeigth<rect.Height()) {

			m_nHeigth=rect.Height();
			CImageList imageList;
			imageList.Create(1,rect.Height()>20 ? rect.Height() : 20,ILC_COLOR4,1,1);
			SetImageList(&imageList, LVSIL_SMALL);
		}
	}
}

int	CListCtrlSize::AddItem(LPCTSTR lpszItem) {

	int retorno= CListCtrlXP::AddItem(lpszItem);
	UpdateHeight(lpszItem, 0);
	UpdateSize();
	return retorno;
}

void CListCtrlSize::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent==TIMER_UPDATE) {
		KillTimer(TIMER_UPDATE);
		RealUpdateSize();
	} 
	CListCtrlXP::OnTimer(nIDEvent);
}

void CListCtrlSize::OnDestroy()
{
	KillTimer(TIMER_UPDATE);
	CListCtrlXP::OnDestroy();
}

int	CListCtrlSize::AddColumn(CString texto, int tipo/*=CListCtrlXP::tipocol_texto*/, int maximocolumna/*=0*/) {

	int pos= CListCtrlXP::AddColumn(texto, tipo);
	if(maximocolumna)
		SetMaxColumnWidth(pos, maximocolumna);
	return pos;
}
