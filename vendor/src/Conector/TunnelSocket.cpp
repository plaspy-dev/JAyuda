// TunnelSocket.cpp: archivo de implementación
//

#include "stdafx.h"
# include "conector.h"
#include "TunnelSocket.h"

# define TAMANO 10240

CTunnelSocket::CTunnelSocket()
{
	m_pAuxSocket=NULL;
	m_pRespuesta=NULL;

	m_nTipoProxy= PROXYTYPE_NOPROXY;
	m_nPuertoDestino=0;
	m_nPuertoProxy=0;
	m_nListenPort= 0;
}

CTunnelSocket::~CTunnelSocket()
{
}

void CTunnelSocket::OnReceive(int nErrorCode)
{
	FUNCTION_STACK_TRACE("EEEEEEE1");
	CAsyncProxySocket::OnReceive(nErrorCode);
	if (nErrorCode)
	{
		AfxMessageBox("Fatal error! Network subsystem failed!",MB_ICONSTOP);
		Close();
		return;
	}

	for(;;)
	{
		BYTE buffer[TAMANO];
		int res=Receive(&buffer,TAMANO);

		if (res==SOCKET_ERROR)
		{
			DWORD lasterror= GetLastError();
			if (GetLastError()!=WSAEWOULDBLOCK)
			{
				Close();
				if(m_pAuxSocket) {
					m_pAuxSocket->Close();
				}
				return;
			}
			break;
		} else if(res==0) { // fin de la conexion.
			
			Close();
			if(m_pAuxSocket) {
				m_pAuxSocket->Close();
			}
			return;
		}

		if(m_pAuxSocket) {

			res=m_pAuxSocket->Send(buffer, res);
			if (res==SOCKET_ERROR)
			{
				if (GetLastError()!=WSAEWOULDBLOCK)
				{
					Close();
					if(m_pAuxSocket) {
						m_pAuxSocket->Close();
					}
					return;
				}
			}
		}
	}	
}

void CTunnelSocket::Wait() {

	while(m_nListenPort==0)
		XSleep(100);
}

bool CTunnelSocket::listen(int puerto) {

	BOOL bRet = Create(puerto);
	m_nListenPort= puerto;
	if (bRet != TRUE)
	{
		/*UINT uErr = GetLastError();
		TCHAR szError[256];
		wsprintf(szError, "Server Receive Socket Create() failed: %d", uErr);
		AfxMessageBox(szError);*/
		m_nListenPort=0;
		return false;
	}
	if(!Listen()) {

		Close();
		m_nListenPort=0;
		return false;
	}
	return true;
}

int CTunnelSocket::GetListenPort() const {

	return m_nListenPort;
}

CString CTunnelSocket::GetDestino() const {

	return m_szDestino;
}

int CTunnelSocket::GetPuertoDestino() const {

	return m_nPuertoDestino;
}

void CTunnelSocket::SetProxyDatos(int tipo, const CString &proxy, int puertoproxy, const CString &destino, int puertodestino, const CString &usuario, const CString &contrasena ) {

	m_nTipoProxy= tipo;
	m_szProxy= proxy;
	m_nPuertoProxy= puertoproxy;
	m_szDestino= destino;
	m_nPuertoDestino= puertodestino;
	m_szUsuario= usuario;
	m_szContrasena= contrasena;
}

void CTunnelSocket::OnProxyOperationFailed(int nOpID)
{
	FUNCTION_STACK_TRACE("EEEEEEE2");
	CString mensaje="";
	switch(nOpID) {

		case PROXYERROR_NOCONN:
			mensaje="No se pudo conectar al servidor proxy";
			break;
		case PROXYERROR_REQUESTFAILED:
			mensaje="El servidor proxy rechazo la solicitud.";
			break;
		case PROXYERROR_AUTHREQUIRED:
			mensaje="El servidor proxy requiere autenticación";
			break;
		case PROXYERROR_AUTHTYPEUNKNOWN:
			mensaje="El servidor proxy rechazo el tipo de autenticación";
			break;
		case PROXYERROR_AUTHFAILED:
			mensaje="El servidor proxy rechazo el usuario o la contraseña";
			break;
		case PROXYERROR_AUTHNOLOGON:
			mensaje="El servidor proxy no soporta autenticación";
			break;
		case PROXYERROR_CANTRESOLVEHOST:
			mensaje="No se puede resolver la dirección IP del servidor proxy.";
			break;
		default:
			mensaje="Error conectando al proxy";
			break;
	}
	MessageBox(NULL, mensaje, "Proxy Error", MB_ICONERROR | MB_SETFOREGROUND);
	if(m_pAuxSocket) {

		m_pAuxSocket->Close();
		delete m_pAuxSocket;
		m_pAuxSocket=NULL;
	}
	Close();
}

void CTunnelSocket::OnListenFinished(unsigned long &retProxyIp,int &retProxyPort)
{
	FUNCTION_STACK_TRACE("EEEEEEE3");
}

void CTunnelSocket::OnConnect(int nErrorCode)
{
	FUNCTION_STACK_TRACE("EEEEEEE4");
	CAsyncProxySocket::OnConnect(nErrorCode);
	if (nErrorCode)
	{
		if (GetProxyType())
			MessageBox(NULL, "No se puede conectar al servidor proxy", "Proxy Error", MB_ICONERROR | MB_SETFOREGROUND);
		else
			MessageBox(NULL, "No se puede conectar al servidor", "Proxy Error", MB_ICONERROR | MB_SETFOREGROUND);
		if(m_pAuxSocket) {
			m_pAuxSocket->Close();
			delete m_pAuxSocket;
			m_pAuxSocket=NULL;
		}
		Close();
	}
}

void CTunnelSocket::OnAccept(int nErrorCode)
{
	FUNCTION_STACK_TRACE("EEEEEEE5");
	m_pRespuesta= new CTunnelSocket();
	Accept(*m_pRespuesta);

	if(m_nTipoProxy) {

		m_pAuxSocket= new CTunnelSocket();
		m_pAuxSocket->m_pAuxSocket= m_pRespuesta;
		m_pRespuesta->m_pAuxSocket= m_pAuxSocket;
		if(m_szUsuario.GetLength()>0)
			m_pAuxSocket->SetProxy(m_nTipoProxy,m_szProxy, m_nPuertoProxy, m_szUsuario, m_szContrasena);
		else 
			m_pAuxSocket->SetProxy(m_nTipoProxy,m_szProxy, m_nPuertoProxy);
		m_pAuxSocket->Create();	
		//Connect
		BOOL fOptval = TRUE;
		m_pAuxSocket->SetSockOpt(SO_KEEPALIVE, &fOptval, sizeof(BOOL));

		BOOL res=m_pAuxSocket->Connect(m_szDestino, m_nPuertoDestino);
		if (!res)
			if (GetLastError()!=WSAEWOULDBLOCK)
				MessageBox(NULL, "Error, No se puede conectar al servidor proxy!", "Proxy Error", MB_ICONERROR | MB_SETFOREGROUND);
	}
	CAsyncProxySocket::OnAccept(nErrorCode);
}

void CTunnelSocket::OnClose(int nErrorCode)
{
	FUNCTION_STACK_TRACE("EEEEEEE7");
	if(m_pAuxSocket) {
		m_pAuxSocket->Close();
		delete m_pAuxSocket;
		m_pAuxSocket=NULL;
	}
	Close();
	CAsyncProxySocket::OnClose(nErrorCode);
}
