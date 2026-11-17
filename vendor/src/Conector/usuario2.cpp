# include "stdafx.h"
#include "Usuario2.h"

CUsuario2::CUsuario2(void)
{
	m_nExpira=0;
	m_nAutonumerico=0;
	m_bArchivo=false;
	m_bNuevoEquipo=false;
}

CUsuario2::CUsuario2(CString Nombre, DWORD id, DWORD grupo) {

	m_szNombre= Nombre;
	m_dwID= id;
	m_dwGrupo=grupo;
	m_nExpira=0;
	m_bArchivo=false;
	m_bNuevoEquipo=false;
}

CUsuario2 &CUsuario2::operator=(const CUsuario2 &user) {

	if (&user!= this)
	{
		m_szNombre= user.m_szNombre;
		m_dwID= user.m_dwID;
		m_dwGrupo= user.m_dwGrupo;
		m_szDescripcion= user.m_szDescripcion;
		m_szCodificado= user.m_szCodificado;
		m_dwData= user.m_dwData;
		m_szNombreCompleto= user.m_szNombreCompleto;
		m_szContrasena= user.m_szContrasena;
		m_dtCreacion= user.m_dtCreacion;
		m_dtVigencia= user.m_dtVigencia;
		m_dtUCambio= user.m_dtUCambio;
		m_dtAcceso= user.m_dtAcceso;
		m_nExpira= user.m_nExpira;
		m_nAutonumerico= user.m_nAutonumerico;
		m_bArchivo= user.m_bArchivo;
		m_bNuevoEquipo= user.m_bNuevoEquipo;
		m_pPermisos.DeleteAll();
		for(int i=0; i<user.m_pPermisos.GetCount(); i++) {
			CMiscelaneosEx *info= user.m_pPermisos[i];
			m_pPermisos.Add(info->GetNombre(), info->GetData());
		}
		CleanHorario();
		for(int i=0; i<user.m_pHorario.GetCount(); i++) {
			CHorario *hor=(CHorario *)user.m_pHorario[i];
			m_pHorario.Add(new CHorario(hor->m_nDia, hor->m_nEntrada, hor->m_nSalida));
		}
	}
	return *this;
}

void CUsuario2::SetExpiracion(int expira) {

	m_nExpira= expira;
}

int CUsuario2::GetExpiracion() const {

	return m_nExpira;
}

CUsuario2::~CUsuario2(void)
{
	CleanHorario();
}

void CUsuario2::SetContrasena(const CString &contrasena) {
	m_szContrasena= contrasena;
}
CString CUsuario2::GetContrasena() const {
	return m_szContrasena;
}

void CUsuario2::SetNombreCompleto(const CString &nombre) {
	m_szNombreCompleto= nombre;
}
CString CUsuario2::GetNombreCompleto() const {
	return m_szNombreCompleto;
}
void CUsuario2::SetCreacion(CMyTime tiempo) {
	m_dtCreacion= tiempo;
}
CMyTime CUsuario2::GetCreacion() const {
	return m_dtCreacion;
}
void CUsuario2::SetVigencia(CMyTime tiempo) {
	m_dtVigencia= tiempo;
}
CMyTime CUsuario2::GetVigencia() const {
	return m_dtVigencia;
}
void CUsuario2::SetUCambio(CMyTime tiempo) {
	m_dtUCambio= tiempo;
}
CMyTime CUsuario2::GetUCambio() const {
	return m_dtUCambio;
}
void CUsuario2::SetAcceso(CMyTime tiempo) {
	m_dtAcceso= tiempo;
}
CMyTime CUsuario2::GetAcceso() const {
	return m_dtAcceso;
}

void CUsuario2::AddHorario(int dia, int entrada, int salida) {
	
	m_pHorario.Add(new CHorario(dia, entrada, salida));
}
CPtrArray *CUsuario2::GetHorario() {

	return &m_pHorario;
}

void CUsuario2::SetArchivo(bool archivo) {
	m_bArchivo=archivo;
}
bool CUsuario2::GetArchivo() const {
	return m_bArchivo;
}

void CUsuario2::SetNuevoEquipo(bool equipo) {
	m_bNuevoEquipo=equipo;
}
bool CUsuario2::GetNuevoEquipo() const {
	return m_bNuevoEquipo;
}

