#pragma once

# include <winioctl.h>
# include <Windows.h>
#include <stdlib.h>
#include <stdio.h>

class  __CONECTOR_EXPORT___ CDiskID
{
protected:
	CStringArray m_szIDDiscosDuros;
	CStringArray m_saModelosDiscos;
public:
	int GetDiscosDuros();
	CString GetModelName(int index);
	CString GetSerialDisco(int index);
	unsigned int GetSerialNumber(int index=-1);
protected:
	void AddDiscoDuro(CString modelo, CString cadena);
	BYTE IdOutCmd [sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];
public:
	CDiskID(void);
public:
	~CDiskID(void);
protected:
	int ReadPhysicalDriveInNTWithAdminRights();
	char *flipAndCodeBytes (char * str);
	int ReadPhysicalDriveInNTWithZeroRights (void);
	BOOL DoIDENTIFY (HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
		PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
		PDWORD lpcbBytesReturned);
	int ReadDrivePortsInWin9X();
	int ReadIdeDriveAsScsiDriveInNT(void);
	void PrintIdeInfo (int drive, DWORD diskdata [256]);
	char *ConvertToString (DWORD diskdata [256],
		int firstIndex,
		int lastIndex,
		char* buf);
	long getHardDriveComputerID();
};
