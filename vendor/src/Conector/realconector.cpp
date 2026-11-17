#include "StdAfx.h"
# include "conector.h"
#include "realconector.h"
# include "ThreadMySQL.h"
# include "resource.h"
# include <atlbase.h>
// tamaño maximo del archivo de logs 1Mega
# define MAX_LOG_SIZE 1*1024*1024

CTunnelSocketThread *CConector::m_pTunnelThread=NULL;

CConector::CConector() {

	FUNCTION_STACK_TRACE("FFFFC010");
	pMainFrame= NULL;
	m_bRunning= false;
	m_bModal= false;
	m_nReintentos= CMyRegKey(CApplicacion::GetAppKey()).GetProfileInt("control", "retry", 3);
	m_bVerbose=true;
	m_nLastState= -1;
	m_pThread= NULL;
	m_pThreadOld= NULL;
	m_pThreadTimer=NULL;
}

CConector::~CConector() {

	OnDestroy();
}

void CConector::OnDestroy() {

	FUNCTION_STACK_TRACE("FFFFC011");
	m_pMutex.Lock();
	m_pIDs.RemoveAll();
	m_pQuerys.RemoveAll();	
	m_pArrayCWnd.RemoveAll();
	m_pBinary.RemoveAll();
	m_nDatabase.RemoveAll();
	m_pMutex.Unlock();
	if(m_pThread) {
		m_pThread->m_pParent= NULL;
		m_pThread->GetArrayDB()->SetPadre(NULL);
		MSG msg;

		while(m_pThread && m_pThread->IsRunning()) {
			if(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		if(m_pThreadOld) {
			m_pThreadOld->Kill();
			m_pThreadOld=NULL;
		}
		ASSERT(!m_pThread);
		if(m_pThread) {
			m_pThread->Kill();
			m_pThread=NULL;
		}
	}
	if(m_pThreadTimer) {
		m_pThreadTimer->Kill();
		m_pThreadTimer->m_pConector=NULL;
		m_pThreadTimer=NULL;
	}
}

void CConector::SetMainFrame( CWnd *parent) {

	pMainFrame= parent;
	m_pArrayDB.SetMainFrame(parent);
	m_pArrayDBModal.SetMainFrame(parent);
}

/* = operator definition */
CConector& CConector::operator=(CConector &conector)
{
	FUNCTION_STACK_TRACE("FFFFC012");
	if (&conector != this)
	{
		if(m_pThreadTimer) {
			m_pThreadTimer->Kill();
			m_pThreadTimer->m_pConector=NULL;
			m_pThreadTimer=NULL;
		}
		m_pArrayDB.RemoveAll();
		for ( int i=0; i<conector.m_pArrayDB.GetCount(); i++) {

			m_pArrayDB.Add( conector.m_pArrayDB.GetAt(i));
			if( !conector.m_pArrayDB.GetAt(i)->CanConnect()) {
				m_pArrayDB.GetAt(i)->SetCanConnect(false);
			}
		}
		m_pArrayDBModal.RemoveAll();
		for( int i=0; i<conector.m_pArrayDBModal.GetCount(); i++) {

			m_pArrayDBModal.Add(conector.m_pArrayDBModal.GetAt(i));
			if( !conector.m_pArrayDBModal.GetAt(i)->CanConnect()) {
				m_pArrayDBModal.GetAt(i)->SetCanConnect(false);
			}
		}
		pMainFrame= conector.pMainFrame;
	}
	return *this;
}

void CConector::SetTimeOut(unsigned int timeout) {

	m_pArrayDB.SetTimeOut(timeout);
	m_pArrayDBModal.SetTimeOut(timeout);
}

bool CConector::EsIncrustado() const {

	return m_pArrayDB.EsIncrustado();
}

bool CConector::Ping() {

	FUNCTION_STACK_TRACE("FFFFC013");
	bool retorno=false;
	if( !m_bRunning) {
		m_pMutex.Lock();
		retorno= m_pArrayDB.Ping();
		m_pMutex.Unlock();
	}
	if( !m_bModal) {
		m_pArrayDBModal.Lock();
		retorno= m_pArrayDBModal.Ping();
		m_pArrayDBModal.UnLock();
	}
	return retorno;
}

bool CConector::AddDataBase(CString nombreDB, CString user, CString passwd, CString host, int port, bool incrustado/*=false*/) {

	FUNCTION_STACK_TRACE("FFFFC014");
	CArrayDB nueva;

	if(port==0)
		port=3306;

	nueva.SetHost(host);
	nueva.SetDB(nombreDB);
	nueva.SetPassword(passwd);
	nueva.SetUser(user);
	nueva.SetPort(port);
	nueva.SetPadre(pMainFrame);
	nueva.SetIncrustado(incrustado);

	m_pArrayDB.Add( &nueva);
	m_pArrayDBModal.Add(&nueva);

	m_pArrayDB.SetMainFrame(pMainFrame);
	m_pArrayDBModal.SetMainFrame(pMainFrame);
	
	return true;
}

void CConector::CopyState() {

	for( int i=0; i<m_pArrayDB.GetCount(); i++) {
		if( !m_pArrayDB.GetAt(i)->CanConnect()) {
			m_pArrayDBModal.GetAt(i)->SetCanConnect(false);
		}
	}
}

CString	CConector::GetHost() const {

	return (m_pTunnelThread && m_pTunnelThread->m_pTunnelSocket) ? m_pTunnelThread->m_pTunnelSocket->GetDestino() : m_pArrayDB.GetHost();
}

int	CConector::GetPort() const {

	return (m_pTunnelThread && m_pTunnelThread->m_pTunnelSocket) ? m_pTunnelThread->m_pTunnelSocket->GetPuertoDestino() : m_pArrayDB.GetPort();
}

bool CConector::IsEmbedded() const {

	return m_pArrayDB.EsIncrustado();
}


CString CConector::GetNombreDatabase() {
	
	FUNCTION_STACK_TRACE("FFFFC015");
	ASSERT(m_pArrayDB.GetCount());
	if( m_pArrayDB.GetCount())
		return m_pArrayDB.GetDB();
	else return NULL;
}

CArrayDB *CConector::GetDatabaseModalAt(int index) {

	return m_pArrayDBModal.GetAt(index);
}

CArrayDB *CConector::GetDatabaseAt(int index) {

	FUNCTION_STACK_TRACE("FFFFC016");
	return m_pArrayDB.GetAt(index);
}

int CConector::GetDatabasesCount() {
	FUNCTION_STACK_TRACE("FFFFC017");
	return m_pArrayDB.GetCount();
}

void CConector::CloseConections() {

	// ASSERT(m_bRunning==false);
	CString usuario, contrasena;

	ASSERT(m_pArrayDB.GetCount());
	if(m_pArrayDB.GetCount()) {

		usuario= m_pArrayDB.GetAt(0)->GetUser();
		contrasena= m_pArrayDB.GetAt(0)->GetPassword();
	}
	else return;
	if(m_pThreadTimer) {
		m_pThreadTimer->Kill();
		m_pThreadTimer->m_pConector=NULL;
		m_pThreadTimer=NULL;
	}
	m_pArrayDB.RemoveAll();
	m_pArrayDBModal.RemoveAll();
	Cargar();
	SetUser(usuario);
	SetPasswd(contrasena);
}

int CConector::QueryEx( CString query, int id, CWnd *parent) {

	int i=0;
	for( ; i<m_pArrayDB.GetCount(); i++) 
		Query(query, id, parent, i);
	return i;
}

void CConector::ResetState() {
    
	m_pArrayDB.ResetState();
	m_pArrayDBModal.ResetState();
}

int CConector::QueryBinary( CString query, int id, CWnd *parent, int usedatabase) {

	FUNCTION_STACK_TRACE("FFFFC018");
	if( pMainFrame && ::IsWindow(pMainFrame->GetSafeHwnd()))

			pMainFrame->PostMessage( WM_TREADRESULT, 1, 0);
	if(query.IsEmpty()) {
		ASSERT(false);
		return 0;
	}
	m_pMutex.Lock();
	m_pQuerys.Add( query);
	m_pArrayCWnd.Add(parent);
	m_pBinary.Add(1);
	if( usedatabase<0 || usedatabase>=m_pArrayDB.GetCount())
		usedatabase= 0;
	m_nDatabase.Add(usedatabase);
	m_pIDs.Add(id);
	m_pMutex.Unlock();
	RealQuery();
	return 0;
}

int CConector::Query( CString query, int id, CWnd *parent, int usedatabase) {

	FUNCTION_STACK_TRACE("FFFFC018");
	if(query.IsEmpty()) {
		ASSERT(false);
		return 0;
	}
	if( pMainFrame && ::IsWindow(pMainFrame->GetSafeHwnd()))
		pMainFrame->PostMessage( WM_TREADRESULT, 1, 0);

	m_pMutex.Lock();
	m_pQuerys.Add( query);
	m_pArrayCWnd.Add(parent);
	m_pBinary.Add(0);
	if( usedatabase<0 || usedatabase>=m_pArrayDB.GetCount())
		usedatabase= 0;
	m_nDatabase.Add(usedatabase);
	m_pIDs.Add(id);
	m_pMutex.Unlock();
	RealQuery();
	return 0;
}

void CConector::RealQuery() {
	
	m_pMutex.Lock();
	FUNCTION_STACK_TRACE("FFFFC019");
	if( !m_pIDs.IsEmpty()) {

		if( !m_bRunning) {
		
			m_bRunning= true;
			m_nLastState= -1;
			m_pMutex.Unlock();
			if( pMainFrame && ::IsWindow(*pMainFrame))
				pMainFrame->SendMessage(WM_SETSTATUS, 0, (LPARAM)(const char *)"Realizando Consulta");
			m_pMutex.Lock();
			if(m_pIDs.IsEmpty()) {
				m_pMutex.Unlock();
				return;
			}
			m_pThread= new CThreadMySQL(m_pThreadOld);
			if(m_pIDs.GetCount()==0) {
				m_pMutex.Unlock();
				return;
			}

			m_pThread->m_nID= m_pIDs.GetAt(0);
			m_pIDs.RemoveAt(0);
			CWnd *handle=(CWnd *) m_pArrayCWnd.GetAt(0);
			m_pThread->m_pParent= handle;
			m_pThread->m_nType= m_pBinary.GetAt(0);
			m_pThread->m_pConector= this;
			m_pThread->m_szQuery= m_pQuerys.GetAt(0);
			m_pThread->SetArrayDB( m_pArrayDB.GetAt(m_nDatabase.GetAt(0)));
			m_pThread->m_pMainWnd= m_bVerbose ? pMainFrame : NULL;
			m_pThread->m_nReintentos= m_nReintentos;
			
			m_pThreadOld= NULL;
			m_pQuerys.RemoveAt(0);
			m_nDatabase.RemoveAt(0);
			m_pArrayCWnd.RemoveAt(0);
			m_pBinary.RemoveAt(0);
			m_pThread->Run();
		}
	}
	m_pMutex.Unlock();
}

void CConector::SetUser(CString user) {

	FUNCTION_STACK_TRACE("FFFFC01A");
	CArrayDB *info=NULL;
	for( int i=0; i<m_pArrayDB.GetCount(); i++) {
		info= m_pArrayDB.GetAt(i);
		info->SetUser(user);
		info= m_pArrayDBModal.GetAt(i);
		info->SetUser(user);
	}
}

bool CConector::KillQuery(DWORD id, CWnd *parent) {
	
	FUNCTION_STACK_TRACE("FFFFC01B");
	CWaitCursor wait;
	m_pMutex.Lock();
	for( int i=0; i<m_pIDs.GetCount(); i++) {
		if( (m_pIDs.GetAt(i)==id || id==-1) && m_pArrayCWnd.GetAt(i)==parent) { //esta en cola
			m_pIDs.RemoveAt(i);
			m_pQuerys.RemoveAt(i);
			m_nDatabase.RemoveAt(i);
			m_pArrayCWnd.RemoveAt(i);
			m_pBinary.RemoveAt(i);
			if(pMainFrame && ::IsWindow(*pMainFrame))
				pMainFrame->PostMessage( WM_TREADRESULT, 0, 0);
			i--;
		}
	}
	if( m_pThread) {
		if( (m_pThread->m_nID==id || id==-1 )&& m_pThread->m_pParent==parent) {
			m_pThread->m_pParent= NULL;
			m_pThread->GetArrayDB()->SetPadre(NULL);
		}
	}
	m_pMutex.Unlock();
	return true;
}

void CConector::SetPasswd(CString passwd) {

	FUNCTION_STACK_TRACE("FFFFC01C");
	CArrayDB *info=NULL;
	for( int i=0; i<m_pArrayDB.GetCount(); i++) {

		info= m_pArrayDB.GetAt(i);
		info->SetPassword(passwd);
		info= m_pArrayDBModal.GetAt(i);
		info->SetPassword(passwd);
	}
}

ULONG CConector::QueryModal( CString query, CStringArray *valores/*=NULL*/, CStringArray *columnas/*=NULL*/) {

	CWaitCursor wait;
	if(query.IsEmpty()) {
		ASSERT(FALSE);
		return -1;
	}
	while(true) { // esperamos que se terminen todas las consultas
		m_pMutex.Lock();
		if( m_pIDs.IsEmpty() && !m_bRunning) 
			break;
		m_pMutex.Unlock();

		MSG msg;
		if(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	m_bRunning=true;

	m_bModal= true;
	ULONG result= 0;
	for( int i=0; i<m_pArrayDB.GetCount(); i++) {
		CArrayDB *info=m_pArrayDB.GetAt(i);
		if( !info)
			return -1;
		info->Lock();
		info->SetPadre(NULL);
		info->SetRetry(m_nReintentos);
		if( (result= info->Find(query, valores, columnas))!=-1) {
			if(pMainFrame && ::IsWindow(*pMainFrame))
				pMainFrame->PostMessage(WM_TREADRESULT,2, i);
			info->UnLock();
			break;
		}
		info->UnLock();
	}
	m_bModal= false;
	m_bRunning=false;
	m_pMutex.Unlock();
	RealQuery();
	return result;
}

ULONG CConector::Find( CString query, CWnd *parent, CStringArray *valores, CStringArray *columnas) {

	FUNCTION_STACK_TRACE("FFFFC01D");

	if(query.IsEmpty()) {
		ASSERT(FALSE);
		return -1;
	}
	m_bModal= true;
	ULONG result= 0;
	for( int i=0; i<m_pArrayDBModal.GetCount(); i++) {
		CArrayDB *info=m_pArrayDBModal.GetAt(i);
		if( !info)
			return -1;
		info->SetPadre(parent);
		info->SetRetry(m_nReintentos);
		if( (result= info->Find(query, valores, columnas))!=-1) {
			if(pMainFrame && ::IsWindow(*pMainFrame))
				pMainFrame->PostMessage(WM_TREADRESULT,2, i);
			break;
		}
	}
	m_bModal= false;
	return result;
}

bool CConector::TestConection(CWnd *parent) {

	FUNCTION_STACK_TRACE("FFFFC01E");
	CArrayDB *info=NULL;
	for( int i=0; i<m_pArrayDB.GetCount(); i++) {
		info= m_pArrayDB.GetAt(i);
		info->SetPadre(parent);
		if(info->Conectar(0, false))
			return true;
	}
	return false;
}

CString CConector::GetPasswd() {

	ASSERT(m_pArrayDB.GetCount());
	if(m_pArrayDB.GetCount())
		return m_pArrayDB.GetAt(0)->GetPassword();
	else return NULL;
}

CString CConector::GetUser() {

	FUNCTION_STACK_TRACE("FFFFC01F");
	ASSERT(m_pArrayDB.GetCount());
	if(m_pArrayDB.GetCount())
		return m_pArrayDB.GetAt(0)->GetUser();
	else return NULL;
}

bool CConector::RealDeleteThread() {

	FUNCTION_STACK_TRACE("FFFFC020");
	ASSERT(m_bRunning);
	m_bRunning= false;
	m_pThreadOld= m_pThread;
	m_pThread= NULL;
	if( pMainFrame && ::IsWindow(pMainFrame->GetSafeHwnd()))
		pMainFrame->SendMessage(WM_SETSTATUS, 0, (LPARAM)(const char *)"Listo");
	RealQuery();
	return true;
}

bool CConector::Guardar(HKEY hKey/*= HKEY_CURRENT_USER*/) {

	FUNCTION_STACK_TRACE("FFFFC021");

	// limpiamos el registro
	CMyRegKey myregkey(hKey, CApplicacion::GetAppKey());
	myregkey.DeleteRegKey("", "conector");

	CArrayDB *info=NULL;
	CString clave;

	for( int i=0; i<m_pArrayDB.GetCount(); i++) {

		info= m_pArrayDB.GetAt(i);
		ASSERT(info);
		clave.Format("conector\\servidor%d", i);
		myregkey.WriteString(clave, "servidor", info->GetHost());
		myregkey.WriteInt(clave, "puerto", info->GetPort());
		myregkey.WriteString(clave, "db", info->GetDB());
		myregkey.WriteInt(clave, "incrustado", info->EsIncrustado());
	}
	return true;
}

CTunnelSocket *CConector::GetTunnel() {

	return m_pTunnelThread ? m_pTunnelThread->m_pTunnelSocket : NULL;
}

bool CConector::Cargar(HKEY hKey/*= HKEY_CURRENT_USER*/) {
	
	FUNCTION_STACK_TRACE("FFFFC022");
	
	CString usuario, contrasena;
	CString clave, servidor;
	
	usuario= m_pArrayDB.GetUser();
	contrasena= m_pArrayDB.GetPassword();
	
	m_pArrayDB.RemoveAll();
	m_pArrayDBModal.RemoveAll();

	CMyRegKey key;
	key.MyOpen(hKey, CApplicacion::GetAppKey());

	if(m_pTunnelThread==NULL && key.GetInt("conectorp\\proxy", "tipo", 0)!=PROXYTYPE_NOPROXY) {

		m_pTunnelThread= new CTunnelSocketThread(key.GetInt("conectorp\\proxy", "tipo", 0), 
			key.GetString("conectorp\\proxy", "servidor", ""), 
			atoi(key.GetString("conectorp\\proxy", "puerto", "")), 
			key.GetString("conector\\servidor0","servidor"),  // servidor de destino
			key.GetInt("conector\\servidor0", "puerto", MYSQL_PORT),  // puerto destino
			key.GetString("conectorp\\proxy", "usuario", ""), 
			key.GetString("conectorp\\proxy", "contrasena", ""));
		m_pTunnelThread->Run(AfxGetMainWnd());
		m_pTunnelThread->Wait();
	}

	m_nReintentos= key.GetInt("control", "retry", 3);
	for( int i=0; ;i++) {
		clave.Format("conector\\servidor%d", i);
		servidor= key.GetString(clave,"servidor");
		if(servidor.IsEmpty())
			break;
		CString nombre=CApplicacion::GetAppName();
		nombre.MakeLower();
		if(m_pTunnelThread) {

			AddDataBase( key.GetString(clave, "db", nombre), "","", 
				"127.0.0.1", m_pTunnelThread->m_pTunnelSocket->GetListenPort(), 
				key.GetInt(clave, "incrustado", 0)
				); // usamos al servidor localhost
		} else {

			AddDataBase( key.GetString(clave, "db", nombre), "","", 
				servidor, key.GetInt(clave, "puerto", MYSQL_PORT), 
				key.GetInt(clave, "incrustado", 0)
				);
		}
	}
	SetUser(usuario);
	SetPasswd(contrasena);

	m_pArrayDB.SetMainFrame(pMainFrame);
	m_pArrayDBModal.SetMainFrame(pMainFrame);

	// establecemos el timeout, esto para que abra la conexion al proxy
	if(m_pThreadTimer==NULL && CMyRegKey::GetProfileInt("control", "keepalive")) {
		m_pThreadTimer= new CTimerThread(this);
		m_pThreadTimer->Run();
	}

	return m_pArrayDB.GetCount()>0;
}

CTunnelSocketThread::CTunnelSocketThread(int tipo, CString proxy, int puerto, CString destino, int puertodestino, CString usuario, CString contrasena) {

	m_pTunnelSocket= new CTunnelSocket();
	m_pTunnelSocket->SetProxyDatos(tipo, 
		proxy, 
		puerto, 
		destino,  // servidor de destino
		puertodestino,  // puerto destino
		usuario, 
		contrasena);
}

CTunnelSocketThread::~CTunnelSocketThread() {

}

bool CTimerThread::Init() {		

	while(IsRunning() && m_pConector) {
		int keepalive= CMyRegKey::GetProfileInt("control", "keepalive", 10);
		if(keepalive==0) {
			if(m_pConector)
				m_pConector->m_pThreadTimer=NULL;
			m_pConector=NULL;
			Kill();
			break;
		}
		if(IsRunning() && m_pConector) {
			m_pConector->Ping();
		}
		Sleep(keepalive*1000);
	}
	if(m_pConector)
		m_pConector->m_pThreadTimer=NULL;
	return CThreadAuto::Init();
}

CTimerThread::CTimerThread(CConector *conector) {

	m_pConector= conector;
}

CTimerThread::~CTimerThread() {

}



class CAuxSocketDiag : public CDialog
{
	DECLARE_DYNAMIC(CAuxSocketDiag)
public:
	CAuxSocketDiag(CWnd *parent=NULL);   // standard constructor
	virtual ~CAuxSocketDiag();
	CWnd	*m_wndMainWindow;
	// Dialog Data
	enum { IDD = IDD_DWEBPASSWORD };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void OnWindowPosChanging( WINDOWPOS* lpwndpos );
	void OnBnClicked();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

IMPLEMENT_DYNAMIC(CAuxSocketDiag, CDialog)

CAuxSocketDiag::CAuxSocketDiag(CWnd *parent/*=NULL*/)
: CDialog(CAuxSocketDiag::IDD, NULL)
{
	m_wndMainWindow= parent;
}

CAuxSocketDiag::~CAuxSocketDiag()
{
}

void CAuxSocketDiag::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAuxSocketDiag, CDialog)
	ON_WM_WINDOWPOSCHANGING()
	ON_BN_CLICKED(IDOK, OnBnClicked)
	ON_BN_CLICKED(IDCANCEL, OnBnClicked)
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

BOOL CAuxSocketDiag::OnInitDialog()
{
	CDialog::OnInitDialog();
	CWnd wnd;
	if (wnd.SubclassDlgItem(IDC_EWEBBROWSER, this)) 
		wnd.DestroyWindow();
	return TRUE;
}

void CAuxSocketDiag::OnBnClicked() {}
void CAuxSocketDiag::OnWindowPosChanging( WINDOWPOS* lpwndpos )
{
	lpwndpos->flags &= ~SWP_SHOWWINDOW;
	CDialog::OnWindowPosChanging(lpwndpos);
}

void CAuxSocketDiag::OnSetFocus(CWnd* pOldWnd)
{
	// m_wndMainWindow->BringWindowToTop();
	m_wndMainWindow->SetFocus();
	m_wndMainWindow->SetForegroundWindow();
}

bool CTunnelSocketThread::Init() {

	int puerto=3306;
	do {
		if(puerto==-1)
			puerto= rand();
		if(!m_pTunnelSocket->listen(puerto))
			puerto=-1;
	} while(puerto==-1);

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CAuxSocketDiag diag((CWnd *)m_pParametro);
	diag.DoModal();
	delete m_pTunnelSocket;
	m_pTunnelSocket=NULL;

	return true;
}

void CTunnelSocketThread::Wait() {
	
	if(m_pTunnelSocket)
		m_pTunnelSocket->Wait();
}