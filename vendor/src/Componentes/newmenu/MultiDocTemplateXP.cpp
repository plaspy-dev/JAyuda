#include "StdAfx.h"
#include "newmenu/multidoctemplatexp.h"

IMPLEMENT_DYNAMIC(CMultiDocTemplateXP,CMultiDocTemplate)

CMultiDocTemplateXP::CMultiDocTemplateXP(UINT nIDResource, 
                                           CRuntimeClass* pDocClass,
                                           CRuntimeClass* pFrameClass,
                                           CRuntimeClass* pViewClass)
: CMultiDocTemplate(nIDResource,pDocClass,pFrameClass,pViewClass)
{
  
}

CString CMultiDocTemplateXP::GetViewClassName() {
    
	ASSERT(m_pViewClass);
	return m_pViewClass->m_lpszClassName;
}

CMultiDocTemplateXP::~CMultiDocTemplateXP()
{
}