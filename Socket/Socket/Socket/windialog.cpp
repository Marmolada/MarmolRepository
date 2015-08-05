#include "WinsockClass.h"
#include "resource.h"

SocketClass sockClass;

BOOL CALLBACK DlgProc( HWND, UINT, WPARAM, LPARAM );
void GetTextandAddLine(char Line[], HWND hParent, int IDC);

SOCKADDR_IN from;             
int fromlen = sizeof(from);

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
}

BOOL CALLBACK DlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		case WM_INITDIALOG:
		{
			if(!sockClass.setSocket(3000, 1045, hwnd))
				MessageBox(hwnd, ":c", "Error", MB_OK); 
			else
			{
				MessageBox(hwnd, "Socket set", "Done", MB_OK);
			}
		}
		break;

		case WM_COMMAND:
		{
			switch(LOWORD(wparam))
			{
				case IDC_OK:
				{
					int len = GetWindowTextLength(GetDlgItem(hwnd, IDC_EDITSEND));

					if(len)
					{
						char* data;
						data = (char*)GlobalAlloc(GPTR, len+1);

						GetDlgItemText(hwnd, IDC_EDITSEND, data, len+1);

						if(!sockClass.sendData(data, len))
							MessageBox(hwnd, "Unable to send data", "Error", MB_OK);

						char* bufferShown;
						bufferShown= (char*)GlobalAlloc(GPTR, 80);
						wsprintf(bufferShown, "[You said:] %s\r\n", data);
						GetTextandAddLine(bufferShown,hwnd,IDC_EDITRECV);

						SetDlgItemText(hwnd, IDC_EDITSEND, "");
						GlobalFree((HANDLE)data);
						GlobalFree((HANDLE)bufferShown);
					}
				}
				break;

				case IDC_EXIT:
				{
					EndDialog(hwnd, NULL);
				}
			}
		}
		break;

		case 1045:
		{
			switch(lparam)
			{
				case FD_READ:
				{
					char* buffer;
					buffer = (char*)GlobalAlloc(GPTR, 80);

					if(sockClass.receiveData(buffer, 80))
						MessageBox(hwnd, buffer, "OK", MB_OK);
					else
						MessageBox(hwnd, "Unable to receive data", "Error", MB_OK);

					char* bufferShown;
					bufferShown= (char*)GlobalAlloc(GPTR, 80);
					wsprintf(bufferShown, "[Incoming:] %s\r\n", buffer);

					GetTextandAddLine(bufferShown,hwnd,IDC_EDITRECV);

					GlobalFree((HANDLE)buffer);			
				}
				break;

				case FD_ACCEPT:
				{
					if(sockClass.acceptConnect())
						MessageBox(hwnd, "Accepted connection", "OK", MB_OK);
					else
						MessageBox(hwnd, "Failed accepting connection", "Error", MB_OK);
				}
				

				case FD_CONNECT:
				{
					MessageBox(hwnd, "Connection Established", "OK", MB_OK);
				}
				break;

				case FD_CLOSE:
				{
					MessageBox(hwnd, "Connection closed", "OK", MB_OK);
					sockClass.resetSocket(3000, 1045, hwnd);
				}
				break;
			}
		}
		break;

		default:
			return FALSE;
		break;
	}
	return true;
}

void GetTextandAddLine(char Line[], HWND hParent, int IDC)
{
	HWND hEdit = GetDlgItem(hParent, IDC);
	int nTxtLen = GetWindowTextLength(hEdit);
	SendMessage(hEdit, EM_SETSEL, nTxtLen, nTxtLen);
	SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)Line);
	SendMessage(hEdit, EM_SCROLLCARET, 0, 0);           
}