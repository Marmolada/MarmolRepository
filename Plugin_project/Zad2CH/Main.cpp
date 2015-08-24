// Zad2CH
// Initial code generated by Softimage SDK Wizard
// Executed Sat Jul 11 20:24:50 UTC+0200 2015 by Andrzej
// 
// Tip: You need to compile the generated code before you can load the plug-in.
// After you compile the plug-in, you can load it by clicking Update All in the Plugin Manager.
#include "Zad2CH.h"
#include "resource.h"
#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_pluginregistrar.h>
#include <xsi_status.h>
#include <xsi_viewcontext.h>
#include <xsi_viewnotification.h>
#include <xsi_status.h>

using namespace XSI; 

HINSTANCE g_Instance = NULL;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
	g_Instance = (HINSTANCE)hModule;

    return TRUE;
}
///////////////////////////////////////////////////////////////////////
//////// fukcje ktore wywolywac bedzie juz sobie XSI dla zarejestrowanego obiektu CustomDisplay

XSIPLUGINCALLBACK void MyCustomView_Init( CRef& in_pViewCtx  )
{
	// czy CRef przekaza�o obiekt View Context
	assert ( in_pViewCtx.IsA (siViewContextID ) );
	ViewContext myViewContext = in_pViewCtx;
	assert ( myViewContext.IsValid() );

	// wskaznij na nowy obiekt CCustomUI
	// dzieki temu bedziemy mogli wywolywac metody Init, Term i Notify dla klasy!
	CCustomUI* myCCustomUI = new CCustomUI();

	myViewContext.PutUserData( (void*)myCCustomUI );
	myViewContext.SetFlags( XSI::siWindowNotifications );
	// w ciele tej funkcji wywolujemy Init z argumentem przekazanych dalej
	myCCustomUI->Init( in_pViewCtx );
}

XSIPLUGINCALLBACK void MyCustomView_Term ( CRef& in_pViewCtx )
{
	// 1) przy terminacji okna zweryfikowac musimy czy aby na pewno istnieje wywolany obiekt
	// 2) jezeli istnieje to wywolujemy na nim metode terminacji
	// 3) i zwalniamy pamiec ktora zajmowal
	ViewContext myViewContext = in_pViewCtx;
	assert ( myViewContext.IsValid() );
	CCustomUI* myCCustomUI = (CCustomUI*)( (void*)myViewContext.GetUserData ());
	// 1)
	assert( myCCustomUI != NULL );
	// 2)
	myCCustomUI->Term( in_pViewCtx );
	// 3)
	delete myCCustomUI;
	myCCustomUI = NULL;
}

//////////////////////////////////////////////////////////////
/////// konfiguracja self-installing | rejestracja CustomDisplay

XSIPLUGINCALLBACK CStatus XSILoadPlugin( PluginRegistrar& in_reg )
{
	in_reg.PutAuthor(L"Marmol");
	in_reg.PutName(L"Zad2CH");
	in_reg.PutVersion(1,0);
	//RegistrationInsertionPoint - do not remove this line

	in_reg.RegisterCustomDisplay(L"MyCustomView");

	return CStatus::OK;
}

XSIPLUGINCALLBACK CStatus XSIUnloadPlugin( const PluginRegistrar& in_reg )
{
	return CStatus::OK;
}


