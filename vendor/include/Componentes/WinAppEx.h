#pragma once


class __COMPONENTES_EXPORT__ CWinAppXP : public CWinAppEx
{
	// solo por compatibilidad con versiones anteriores
	LPARAM	m_mlparamold;

	DECLARE_DYNCREATE( CWinAppXP )

protected:
	CWinAppXP( LPCTSTR lpszAppName = NULL );
	virtual ~CWinAppXP( void );
	
public:	
	void  SetSaveState(bool save=false);
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	BOOL PostInstanceMessage( WPARAM wParam, LPARAM lParam );		
	BOOL EnableTokenPrivilege( LPCTSTR lpszSystemName, BOOL bEnable = TRUE );
	
public:
	AFX_INLINE void RegisterShellFileTypes( BOOL bCompat = FALSE );
	AFX_INLINE void UnregisterShellFileTypes( void );

public:
	virtual BOOL InitInstance( LPCTSTR lpszUID = NULL );
	virtual BOOL PreTranslateMessage( LPMSG pMsg );
	virtual BOOL OnAnotherInstanceMessage( LPMSG pMsg) ;
	afx_msg void OnFilePrintSetup();

protected:
	BOOL FindAnotherInstance( LPCTSTR lpszUID );
	void RegisterShellFileTypesEx( BOOL bCompat = FALSE, BOOL bRegister = TRUE );
	virtual void OnCommandLine(CString szCommandLine);
	virtual void OnShowApp();
			
protected:
	DECLARE_MESSAGE_MAP()

protected:	
	UINT	m_uMsgCheckInst;
};

AFX_INLINE CWinAppXP* GetApp( void )
	{ return STATIC_DOWNCAST( CWinAppXP, AfxGetApp() ); }

AFX_INLINE void CWinAppXP::RegisterShellFileTypes( BOOL bCompat /*FALSE*/ )
	{ RegisterShellFileTypesEx( bCompat, TRUE ); }

AFX_INLINE void CWinAppXP::UnregisterShellFileTypes( void) 
	{ RegisterShellFileTypesEx( FALSE, FALSE ); }
