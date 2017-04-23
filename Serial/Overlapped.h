
#ifndef __OVERLAPPED_H
#define __OVERLAPPED_H
#include "Serial.h"
#include <Windows.h>


class CAsynSerial {
private:
	CSerial serial;
	LONG    lLastError;
	HANDLE	hevtOverlapped;
	OVERLAPPED ov;
	HANDLE	hevtStop;
	bool	fContinue;

public:
	CAsynSerial();
	LONG Open (LPCTSTR lpszDevice, DWORD dwInQueue, DWORD dwOutQueue);

	LONG Setup(CSerial::EBaudrate eBaudrate, CSerial::EDataBits eDataBits, CSerial::EParity eParity, CSerial::EStopBits eStopBits);
	
	LONG Read (char* szBuffer, int sizeofBuffer);
	LONG Readii (char* szBuffer, int sizeofBuffer);

	LONG Write(char* szBuffer);

	LONG ShowError (LONG lError, LPCTSTR lptszMessage);
};

#endif //__OVERLAPPED_H