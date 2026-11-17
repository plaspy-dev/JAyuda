#pragma once

#include "AsyncProxySocket.h"

class CTunnelSocket : public CAsyncProxySocket  
{
protected:
	CTunnelSocket	*m_pAuxSocket;
	CTunnelSocket	*m_pRespuesta;
	int			m_nTipoProxy;
	CString		m_szProxy;
	int			m_nPuertoProxy;
	CString		m_szDestino;
	int			m_nPuertoDestino;
	CString		m_szUsuario;
	CString		m_szContrasena;
	int			m_nListenPort;
public:
	CTunnelSocket();
	virtual ~CTunnelSocket();
	void SetProxyDatos(int tipo, const CString &proxy, int puertoproxy, const CString &destino, int puertodestino, const CString &usuario, const CString &contrasena );
	virtual void OnConnect(int nErrorCode);
	virtual void OnProxyOperationFailed(int nOpID);
	virtual void OnListenFinished(unsigned long &retProxyIp,int &retProxyPort);
	virtual void OnReceive(int nErrorCode);
	virtual void OnAccept(int nErrorCode);
	bool listen(int puerto);
	CString GetDestino() const;
	int GetPuertoDestino() const;
	int GetListenPort() const;
	virtual void OnClose(int nErrorCode);
	void Wait();
};