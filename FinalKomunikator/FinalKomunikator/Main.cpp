#include <winsock2.h>
#include "resource.h"

#pragma comment(lib, "Ws2_32.lib")

HINSTANCE hInst = NULL; // by móc z wywo³ywaæ nowe okna dialogowe
HWND hwnd;
HWND status;
SOCKET s;
SOCKADDR_IN thread;

BOOL CALLBACK MenuProc( HWND, UINT, WPARAM, LPARAM );    // menu glowne
BOOL CALLBACK ListenProc( HWND, UINT, WPARAM, LPARAM );  // dialog Listen
BOOL CALLBACK ConnectProc( HWND, UINT, WPARAM, LPARAM ); // dialog connect

int Listen(int Portnr);
int Connect(long hostname, int Portnr);
void GetTextandAddLine(char Line[], HWND hParent, int IDC); // funkcja do wpisywania kolejnych stringów do edita

SOCKADDR_IN from;             // do odpowiedzenia na
int fromlen = sizeof(from);   // prosbe o polaczenie

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	hInst = hInstance;
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MENU), NULL, MenuProc);
}

BOOL CALLBACK MenuProc( HWND hwndm, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		case WM_INITDIALOG:
		{
			hwnd = hwndm;
			status = GetDlgItem(hwndm, IDC_MSTATUS);
		}
		break;

		case WM_COMMAND:
		{
			switch(LOWORD(wparam)) // clicki poszczegolnych buttonów
			{
				case IDC_EXIT:
					EndDialog(hwndm, NULL);
				break;

				case IDC_CONNECT:
					return DialogBox(hInst, MAKEINTRESOURCE(IDD_CONNECT), NULL, ConnectProc);
				break;

				case IDC_LISTEN:
					return DialogBox(hInst, MAKEINTRESOURCE(IDD_LISTEN), NULL, ListenProc);
				break;

				case IDC_CANCEL:
					if (s) //If there's a connection
                    {
                        int a = MessageBox(hwndm, "Are you sure you want to end the current connection?",
                                        "End Connection", MB_ICONQUESTION | MB_YESNO);
                        if (a == IDYES)
                        {
                            SendDlgItemMessage(hwndm, IDC_EDIT1, WM_SETTEXT, 0, (LPARAM)"");
                            closesocket(s); //Shut down socket
                            WSACleanup(); //Clean up Winsock
                        }
					}
					break;

				case IDC_SEND:
					int len = GetWindowTextLength(GetDlgItem(hwndm, IDC_EDITSEND));
			        
          	        if (len) // jezeli zostalo cos wpisane
			        {
			            if (s)
			            {
			                char* Data;
			                Data = (char*)GlobalAlloc(GPTR, len + 1);
			                
			                GetDlgItemText(hwndm, IDC_EDITSEND, Data, len + 1);

							char *szTemp;
							szTemp = (char*)GlobalAlloc(GPTR, len + 1);
			                
			                wsprintf(szTemp, "%s\r\n", Data);
			                
			                send(s, szTemp, len+strlen(szTemp)+1, 0); // wysylanie tekstu
			                
			                SetDlgItemText(hwndm, IDC_EDITSEND, ""); // reset edita
			                
			                wsprintf(szTemp, "[You said:] %s\r\n", Data);
			                GetTextandAddLine(szTemp, hwndm, IDC_EDIT1); // dodanie do naszego edita to co napisalismy
			                
			                GlobalFree((HANDLE)Data);   // !!!
							GlobalFree((HANDLE)szTemp); // !!!
			            }
			            else
			            {
			            // w razie gdyby okazalo sie ze nie zostalismy jeszcze polaczeni
			            MessageBox(hwnd, "No established connection detected.",
			              NULL, MB_ICONERROR | MB_OK);
			            }
			        }
				break;
				}
		}
		break;

		case 1045:
		{
			switch (lparam)
			{
				case FD_CONNECT:
					MessageBeep(MB_OK);
					SendMessage(status, WM_SETTEXT, 0, (LPARAM)"Connection Established.");
				break;

				case FD_CLOSE:
                    MessageBeep(MB_ICONERROR);                  
                    if (s) closesocket(s);  // jak ryp³o to trzeba posprzatac socket
                    WSACleanup();                   
                    SendMessage(status, WM_SETTEXT, 0, (LPARAM)"Connection to Remote Host Lost.");                
				break;

				case FD_READ:
					char buffer[80];
                    memset(buffer, 0, sizeof(buffer));
                    recv (s, buffer, sizeof(buffer)-1, 0); // tekst nadeslany   
					GetTextandAddLine(buffer, hwnd, IDC_EDIT1);                    
				break;

				case FD_ACCEPT:
					MessageBeep(MB_OK);

					SOCKET tempsock = accept(s, (struct sockaddr*)&from, &fromlen);
					s = tempsock;

					char AccAdr[100]; // do wyswietlenia w statusie numeru ip
					wsprintf(AccAdr, "Connection from [%s] accepted.",
                      inet_ntoa(from.sin_addr));
					SendMessage(status, WM_SETTEXT, 0, 
                      (LPARAM)AccAdr);
				break;
			}

		}
		default:
            return FALSE;
		break;
	}
	return true;
}

