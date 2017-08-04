//	Overlapped.cpp - Using CSerial in overlapped mode (single class to use Serial in overlaped mode).
//
//	Copyright (C) 1999-2017 Mario Leal Fuentes (mariolealfuentes@gmail.com)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#define STRICT
#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "Overlapped.h"



CAsynSerial::CAsynSerial() {
		lLastError = ERROR_SUCCESS;
		fContinue = FALSE;
		memset(&ov, 0, sizeof(OVERLAPPED));
}


LONG CAsynSerial::ShowError (LONG lError, LPCTSTR lptszMessage)
{
	// Generate a message text
	TCHAR tszMessage[256];
	wsprintf(tszMessage,_T("%s\n(error code %d)"), lptszMessage, lError);
	
	// Display message-box and return with an error-code
	::MessageBox(0,tszMessage,_T("Listener"), MB_ICONSTOP|MB_OK);
	return 1;
}

LONG CAsynSerial::Open (LPCTSTR lpszDevice, DWORD dwInQueue, DWORD dwOutQueue) {
		// Attempt to open the serial port (COM1)
		lLastError = serial.Open(lpszDevice,dwInQueue,dwOutQueue,true);
		if (lLastError != ERROR_SUCCESS)
			return this->ShowError(serial.GetLastError(), _T("Unable to open COM-port"));
		else {
			serial.Purge();
			fContinue=true;
		}
		return 0;
}

LONG CAsynSerial::Setup(CSerial::EBaudrate eBaudrate, CSerial::EDataBits eDataBits, CSerial::EParity eParity, CSerial::EStopBits eStopBits) {
	if(fContinue == true) {
		//ShowError(lLastError, _T("Serial.SetUP"));
		lLastError = serial.Setup(eBaudrate,eDataBits,eParity,eStopBits);
		if (lLastError != ERROR_SUCCESS)
			return this->ShowError(serial.GetLastError(), _T("Unable to set COM-port setting"));
		// Setup handshaking (default is no handshaking)
		lLastError = serial.SetupHandshaking(CSerial::EHandshakeHardware);
		if (lLastError != ERROR_SUCCESS)
			return this->ShowError(serial.GetLastError(), _T("Unable to set COM-port handshaking"));
		// Register only for the receive event
		lLastError = serial.SetMask(CSerial::EEventBreak |
									CSerial::EEventCTS   |
									CSerial::EEventDSR   |
									CSerial::EEventError |
									CSerial::EEventRing  |
									CSerial::EEventRLSD  |
									CSerial::EEventRecv);
		if (lLastError != ERROR_SUCCESS)
			return this->ShowError(serial.GetLastError(), _T("Unable to set COM-port event mask"));

		// Use 'non-blocking' reads, because we don't know how many bytes
		// will be received. This is normally the most convenient mode
		// (and also the default mode for reading data).
		lLastError = serial.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
		if (lLastError != ERROR_SUCCESS)
			return this->ShowError(serial.GetLastError(), _T("Unable to set COM-port read timeout."));
		// Create a handle for the overlapped operations
		hevtOverlapped = ::CreateEvent(0,TRUE,FALSE,0);;
		if (hevtOverlapped == 0)
			return this->ShowError(serial.GetLastError(), _T("Unable to create manual-reset event for overlapped I/O."));

		// Setup the overlapped structure
		ov.hEvent = hevtOverlapped;

		// Open the "STOP" handle
		hevtStop = ::CreateEvent(0,TRUE,FALSE,_T("Overlapped_Stop_Event"));
		if (hevtStop == 0)
			return this->ShowError(serial.GetLastError(), _T("Unable to create manual-reset event for stop event."));
		// Keep reading data, until an EOF (CTRL-Z) has been received
		fContinue = true;
	}
	return 0;
}
	
LONG CAsynSerial::Write(char* szBuffer) {

	if(fContinue) {
		lLastError = this->serial.Write(szBuffer);
		if (lLastError != ERROR_SUCCESS)
			return ShowError(this->serial.GetLastError(), _T("Unable to Write."));
	}else
		ShowError(0, _T("Unable to Continue, variable 'fContinue' is set to false."));
	return 0;


}

