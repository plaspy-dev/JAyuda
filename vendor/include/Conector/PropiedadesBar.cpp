// PropiedadesBar.cpp : implementation file
//

#include "stdafx.h"
#include "Conector.h"
#include "PropiedadesBar.h"

CPropiedadesModificacion::CPropiedadesModificacion(const CPropiedadesModificacion &nuevo) {

	*this= nuevo;
}

CPropiedadesModificacion::CPropiedadesModificacion(const CString &usuario, const CString &tiempo) {

	m_szUsuario= usuario;
	m_dtTiempo.SetStringDateTime(tiempo);
}

CPropiedadesModificacion::~CPropiedadesModificacion() {
}

CString CPropiedadesModificacion::GetUsuario() const {

	return m_szUsuario;
}

CMyTime CPropiedadesModificacion::GetFecha() const {

	return m_dtTiempo;
}

bool CPropiedadesModificacion::IsEmpty() const {
	
	return m_szUsuario.IsEmpty();
}

CPropiedadesModificacion &CPropiedadesModificacion::operator = (const CPropiedadesModificacion &nuevo) {
	
	m_szUsuario= nuevo.m_szUsuario;
	m_dtTiempo= nuevo.m_dtTiempo;
	return *this;
}

CPropiedades::CPropiedades(const CString &clase, const CString &titulo, const CString &id) {

	m_szClase= clase;
	m_szTitulo= titulo;
	m_szID= id;
}

CPropiedades::~CPropiedades() {

	Clean();
}

void CPropiedades::SetID(const CString &id) {
	m_szID= id;
}

CString CPropiedades::GetTitulo() const {

	return m_szTitulo;
}

CString CPropiedades::GetClase() const {

	return m_szClase;
}

CString CPropiedades::GetID() const {

	return m_szID;
}

void CPropiedades::Add(const CString &usuario, const CString &fecha) {

	if(m_pCreacion.IsEmpty()) {
		m_pCreacion = CPropiedadesModificacion(usuario, fecha);
		return;
	}
	if(!m_pUltimo.IsEmpty()) 
		m_pModificaciones.Add(new CPropiedadesModificacion(m_pUltimo));
	m_pUltimo= CPropiedadesModificacion(usuario, fecha);
}

CPtrArray *CPropiedades::GetModificaciones() {

	return &m_pModificaciones;
}

CPropiedadesModificacion *CPropiedades::GetCreacion() {
	
	return &m_pCreacion;
}

CPropiedadesModificacion *CPropiedades::GetUltimo() {

	return &m_pUltimo;
}

void CPropiedades::Clean() {

	m_pCreacion= CPropiedadesModificacion();
	m_pUltimo= CPropiedadesModificacion();
	for(int i=0; i<m_pModificaciones.GetCount(); i++) {
		CPropiedadesModificacion *info= (CPropiedadesModificacion *)m_pModificaciones[i];
		delete info;
	}
	m_pModificaciones.RemoveAll();
}

// CPropiedadesBar

IMPLEMENT_DYNAMIC(CPropiedadesBar, CDockablePane)

CPropiedadesBar::CPropiedadesBar()
{
	m_pConector= NULL;
	m_bUpdate= true;
	m_pModificaciones=NULL;
	m_pActual=NULL;
}

CPropiedadesBar::~CPropiedadesBar()
{
	for(int i=0; i<m_pPropiedades.GetCount(); i++) {
		CPropiedades *info=(CPropiedades *)m_pPropiedades[i];
		delete info;
	}
}


BEGIN_MESSAGE_MAP(CPropiedadesBar, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MYSQL, OnMySQLMessage)
END_MESSAGE_MAP()


void CPropiedadesBar::SetPropiedades(const CPropiedades &propiedades, bool force) {

	m_pActual=NULL;
	for(int i=0; i<m_pPropiedades.GetCount(); i++) {
		CPropiedades *info=(CPropiedades *)m_pPropiedades[i];
		if(info->GetClase()==propiedades.GetClase()) { // se encontro la misma clase
			m_pActual= info;
			if(m_pActual->GetID()==propiedades.GetID()) { // es la misma
				if(!force) {

					m_wndPropList.ResetOriginalValues();
					if(m_pModificaciones) {
						m_wndPropList.DeleteProperty(m_pModificaciones);
						m_pModificaciones=NULL;
					}

					CPropiedadesModificacion *modificacion=m_pActual->GetCreacion();
					CMFCPropertyGridProperty* info=m_wndPropList.GetProperty(0);
					info->SetValue(m_pActual->GetTitulo());

					if(!modificacion->IsEmpty()) {
						info= m_wndPropList.GetProperty(1);
						info->SetValue(modificacion->GetUsuario());
						info= m_wndPropList.GetProperty(2);
						info->SetValue(modificacion->GetFecha().GetFechaHora());
					}
					modificacion= m_pActual->GetUltimo();
					if(!modificacion->IsEmpty()) {
						info= m_wndPropList.GetProperty(3);
						info->SetValue(modificacion->GetUsuario());
						info= m_wndPropList.GetProperty(4);
						info->SetValue(modificacion->GetFecha().GetFechaHora());
					}

					CPtrArray *arreglo= m_pActual->GetModificaciones();
					if(!arreglo->IsEmpty()) {
						m_pModificaciones= new CMFCPropertyGridProperty ("Modificaciones");
						m_wndPropList.AddProperty(m_pModificaciones);
					}
					for(int i=0; i<arreglo->GetCount(); i++) {
						modificacion=(CPropiedadesModificacion *) arreglo->GetAt(i);
						CMFCPropertyGridProperty* pBorderProp = new CMFCPropertyGridProperty (modificacion->GetUsuario(),modificacion->GetFecha().GetFechaHora());
						pBorderProp->AllowEdit(false);
						m_pModificaciones->AddSubItem(pBorderProp);
					}
					if(m_pModificaciones) {
						m_pModificaciones->Expand(false);
						m_pModificaciones->Expand();
					}
					return; 
				}
			}
			m_pActual->SetID(propiedades.GetID());
		}
	}
	if(!m_pActual) {
		m_pActual= new CPropiedades(propiedades.GetClase(), propiedades.GetTitulo(), propiedades.GetID());
		m_pPropiedades.Add(m_pActual);
	}
	m_bUpdate=false;
	Update();
}

