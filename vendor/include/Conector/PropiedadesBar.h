#pragma once


class __CONECTOR_EXPORT___ CPropiedadesModificacion {

protected:
	CString m_szUsuario;
	CMyTime m_dtTiempo;
public:
	CPropiedadesModificacion(const CPropiedadesModificacion &nuevo);
	CPropiedadesModificacion(const CString &usuario="", const CString &tiempo="");
	~CPropiedadesModificacion();
	CString GetUsuario() const;
	CMyTime GetFecha() const;
	bool IsEmpty() const;
	CPropiedadesModificacion &operator = (const CPropiedadesModificacion &nuevo);
};

// CPropiedadesBar
class __CONECTOR_EXPORT___ CPropiedades {

	CString m_szClase;
	CString	m_szTitulo;
	CString	m_szID;
	CPropiedadesModificacion	m_pCreacion;
	CPropiedadesModificacion	m_pUltimo;
	CPtrArray		m_pModificaciones;
public:
	CPropiedades(const CString &clase, const CString &titulo, const CString &id);
	~CPropiedades();
	void SetID(const CString &id);
	CString GetTitulo() const;
	CString GetClase() const;
	CString GetID() const;
	void Add(const CString &usuario, const CString &fecha);
	CPtrArray *GetModificaciones();
	CPropiedadesModificacion *GetCreacion();
	CPropiedadesModificacion *GetUltimo();
	void Clean();
};

class __CONECTOR_EXPORT___ CPropiedadesBar : public CDockablePane
{
	DECLARE_DYNAMIC(CPropiedadesBar)
	CConector		*m_pConector;
	bool			m_bUpdate;
	CMFCPropertyGridProperty* m_pModificaciones;
	CMFCPropertyGridCtrl	m_wndPropList;
	CPropiedades		*m_pActual;
	CPtrArray			m_pPropiedades;
public:
	CPropiedadesBar();
	virtual ~CPropiedadesBar();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void Update(bool Force=false);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	LRESULT OnMySQLMessage(WPARAM wParam, LPARAM lParam);
	void SetConector( CConector *conector);
	void SetPropiedades(const CPropiedades &propiedades, bool force);
};


