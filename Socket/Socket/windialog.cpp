#include "WinsockClass.h"
#include "resource.h"

SocketClass sockClass;

BOOL CALLBACK DlgProc( HWND, UINT, WPARAM, LPARAM );
void GetTextandAddLine(char Line[], HWND hParent, int IDC);

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
				MessageBox(hwnd, (LPCTSTR)sockClass.socketErrorInfo(), "Error", MB_OK); 
			else
				MessageBox(hwnd, "Socket set", "Done", MB_OK);
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
						if(!sockClass.sendTo(data, len))
							MessageBox(hwnd, "Unable to send datagram", "Error", MB_OK);
						SetDlgItemText(hwnd, IDC_EDITSEND, "");
						GlobalFree((HANDLE)data);
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
					//MessageBox(hwnd, "odebrano", "asd", MB_OK);
					char buffer[80];
					memset(buffer, 0, sizeof(buffer));
					sockClass.receiveData(buffer, sizeof(buffer));
					GetTextandAddLine(buffer,hwnd,IDC_EDITRECV);
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