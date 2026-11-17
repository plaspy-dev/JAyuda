#pragma once
#include "afxcmn.h"
#include "afxwin.h"
# include <resource.h>
# include <Componentes/Componentes.h>
// CDlgBases dialog

class CDlgBases : public CDialog
{
	DECLARE_DYNAMIC(CDlgBases)

public:
	CDlgBases(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgBases();

// Dialog Data
	enum { IDD = IDD_DIALOGCONEXIONES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	bool		m_bModificar;
	int			m_nSeleccion;
	CMyListCtrl m_LCDB;
	CEditXP		m_ENombreServidor;
	CEditXP		m_ENombreDB;
	CEditXP		m_EPuerto;
	CEditXP		m_EReintentos;
	CButtonXP	m_BOK;
	CButtonXP	m_BCancelar;
	CSectionSeparator m_SPrimero;
	CSectionSeparator m_SSegundo;
	CSeparator m_SFinProxy;
	CComboBoxXP	m_CBTipo;
	CEditXP		m_EServidor;
	CEditXP		m_EPuertoProxy;
	CEditXP		m_EUsuario;
	CEditXP		m_EContrasena;
	CEditXP		m_EKeepAlive;

	afx_msg void OnBnClickedOk();
	afx_msg void OnNMRclickListdb(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListdb(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEditservidor();
	afx_msg void OnListasEditar();
	afx_msg void OnListasEliminar();
	afx_msg void OnBnClickedCancel();
public:
	CString m_szArchivo;
	CEditXP m_EArchivo;
public:
	CButtonXP m_BExaminar;
public:
	afx_msg void OnBnClickedBexaminar();
	afx_msg void OnCbnSelChangeCProxy();
	CEditXP m_ELecturaTimeout;
};
