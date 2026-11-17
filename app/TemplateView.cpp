// TemplateView.cpp: implementación de la clase CTemplateView
//

#include "stdafx.h"
#include "Template.h"

#include "TemplateView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTemplateView

IMPLEMENT_DYNCREATE(CTemplateView, CFormView)

BEGIN_MESSAGE_MAP(CTemplateView, CFormView)
END_MESSAGE_MAP()

// Construcción o destrucción de CTemplateView

CTemplateView::CTemplateView()
	: CFormView(CTemplateView::IDD)
{
	// TODO: agregar aquí el código de construcción

}

CTemplateView::~CTemplateView()
{
}

void CTemplateView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CTemplateView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: modificar aquí la clase Window o los estilos cambiando
	//  CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CTemplateView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	ResizeParentToFit();

}


// Diagnósticos de CTemplateView

#ifdef _DEBUG
void CTemplateView::AssertValid() const
{
	CFormView::AssertValid();
}

void CTemplateView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// Controladores de mensaje de CTemplateView