BOOL CALLBACK ListenProc( HWND hwndm, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		case WM_INITDIALOG:
		{
		}
		break;

		case WM_COMMAND:
		{
			switch(LOWORD(wparam)) // clicki poszczegolnych buttonów
			{
				case IDC_LCANCEL:
					EndDialog(hwndm, IDOK);
				break;

				case IDC_LOK:
					if(GetWindowTextLength(GetDlgItem(hwndm, IDC_lEDITPORT)) == 0)
						return 0;
					int portnr = GetDlgItemInt(hwndm, IDC_lEDITPORT, 0, 0);
					if(!Listen(portnr))
					{
						if(s) closesocket(s);
						MessageBox(hwndm, "Error listening on specified port.", "Komunikator9000", MB_ICONERROR | MB_OK);
					}
					EndDialog(hwndm, IDOK);
				break;
			}
		}
		break;

		default:
			return false;
	}
	return true;
}

BOOL CALLBACK ConnectProc( HWND hwndm, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		case WM_INITDIALOG:
		{
		}
			break;

		case WM_COMMAND:
		{
			switch(LOWORD(wparam)) // clicki poszczegolnych buttonów
			{
				case IDC_CCANCEL:
					EndDialog(hwndm, IDOK);
					break;

				case IDC_COK:
					int len = GetWindowTextLength(GetDlgItem(hwndm, IDC_CEDITIP));
					int lenport = GetWindowTextLength(GetDlgItem(hwndm, IDC_CEDITPORT));
					if(lenport == 0)
						return 0; // znaczy ¿e nie zosta³ wpisany port;
					int portnr = GetDlgItemInt(hwndm, IDC_CEDITPORT, 0, 0);

					if(len) // czy wpisane zostalo ip/nazwa
					{
						char* data;
						// globalna alokacja, odpowiednik 'new' tyle ¿e na ni¿szym poziomie
						// GPTR - memory na zero i zwraca wskaŸnik
						data = (char*)GlobalAlloc(GPTR, len + 1); 
						GetDlgItemText(hwndm, IDC_CEDITIP, data, len + 1);

						if(!gethostbyname(data))
						{
							// czyli ip adres, nie nazwa
							long hostname = inet_addr(data);
							if(!Connect(hostname, portnr))
							{
								MessageBox(hwndm, "Could not connect to remote host.", NULL, MB_ICONERROR | MB_OK);
                                if (s) closesocket(s); //Shut down socket
							}
						}
						GlobalFree((HANDLE)data); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						EndDialog(hwndm, IDOK);
					}
					break;
			}

		}
			break;

		default:
			return false;
	}
	return true;
}

int Listen(int Portnr)
{
	WSADATA wsaData;

	int iResult;
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(iResult != 0)
	{
		return 0;
	}

	SOCKADDR_IN addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl (INADDR_ANY);
	addr.sin_port = htons(Portnr);

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s == INVALID_SOCKET)
    {
		SendMessage(status, WM_SETTEXT, 0, (LPARAM)"Could not create socket.");
        return 0;
    }

	if(bind(s, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		SendMessage(status, WM_SETTEXT, 0, (LPARAM)"Could not bind to IP.");
		return 0;
	}
	listen(s, SOMAXCONN);
	WSAAsyncSelect (s, hwnd, 1045, FD_READ | FD_CONNECT | FD_CLOSE | FD_ACCEPT);

	char szTemp[100];
    wsprintf(szTemp, "Listening on port %d...", Portnr);
    
    SendMessage(status, WM_SETTEXT, 0, (LPARAM)szTemp);

	return 1;
}

int Connect(long hostname, int portnr)
{
	WSADATA wsaData;
	SOCKADDR_IN target;

	int iResult;
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(iResult != 0)
	{
		return 0;
	}

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s == INVALID_SOCKET)
	{
		return 0;
	}

	target.sin_family = AF_INET;
	target.sin_addr.s_addr = hostname;
	target.sin_port = htons(portnr);

	if(connect(s, (SOCKADDR*)&target, sizeof(target)) == SOCKET_ERROR)
	{
		// mo¿na rzutowaæ SOCKADDR -> SOCKADDR_IN bo zajmuja ta sam¹ ilosc pamieci! (ba, trzeba)
		return 0;
	}

	WSAAsyncSelect(s, hwnd, 1045, FD_READ | FD_CONNECT | FD_CLOSE | FD_ACCEPT);

	SendMessage(status, WM_SETTEXT, 0, (LPARAM)"Connected to Remote Host.");
    return 1;
}

void GetTextandAddLine(char Line[], HWND hParent, int IDC)
{
	HWND hEdit = GetDlgItem(hParent, IDC);
	int nTxtLen = GetWindowTextLength(hEdit);
	SendMessage(hEdit, EM_SETSEL, nTxtLen, nTxtLen);	// karetka na koniec
	SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)Line);	// append
	SendMessage(hEdit, EM_SCROLLCARET, 0, 0);           
}