// CPropiedadesBar message handlers
LRESULT CPropiedadesBar::OnMySQLMessage(WPARAM wParam, LPARAM lParam) {

	MySQLMessage mess(lParam);
	if(wParam==QUERY_COUNT2) {
		if(mess.GetID()==1 && mess.GetCount()>2) {
			m_pModificaciones= new CMFCPropertyGridProperty ("Modificaciones");
			m_wndPropList.AddProperty(m_pModificaciones);
		} else {
			m_pModificaciones=NULL;
		}
	} else if( wParam==QUERY_RESULT) {

		if( mess.GetID()==1) {

			CPropiedadesModificacion *creacion=m_pActual->GetCreacion();
			CPropiedadesModificacion *final= m_pActual->GetUltimo();
			CMFCPropertyGridProperty* info;

			if(creacion->IsEmpty()) {
				info= m_wndPropList.GetProperty(1);
				info->SetValue(mess["usuario"]);
				info= m_wndPropList.GetProperty(2);
				info->SetValue(CMyTime(mess["fecha"]).GetFechaHora());
			} else {				
				info= m_wndPropList.GetProperty(3);
				info->SetValue(mess["usuario"]);
				info= m_wndPropList.GetProperty(4);
				info->SetValue(CMyTime(mess["fecha"]).GetFechaHora());
				if(!final->IsEmpty() && m_pModificaciones) {
					CMFCPropertyGridProperty* pBorderProp = new CMFCPropertyGridProperty (final->GetUsuario(),final->GetFecha().GetFechaHora());
					pBorderProp->AllowEdit(false);
					m_pModificaciones->AddSubItem(pBorderProp);
				}
			}
			m_pActual->Add(mess["usuario"], mess["fecha"]);
		}
	} else if(wParam==QUERY_END) {
		if(mess.GetID()==1) { // actualizamos todo
			if(m_pModificaciones) {

				m_pModificaciones->Expand(false);
				m_pModificaciones->Expand();
			}
		}
	}
	return 0;
}

void CPropiedadesBar::Update(bool Force) {

	if( !Force)
		if(!IsWindowVisible())
			return;
	if(m_bUpdate) // ya esta actualizado
		return;
	m_bUpdate= true;
	m_pConector->KillQuery(1, this);

	m_wndPropList.ResetOriginalValues();
	if(m_pModificaciones)
		m_wndPropList.DeleteProperty(m_pModificaciones);
	m_pModificaciones=NULL;
	m_pActual->Clean();
	CMFCPropertyGridProperty* info=m_wndPropList.GetProperty(0);
	info->SetValue(m_pActual->GetTitulo());

	CQuery query("modificaciones");
	query["fecha"]="modificaciones.fecha";
	query["usuario"]="usuario.nombre";

	m_pConector->Query(query.GetSelect("modificaciones.id='" + m_pActual->GetID() + "' AND clase='" + m_pActual->GetClase() + "' ORDER BY fecha", "LEFT JOIN usuario ON (usuario.id=modificaciones.usuario)"),1, this);
}

void CPropiedadesBar::SetConector( CConector *conector) {

	m_pConector= conector;
}

int CPropiedadesBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndPropList.Create (WS_VISIBLE | WS_CHILD, CRect(0,0,0,0), this, 1))
	{
		TRACE0("Failed to create Properies Grid \n");
		return -1;      // fail to create
	}

	m_wndPropList.EnableHeaderCtrl(TRUE, "Propiedad","Valor");
	m_wndPropList.SetVSDotNetLook();

	CMFCPropertyGridProperty* pBorderProp = new CMFCPropertyGridProperty ("Documento", "");
	pBorderProp->AllowEdit(false);
	m_wndPropList.AddProperty(pBorderProp);

	pBorderProp = new CMFCPropertyGridProperty ("Creado por", "");
	pBorderProp->AllowEdit(false);
	m_wndPropList.AddProperty(pBorderProp);

	pBorderProp = new CMFCPropertyGridProperty ("Fecha", "");
	pBorderProp->AllowEdit(false);
	m_wndPropList.AddProperty(pBorderProp);

	pBorderProp = new CMFCPropertyGridProperty ("Último modificador", "");
	pBorderProp->AllowEdit(false);
	m_wndPropList.AddProperty(pBorderProp);

	pBorderProp = new CMFCPropertyGridProperty ("Última modificación", "");
	pBorderProp->AllowEdit(false);
	m_wndPropList.AddProperty(pBorderProp);

	return 0;
}

void CPropiedadesBar::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	if(::IsWindow(m_wndPropList))
		m_wndPropList.MoveWindow(0,0,cx,cy);
}

void CPropiedadesBar::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDockablePane::OnShowWindow(bShow, nStatus);
	if(bShow)
		Update(true);
}
