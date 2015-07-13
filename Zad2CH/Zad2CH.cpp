#include "Zad2CH.h"
#include "resource.h"
#include <xsi_viewcontext.h>

extern HINSTANCE g_Instance; // HINSTANCE - bazowy adres do modu³u pamieci
using namespace XSI; 

LRESULT CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
////////////////////////////////////////////////////////////////////////////////
//////// implementacja metod CCustomUI

//// konst/dekonstr
CCustomUI::CCustomUI(){}
CCustomUI::~CCustomUI(){}

//// Init - utworzenie dialogu z uchwytem zdefiniowanym przez view context (czyli przez argument wywolujacy!)
LRESULT CCustomUI::Init( CRef& in_pViewCtx )
{
	ViewContext myViewContext = in_pViewCtx;
	// assert - jezeli wartosc argumentu rowna zero -> zabija proces
	// przydatne do debugowania; mozna wylaczyc wszystkie assert instrukcja #define NDEBUG 
	assert( myViewContext.IsValid() );

	hwnd = CreateDialog( g_Instance, MAKEINTRESOURCE(IDD_MENU), (HWND)myViewContext.GetParentWindowHandle(), (DLGPROC)DlgProc);
	return S_OK;
}

//// Term - zniszczenie dialogu (ewen. obsluzenie zaalokowanej pamieci)
LRESULT CCustomUI::Term( CRef& in_pViewCtx )
{
	DestroyWindow(hwnd);
	hwnd = NULL;

	return S_OK;
}

//// Notify - obsluga zdarzen, eventy w XSI -> dialog
/*LRESULT CCustomUI::Notify( CRef& in_pViewCtx )
{
	return S_OK;
}*/

//////// koniec implementacji metod 
///////////////////////////////////////////////////////////////
//////// obsluga zdarzen wewnatrz dialogu

LRESULT CALLBACK DlgProc(HWND dhwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
			{		
				return TRUE;
			}
			break;
		
		case WM_SHOWWINDOW:
			{
				ShowWindow ( dhwnd, (int)wparam );
			}
			break;

		case WM_CLOSE:
			{
				EndDialog(dhwnd, 0);
			}
			break;
	}

    return FALSE;
}

LRESULT CCustomUI::SetWindowSize(int ox, int oy, int cx, int cy)
{

	SetWindowPos( hwnd,NULL,ox,oy,cx,cy,SWP_NOZORDER); 

	return S_OK;
}