LONG CAsynSerial::Read (char* szBuffer, int sizeofBuffer) {
		serial.Purge();
		while(fContinue)
		{
			
			// Wait for an event
			lLastError = serial.WaitEvent(&ov);
			if (lLastError != ERROR_SUCCESS)
				return ShowError(this->serial.GetLastError(), _T("Unable to wait for a COM-port event."));

			// Setup array of handles in which we are interested
			HANDLE ahWait[2];
			ahWait[0] = hevtOverlapped;
			ahWait[1] = hevtStop;

			// Wait until something happens
			switch (::WaitForMultipleObjects(sizeof(ahWait)/sizeof(*ahWait),ahWait,FALSE,INFINITE))
			{
				case WAIT_OBJECT_0:
				{
					// Save event
					const CSerial::EEvent eEvent = serial.GetEventType();

					// Handle break event
					if (eEvent & CSerial::EEventBreak)
					{
						printf("\n### BREAK received ###\n");
					}

					// Handle CTS event
					if (eEvent & CSerial::EEventCTS)
					{
						printf("\n### Clear to send %s ###\n", serial.GetCTS()?"on":"off");
					}

					// Handle DSR event
					if (eEvent & CSerial::EEventDSR)
					{
						printf("\n### Data set ready %s ###\n", serial.GetDSR()?"on":"off");
					}

					// Handle error event
					if (eEvent & CSerial::EEventError)
					{
						printf("\n### ERROR: ");
						switch (serial.GetError())
						{
						case CSerial::EErrorBreak:		printf("Break condition");			break;
						case CSerial::EErrorFrame:		printf("Framing error");			break;
						case CSerial::EErrorIOE:		printf("IO device error");			break;
						case CSerial::EErrorMode:		printf("Unsupported mode");			break;
						case CSerial::EErrorOverrun:	printf("Buffer overrun");			break;
						case CSerial::EErrorRxOver:		printf("Input buffer overflow");	break;
						case CSerial::EErrorParity:		printf("Input parity error");		break;
						case CSerial::EErrorTxFull:		printf("Output buffer full");		break;
						default:						printf("Unknown");					break;
						}
						printf(" ###\n");
					}

					// Handle ring event
					if (eEvent & CSerial::EEventRing)
					{
						printf("\n### RING ###\n");
					}

					// Handle RLSD/CD event
					if (eEvent & CSerial::EEventRLSD)
					{
						printf("\n### RLSD/CD %s ###\n", this->serial.GetRLSD()?"on":"off");
					}

					// Handle data receive event
					if (eEvent & CSerial::EEventRecv)
					{
						// Read data, until there is nothing left
						DWORD dwBytesRead = 0;
						/*
						do
						{
							//char szBuffer[101];

							// Read data from the COM-port
							//lLastError = this->serial.Read(szBuffer,sizeof(szBuffer)-1,&dwBytesRead);
							lLastError = this->serial.Read(szBuffer,sizeofBuffer-1,&dwBytesRead);
							if (lLastError != ERROR_SUCCESS)
								return this->ShowError(this->serial.GetLastError(), _T("Unable to read from COM-port."));

							if (dwBytesRead > 0)
							{
								// Finalize the data, so it is a valid string
								szBuffer[dwBytesRead] = '\0';

								// Display the data
								printf("%s\n", szBuffer);
							}
						}
						while (dwBytesRead > 0);
						*/

						int count=strlen("ready");
						do
								{
									//char szBuffer[101];

									// Read data from the COM-port
									//lLastError = this->serial.Read(szBuffer,sizeof(szBuffer)-1,&dwBytesRead);
									lLastError = this->serial.Read(szBuffer, count, &dwBytesRead);
									if (lLastError != ERROR_SUCCESS)
										return this->ShowError(this->serial.GetLastError(), _T("Unable to read from COM-port."));
									//count+=dwBytesRead;
								}while (count != dwBytesRead);
						szBuffer[count]='\0';
						printf("%s\n", szBuffer);
						count=0;
						do{
								count=0;
								do
								{
									//char szBuffer[101];

									// Read data from the COM-port
									//lLastError = this->serial.Read(szBuffer,sizeof(szBuffer)-1,&dwBytesRead);
									lLastError = this->serial.Read(&szBuffer[count], 3-count, &dwBytesRead);
									if (lLastError != ERROR_SUCCESS)
										return this->ShowError(this->serial.GetLastError(), _T("Unable to read from COM-port."));
									count+=dwBytesRead;
								}while (count < 3);
								szBuffer[3]='\0';

								int len = atoi(szBuffer);
								do
								{
							
									lLastError = this->serial.Read(&szBuffer[count], len-count, &dwBytesRead);
									if (lLastError != ERROR_SUCCESS)
										return this->ShowError(this->serial.GetLastError(), _T("Unable to read from COM-port."));
									count+=dwBytesRead;
								}while (count < len);
								szBuffer[count]='\0';
								// Display the data
								printf("%s\n", szBuffer);
								count=0;
						}while(1);

					}
				}
				break;

				case WAIT_OBJECT_0+1:
				{
					// Set the continue bit to false, so we'll exit
					fContinue = false;
				}
				break;

				default:
				{
					// Something went wrong
					return ShowError(this->serial.GetLastError(), _T("Error while calling WaitForMultipleObjects."));
				}
				break;
			}
		}

		// Close the port again
		serial.Close();
		return 0;
	}


	LONG CAsynSerial::Readii (char* szBuffer, int sizeofBuffer) {
		//int count;
		//serial.Purge();
		if(fContinue)
		{
			
			// Wait for an event
			lLastError = serial.WaitEvent(&ov);
			if (lLastError != ERROR_SUCCESS)
				return ShowError(this->serial.GetLastError(), _T("Unable to wait for a COM-port event."));

			// Setup array of handles in which we are interested
			HANDLE ahWait[2];
			ahWait[0] = hevtOverlapped;
			ahWait[1] = hevtStop;

			// Wait until something happens
			switch (::WaitForMultipleObjects(sizeof(ahWait)/sizeof(*ahWait),ahWait,FALSE,INFINITE))
			{
				case WAIT_OBJECT_0:
				{
					// Save event
					const CSerial::EEvent eEvent = serial.GetEventType();

					// Handle break event
					if (eEvent & CSerial::EEventBreak)
					{
						printf("\n### BREAK received ###\n");
					}

					// Handle CTS event
					if (eEvent & CSerial::EEventCTS)
					{
						printf("\n### Clear to send %s ###\n", serial.GetCTS()?"on":"off");
					}

					// Handle DSR event
					if (eEvent & CSerial::EEventDSR)
					{
						printf("\n### Data set ready %s ###\n", serial.GetDSR()?"on":"off");
					}

					// Handle error event
					if (eEvent & CSerial::EEventError)
					{
						printf("\n### ERROR: ");
						switch (serial.GetError())
						{
						case CSerial::EErrorBreak:		printf("Break condition");			break;
						case CSerial::EErrorFrame:		printf("Framing error");			break;
						case CSerial::EErrorIOE:		printf("IO device error");			break;
						case CSerial::EErrorMode:		printf("Unsupported mode");			break;
						case CSerial::EErrorOverrun:	printf("Buffer overrun");			break;
						case CSerial::EErrorRxOver:		printf("Input buffer overflow");	break;
						case CSerial::EErrorParity:		printf("Input parity error");		break;
						case CSerial::EErrorTxFull:		printf("Output buffer full");		break;
						default:						printf("Unknown");					break;
						}
						printf(" ###\n");
					}

					// Handle ring event
					if (eEvent & CSerial::EEventRing)
					{
						printf("\n### RING ###\n");
					}

					// Handle RLSD/CD event
					if (eEvent & CSerial::EEventRLSD)
					{
						printf("\n### RLSD/CD %s ###\n", this->serial.GetRLSD()?"on":"off");
					}

					// Handle data receive event
					if (eEvent & CSerial::EEventRecv)
					{
#ifdef C_PRINTF_DEBUG
						printf("\n### Data receive Event ###\n");
#endif//C_PRINTF_DEBUG
						// Read data, until there is nothing left
						DWORD dwBytesRead = 0;
						DWORD count=0;
						do
						{
							// Read data from the COM-port
							//lLastError = this->serial.Read(szBuffer,sizeof(szBuffer)-1,&dwBytesRead);
							lLastError = this->serial.Read(&szBuffer[count], 6-count, &dwBytesRead);
							if (lLastError != ERROR_SUCCESS)
								return this->ShowError(this->serial.GetLastError(), _T("Unable to read from COM-port."));
							count+=dwBytesRead;
						}while (count < 6);
						szBuffer[6]='\0';
#ifdef C_PRINTF_DEBUG
						fprintf(stdout, "Head Read[%s]\n", szBuffer);
#endif//C_PRINTF_DEBUG
						DWORD i = 0;
						//Si esta entre las longuitudes correctas y el string es Stx ...
						while (szBuffer[i] != 'S' && i < 6) i++;
						if ((i >= 2) && (i <= 4) && (szBuffer[i + 1] == 't') /*&& (szBuffer[i + 2] == 'x')*/) {
							char tempBuff[5];
							strncpy(tempBuff, szBuffer, i);
							tempBuff[i]= '\0';
							if (zChar_is_number(tempBuff)) {
#ifdef C_PRINTF_DEBUG
								fprintf(stdout, "zchar_is_number[%s]\n", tempBuff);
#endif//C_PRINTF_DEBUG
								int len = atoi(tempBuff);
								do
								{

									lLastError = this->serial.Read(&szBuffer[count], len - count, &dwBytesRead);
									if (lLastError != ERROR_SUCCESS)
										return this->ShowError(this->serial.GetLastError(), _T("Unable to read from COM-port."));
									count += dwBytesRead;
								} while (count < len);
								szBuffer[count] = '\0';
#ifdef C_PRINTF_DEBUG
								// Display the data
								fprintf(stdout, "All Read[%s]\n", szBuffer);
								//std::cin >> len;
#endif//C_PRINTF_DEBUG
								return 1;
							}
							else {
#ifdef C_PRINTF_DEBUG
								fprintf(stdout, "Not Number Read[%s]\n", szBuffer);
#endif//C_PRINTF_DEBUG
								serial.Purge();
								return 0;
							}


						}
					}
				}
				break;

				case WAIT_OBJECT_0+1:
				{
					// Set the continue bit to false, so we'll exit
					fContinue = false;
				}
				break;

				default:
				{
					// Something went wrong
					return ShowError(this->serial.GetLastError(), _T("Error while calling WaitForMultipleObjects."));
				}
				break;
			}
		}else
			ShowError(0, _T("internal 'fContinue' variable is set to false."));

		// Close the port again
		//serial.Close();
		return 0;
	}

	bool CAsynSerial::zChar_is_number(const char * str) {
		while (*str != '\0')
		{
			if (*str < '0' || *str > '9') {
				if (*str != '.' || *str != '-') {
					return false;
				}
			}
			str++;
		}
		return true;
	}
