#include "COMClass.h"
#include <Process.h>
#include <string>

COMClass::COMClass()
{
	isConnected = false;
}

bool COMClass::Init(std::string portName, DWORD dwBaudRate, BYTE byParity, BYTE byStopBits, BYTE byByteSize)
{
	hCom = CreateFile(portName.c_str (), 
						GENERIC_READ|GENERIC_WRITE, 
						0,
						0,	
						OPEN_EXISTING, 
						FILE_FLAG_OVERLAPPED,
						0
						);
	
	if ( hCom == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	if ( !SetCommMask( hCom , EV_RXCHAR | EV_TXEMPTY ) )
	{
		return false;
	}
		
	DCB dcb = {0};
		
	dcb.DCBlength = sizeof(DCB);
		
	if (!::GetCommState (hCom,&dcb))
	{
		return false;
	}
		
	dcb.BaudRate	= dwBaudRate;
	dcb.ByteSize	= byByteSize;
	dcb.Parity		= byParity;
	if ( byStopBits == 1 )
		dcb.StopBits	= ONESTOPBIT;
	else if (byStopBits == 2 ) 
		dcb.StopBits	= TWOSTOPBITS;
	else 
		dcb.StopBits	= ONE5STOPBITS;

	dcb.fDsrSensitivity = 0;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fOutxDsrFlow = 0;

	if (!::SetCommState (hCom,&dcb))
	{
		return false;
	}
		
	COMMTIMEOUTS timeouts;
		
	timeouts.ReadIntervalTimeout			= MAXDWORD; 
	timeouts.ReadTotalTimeoutMultiplier		= 0;
	timeouts.ReadTotalTimeoutConstant		= 0;
	timeouts.WriteTotalTimeoutMultiplier	= 0;
	timeouts.WriteTotalTimeoutConstant		= 0;
		
	if (!SetCommTimeouts(hCom, &timeouts))
	{
		return false;
	}

	hThreadTerm    = CreateEvent(0,0,0,0);
	hThreadStarted = CreateEvent(0,0,0,0);
	hDataRcvd	   = CreateEvent(0,true,0,0); // event signaling will indicate that data from buffer have been read
	hConnEstabl    = CreateEvent(0,true,0,0);
	hReplyRcvd     = CreateEvent(0,true,0,0);

	hThread = (HANDLE)_beginthreadex( 0,0,COMClass::funcThread,(void*)this,0,0 );

	// waits until thread will be properly set
	WaitForSingleObject ( hThreadStarted , INFINITE );

	//*******************************************************************
	// connection try
	char cDataCnn[] = "|";
	int size = sizeof(cDataCnn);
	sendData(cDataCnn, size);

	//DWORD dwWait = WaitForSingleObject(hDataRcvd, 1000);
	//// if we received an reply
	//if(dwWait == WAIT_OBJECT_0)
	//{
	//	char cDataRpl[] = "|||";
	//	int size = sizeof(cDataRpl);
	//	sendData(cDataRpl, size);

	//	isConnected = true;
	//	SetEvent(hConnEstabl);
	//}
	
	//********************************************************************
	CloseHandle(hThreadStarted);

	return true;
}

bool COMClass::Term()
{
	// we need to end running thread by signaling the thread termination event
	SignalObjectAndWait(hThreadTerm, hThread, INFINITE, FALSE);
	
	cleanHandle( hThread );
	cleanHandle( hThreadStarted );
	cleanHandle( hThreadTerm );
	cleanHandle( hCom );
	cleanHandle( hDataRcvd );

	return true;
}

void COMClass::cleanHandle(HANDLE& hHandle)
{
	CloseHandle( hHandle );
	hHandle = INVALID_HANDLE_VALUE;
}

void* COMClass::getDataEvent(bool param)
{
	if(!param)
		return (void*)hConnEstabl;
	else
		return (void*)hDataRcvd;
}

unsigned _stdcall COMClass::funcThread(void* cParam)
{
	COMClass* comClass = (COMClass*) cParam;

	OVERLAPPED overl;
	memset(&overl,0,sizeof(overl));
	overl.hEvent = CreateEvent( 0,true,0,0 );
	HANDLE arHandles[2];
	arHandles[0] = comClass->hThreadTerm;

	bool abLoop = true;
	DWORD dwWait;
	DWORD dwEventMask=0;
	SetEvent(comClass->hThreadStarted);
	while(abLoop)
	{
		BOOL abRet = WaitCommEvent(comClass->hCom,&dwEventMask, &overl);

		arHandles[1] = overl.hEvent ;
		
		dwWait = WaitForMultipleObjects ( 2,arHandles,FALSE,INFINITE );
		switch ( dwWait )
		{

		case WAIT_OBJECT_0:
			{
				_endthreadex(1);
			}
			break;

		case WAIT_OBJECT_0 + 1:
			{
				DWORD dwMask;
				if( GetCommMask(comClass->hCom,&dwMask) )
				{
					if( dwMask == EV_TXEMPTY )
					{
						ResetEvent ( overl.hEvent );
						continue;
					}
				}
				
				// reading task
				comClass->mtxLocker.lock(); 

				OVERLAPPED overlRead;
				memset(&overlRead,0,sizeof(overlRead));
				overlRead.hEvent = CreateEvent( 0,true,0,0 );
				BOOL abRet = false;
				int accumulate = 0;

				DWORD dwBytesRead = 0;
				do
				{
					ResetEvent( overlRead.hEvent );
					char cBuffer[1];
					int sizeBuffer = sizeof( cBuffer );
					abRet = ReadFile(comClass->hCom, cBuffer, sizeBuffer, &dwBytesRead, &overlRead);
					if (!abRet)
					{
						abLoop = false;
						break;
					}
					if ( dwBytesRead > 0 )
					{
						accumulate+=dwBytesRead;
						comClass->strBuffer.append( cBuffer , dwBytesRead);
					}

				}while(dwBytesRead > 0);

				if( accumulate > 0)
				{
					if( comClass->isConnected )
					{
						SetEvent ( comClass->hDataRcvd );
						//comClass->updateCoordinates();
						comClass->test();
						ResetEvent ( comClass->hDataRcvd );
					}
					else
					{
						if( strcmp(comClass->strBuffer.c_str(), "|") == 0 )
						{
							char cData[] = "||";
							int size = sizeof(cData);
							comClass->sendData(cData, size);
						}
						else if( strcmp(comClass->strBuffer.c_str(), "||") == 0 )
						{
							char cData[] = "|||";
							int size = sizeof(cData);
							comClass->sendData(cData, size);

							comClass->isConnected = true;
							SetEvent(comClass->hConnEstabl);
							comClass->modifyControls(true);
						}
						else if( strcmp(comClass->strBuffer.c_str(), "|||") == 0 )
						{
							comClass->isConnected = true;
							SetEvent(comClass->hConnEstabl); // signal established connection
							comClass->modifyControls(true);
						}
						comClass->strBuffer.clear();
					}
				}

				comClass->mtxLocker.unlock();

				CloseHandle( overlRead.hEvent );
				ResetEvent ( overl.hEvent );
				
			}
			break;

		} // switch
	} // loop

	return 0;
}

std::string COMClass::readData()
{
	mtxLocker.lock();
	std::string strReturn = strBuffer;
	strBuffer.clear();
	mtxLocker.unlock();

	return strReturn;
}

bool COMClass::sendData( const char* paramData, DWORD paramSize )
{
	OVERLAPPED overl;
	memset( &overl , 0, sizeof(overl) );
	overl.hEvent = CreateEvent( 0,true,0,0 );
	DWORD dwBytesWritten = 0;
	int iRet = 0;

	iRet = WriteFile( hCom, paramData, paramSize, &dwBytesWritten, &overl );
	if(iRet == 0)
	{
		WaitForSingleObject(overl.hEvent ,INFINITE);
	}

	CloseHandle(overl.hEvent);

	return true;
}

bool COMClass::isConnectionEstablished()
{
	return isConnected;
}

void COMClass::addControl( HWND& control )
{
	hControls.push_back( control );
}

void COMClass::modifyControls( bool param )
{
	for(int i = 0; i<hControls.size(); i++)
	{
		if(param)
			EnableWindow(hControls[i], true);
		else
			EnableWindow(hControls[i], false);
	}
}

void COMClass::updateCoordinates()
{
	int j = 0;
	int i = 0;
	int k = 0;
	strBuffer = "45|23|31|";
	while(i < strBuffer.size())
	{
		while( strBuffer[i+j] != '|')
		{
			j++;
		}
		std::string strPom = strBuffer.substr(i, j);
		SetWindowText(hControls[k], strPom.c_str());

		i=i+j+1;
		j = 0;
		k++;
	}
	strBuffer.clear();
}

void COMClass::test()
{
	SetWindowText(hControls[3], strBuffer.c_str());
	strBuffer.clear();
}

