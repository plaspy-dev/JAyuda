#pragma once

class   __COMPONENTES_EXPORT__ CMultiDocTemplateXP: public CMultiDocTemplate
{
	DECLARE_DYNAMIC(CMultiDocTemplateXP)
// Constructors
public:
  CMultiDocTemplateXP(UINT nIDResource, CRuntimeClass* pDocClass,
                       CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
  ~CMultiDocTemplateXP();
  CString	GetViewClassName();
};