bool CUsuario2::CanEntry(COleDateTime actual) const {

	if(m_pHorario.IsEmpty())
		return true;
	for(int i=0;i<m_pHorario.GetCount();i++) {
		CHorario *hor= (CHorario *)m_pHorario[i];
		// el lunes es el dia 2, y el domingo esta invertido
		if(hor->m_nDia+2==actual.GetDayOfWeek() || (actual.GetDayOfWeek()==1 && hor->m_nDia==6)) {

			COleDateTime entrada, salida;
			entrada.SetDateTime(actual.GetYear(), actual.GetMonth(), actual.GetDay(), hor->m_nEntrada==24 ? 23 : hor->m_nEntrada, hor->m_nEntrada==24 ? 59 : 0, hor->m_nEntrada==24 ? 59 : 0);
			salida.SetDateTime(actual.GetYear(), actual.GetMonth(), actual.GetDay(), hor->m_nSalida==24 ? 23 : hor->m_nSalida, hor->m_nSalida==24 ? 59 : 0, hor->m_nSalida==24 ? 59 : 0);

			if(actual>=entrada && actual<=salida)
				return true;
		}
	}
	return false;
}

void CUsuario2::CleanHorario() {

	for(int i=0;i<m_pHorario.GetCount();i++) {
		CHorario *hor= (CHorario *)m_pHorario[i];
		delete hor;
	}
	m_pHorario.RemoveAll();
}

void CUsuario2::SetAutonumerico(int autonumerico) {

	m_nAutonumerico= autonumerico;
}

int CUsuario2::GetAutonumerico() const {

	return m_nAutonumerico;
}

CMiscelaneosEx *CUsuario2::GetPermisos() {

	return &m_pPermisos;
}

void CUsuario2::AddPermisos(const CString &clase, DWORD permiso) {

	m_pPermisos.Add(clase, permiso);
}

DWORD CUsuario2::GetPermiso(CFormView *view) const {

	ASSERT(view);
	if(view==NULL)
		return 0;
	return GetPermiso(view->GetRuntimeClass()->m_lpszClassName);
}

DWORD CUsuario2::GetPermiso(const CString &clase) const {

	if(!m_dwGrupo)
		return 63;
	DWORD permiso=0;
	CMiscelaneosEx *info =m_pPermisos.Find(clase);
	if(info)
		permiso= info->GetData();
	return permiso;
}

void CUsuario2::SetPermisos(const CMiscelaneosEx &info) {

	m_pPermisos.DeleteAll();
	for(int i=0; i<info.GetCount(); i++) {
		CMiscelaneosEx *temp=info[i];
		m_pPermisos.Add(temp->GetNombre(), temp->GetData());
	}
}


bool CUsuario2::CanRead(const CString &clase) {

	return GetPermiso(clase)>0;
}

bool CUsuario2::CanPrint(const CString &clase) {

	return GetPermiso(clase)>1;
}

bool CUsuario2::CanInsert(const CString &clase) {

	return GetPermiso(clase)>3;
}

bool CUsuario2::CanUpdate(const CString &clase) {

	return GetPermiso(clase)>7;
}

bool CUsuario2::CanDelete(const CString &clase) {

	return GetPermiso(clase)>15;
}

bool CUsuario2::IsAdmin(const CString &clase) {

	return GetPermiso(clase)>31;
}

bool CUsuario2::CanRead(CFormView *view) {

	return GetPermiso(view)>0;
}

bool CUsuario2::CanPrint(CFormView *view) {

	return GetPermiso(view)>1;
}

bool CUsuario2::CanInsert(CFormView *view) {

	return GetPermiso(view)>3;
}

bool CUsuario2::CanUpdate(CFormView *view) {

	return GetPermiso(view)>7;
}

bool CUsuario2::CanDelete(CFormView *view) {

	return GetPermiso(view)>15;
}

bool CUsuario2::IsAdmin(CFormView *view) {

	return GetPermiso(view)>31;
}

DWORD CUsuario2::GetPermiso(const CMiscelaneosEx &permisos,const CString &clase) {

	DWORD permiso=0;
	CMiscelaneosEx *info =permisos.Find(clase);
	if(info)
		permiso= info->GetData();
	return permiso;
}