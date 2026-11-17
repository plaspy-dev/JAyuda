#pragma once
#include "afxhtml.h"

class __HHELP_EXPORT__ CFixHtmlView : public CHtmlView
{
public:
	CFixHtmlView(void);
	~CFixHtmlView(void);
	//new code
// Operations
public:
    CString GetFullName() const;
    CString GetType() const;
    CString GetLocationName() const;
    CString GetLocationURL() const;

    void Navigate(LPCTSTR lpszURL, DWORD dwFlags = 0 ,  
                  LPCTSTR lpszTargetFrameName = NULL ,  
                  LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL,  
                  DWORD dwPostDataLen = 0);

//Add the following functions. This action prevents the leak because the 
//CHtmlView versions of LoadFromResource() call CHtmlView::Navigate().
//These need to be defined because CHtmlView::Navigate() is not virtual.
    BOOL LoadFromResource(LPCTSTR lpszResource);
    BOOL LoadFromResource(UINT nRes);
//end new code
};
