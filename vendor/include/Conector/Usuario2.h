#pragma once
# include <Componentes/Componentes.h>
#include "usuario.h"

class CHorario {
public:
	CHorario() {
		m_nDia=0;
	}
	CHorario(int dia, int entrada, int salida) {

		m_nDia=dia;
		m_nEntrada= entrada;
		m_nSalida= salida;
	}
public:
	int m_nDia;
	int m_nEntrada;
	int m_nSalida;
};

class __CONECTOR_EXPORT___ CUsuario2 : public CUsuario
{
protected:
	CMiscelaneosEx	m_pPermisos;
	CString			m_szNombreCompleto;
	CString			m_szContrasena;
	CMyTime			m_dtCreacion;
	CMyTime			m_dtVigencia;
	CMyTime			m_dtUCambio;
	CMyTime			m_dtAcceso;
	CPtrArray		m_pHorario;
	int				m_nExpira;
	int				m_nAutonumerico;
	bool			m_bArchivo;
	bool			m_bNuevoEquipo;
public:
	CUsuario2(void);
	CUsuario2(CString Nombre, DWORD id, DWORD grupo);
public:
	~CUsuario2(void);
	void SetContrasena(const CString &contrasena);
	CString GetContrasena() const;
	void SetNombreCompleto(const CString &nombre);
	CString GetNombreCompleto() const;
	void SetCreacion(CMyTime tiempo);
	CMyTime GetCreacion() const;
	void SetVigencia(CMyTime tiempo);
	CMyTime GetVigencia() const;
	void SetUCambio(CMyTime tiempo);
	CMyTime GetUCambio() const;
	void SetAcceso(CMyTime tiempo);
	CMyTime GetAcceso() const;
	void AddHorario(int dia, int entrada, int salida);
	CPtrArray *GetHorario();
	bool CanEntry(COleDateTime actual) const;
	void CleanHorario();
	void SetExpiracion(int expira);
	int GetExpiracion() const;
	void SetAutonumerico(int autonumerico);
	int GetAutonumerico() const;
	void SetArchivo(bool archivo);
	bool GetArchivo() const;
	void SetNuevoEquipo(bool equipo);
	bool GetNuevoEquipo() const;
	CUsuario2 &operator=(const CUsuario2 &user);
	
	// permisos de los usuarios
	CMiscelaneosEx *GetPermisos();
	void AddPermisos(const CString &clase, DWORD permiso);
	DWORD GetPermiso(const CString &clase) const;
	DWORD GetPermiso(CFormView *view) const;
	void SetPermisos(const CMiscelaneosEx &info);


	static DWORD GetPermiso(const CMiscelaneosEx &permisos,const CString &clase);
	bool CanRead(CFormView *view);
	bool CanPrint(CFormView *view);
	bool CanInsert(CFormView *view);
	bool CanUpdate(CFormView *view);
	bool CanDelete(CFormView *view);
	bool IsAdmin(CFormView *view);

	bool CanRead(const CString &clase);
	bool CanPrint(const CString &clase);
	bool CanInsert(const CString &clase);
	bool CanUpdate(const CString &clase);
	bool CanDelete(const CString &clase);
	bool IsAdmin(const CString &clase);
};
