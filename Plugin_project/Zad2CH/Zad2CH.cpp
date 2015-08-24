#include "Zad2CH.h"
#include "resource.h"
#include <string>
#include <xsi_viewcontext.h>
#include <xsi_application.h>
#include <xsi_project.h>
#include <xsi_property.h>
#include <xsi_scene.h>

using namespace XSI;

extern HINSTANCE g_Instance;
Application app;
COMClass com;

LRESULT CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
////*****************************************************************************
////**** implementacja metod CCustomUI

//// konst/dekonstr
CCustomUI::CCustomUI(){}
CCustomUI::~CCustomUI(){}

//void CCustomUI::manipulateKey (Action acParam)
//{
//	CValue outArg;
//	CValueArray inArgs;
//	inArgs.Add( 0 );
//
//	switch(acParam)
//	{
//		case NEXTKEY:
//		{
//			app.ExecuteCommand(L"NextKey", inArgs, outArg);
//		}
//		case PREVKEY:
//		{
//			app.ExecuteCommand(L"PrevKey", inArgs, outArg);
//		}
//	}
//}
//// Init - utworzenie dialogu z uchwytem zdefiniowanym przez view context (czyli przez argument wywolujacy!)
LRESULT CCustomUI::Init( CRef& in_pViewCtx )
{
	ViewContext myViewContext = in_pViewCtx;

	hwnd = CreateDialog( g_Instance, MAKEINTRESOURCE(IDD_MAIN), (HWND)myViewContext.GetParentWindowHandle(), (DLGPROC)DlgProc);
	return S_OK;
}

//// Term - zniszczenie dialogu (ewen. obsluzenie zaalokowanej pamieci)
LRESULT CCustomUI::Term( CRef& in_pViewCtx )
{
	com.Term(); // to free memory space from event/com handles!
	DestroyWindow(hwnd);
	hwnd = NULL;

	return S_OK;
}

//// Notify - obsluga zdarzen, eventy w XSI -> dialog
/*LRESULT CCustomUI::Notify( CRef& in_pViewCtx )
{
	return S_OK;
}*/

////**** koniec implementacji metod 
////*****************************************************************************
////**** obsluga zdarzen wewnatrz dialogu

LRESULT CALLBACK DlgProc(HWND dhwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
			{		
				HWND pom;
				pom = GetDlgItem(dhwnd, IDC_Y_EDIT);
				com.addControl( pom );
				pom = GetDlgItem(dhwnd, IDC_X_EDIT);
				com.addControl( pom );
				pom = GetDlgItem(dhwnd, IDC_Z_EDIT);
				com.addControl( pom );
				pom = GetDlgItem(dhwnd, IDC_EDITTEST);
				com.addControl( pom );
				com.Init("COM1", 9600, 0, 1, 8);
				//Application app;

				//**************** Checking/changing frames
				/*Project prj = app.GetActiveProject();
				CRefArray arProp = prj.GetProperties();
				Property playctrl(arProp.GetItem("Play Control"));
				Parameter curr_frame = playctrl.GetParameter("Current");
				curr_frame.PutValue(50);*/

				/*CValue outArg;
				CValueArray inArgs;
				inArgs.Add( 0 );
				app.ExecuteCommand(L"NextFrame", inArgs, outArg);*/

				//**************** Making new scene
				//CValue outArg;
				//CValueArray inArgs; 
				//inArgs.Add( CValue() );  // ProjectPathName 
				//inArgs.Add( true ); // Confirm 
				//app.ExecuteCommand( L"NewScene", inArgs, outArg );
			}
			break;
		
		case WM_SHOWWINDOW:
			{
				ShowWindow (dhwnd, (int)wparam);
			}
			break;

		case WM_CLOSE:
			{
				EndDialog(dhwnd, 0);
			}
			break;

		case WM_COMMAND:
			{
				CValue outArg;
				CValueArray inArgs;

				switch(LOWORD(wparam))
				{
					case IDC_B_NSCENE:
						{
							inArgs.Add( CValue() );  // ProjectPathName 
							inArgs.Add( true ); // Confirm 
							app.ExecuteCommand( L"NewScene", inArgs, outArg );
						}
						break;

					case IDC_B_NEXT:
						{
							inArgs.Add( CValue() );
							app.ExecuteCommand(L"NextKey", inArgs, outArg);
						}
						break;

					case IDC_B_PREV:
						{
							inArgs.Add( CValue() );
							app.ExecuteCommand(L"PrevKey", inArgs, outArg);
						}
						break;

					case IDC_B_RMV:
						{
							try
							{
								inArgs.Add( CValue() );
								app.ExecuteCommand(L"RemoveKey", inArgs, outArg);
							}
							catch( siErrorValueEnum )
							{
								app.LogMessage("Key frame cannot be removed");
							}
						}

					case IDC_X_P:
						{					
							
						}
				}
			}
			break;

		default:
			return FALSE;
		break;
	}

    return TRUE;
}