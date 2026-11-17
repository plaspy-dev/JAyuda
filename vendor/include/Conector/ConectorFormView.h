#pragma once
#include "afxext.h"
# include "realconector.h"
# include "usuario.h"
# include "usuario2.h"
# include "usuario3.h"

# define WM_UPDATEMODIFICATION	WM_USER+1457

class __CONECTOR_EXPORT___ CConectorFormView : public CResizableFormView
{
protected:
	CString		m_szModificacionesID;
	CConector	*m_pConector;
	CUsuario	*m_pUsuario;
	CUsuario2	*m_pUsuario2;
	CUsuario3	*m_pUsuario3;

	DECLARE_DYNAMIC(CConectorFormView)
public:
	CConectorFormView(LPCTSTR lpszTemplateName);
	CConectorFormView(UINT nIDTemplate);
	~CConectorFormView(void);
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual CString InsertModification(const CString &id);
	void SetModificacion(const CString &id, bool force=true);
	void SetError(const CString &error, CWnd *elemento=NULL,int tipo=0);
protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
};
