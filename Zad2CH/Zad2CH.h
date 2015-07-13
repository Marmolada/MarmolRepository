#include <xsi_ref.h>
#include <windows.h>

class CCustomUI
{
public:
	CCustomUI();
	virtual ~CCustomUI();
	// deklaracja funkcji udostepnianych przez
	// XSI dla custom view, init i term wymagane, notify opcjonalne
	virtual LRESULT Init( XSI::CRef& in_pViewCtx ); // otworzenie okna
	virtual LRESULT	Term( XSI::CRef& in_pViewCtx ); // zamkniecie okna
	//virtual LRESULT Notify ( XSI::CRef& in_pViewCtx ); // zdarzenia w XSI -> nasz custom view

	LRESULT SetWindowSize(int ox, int oy, int cx, int cy);
private:
	HWND hwnd; // uchwyt ktory zwroci nam xsi context - patrz Init
};