// KnopjesDrukker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

#include <gdiplus.h>

using namespace Gdiplus;

#pragma comment(lib, "gdiplus")
struct Sprite
	/* Dit wordt onze sprite
	*/
{
	// Dit is de naam van het schermpje waar we in tekenen
	HWND hWnd;
	static LRESULT CALLBACK wndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		/* 
			Hier komen de berichtjes binnen die Windows ons stuurt
		*/
		if (msg == WM_CREATE)
		{
			/*	
				Dit bericht krijgen we als ons schermpje wordt gemaakt.
				Nu moeten we ons schermpje verbinden met onze sprite
			*/
			CREATESTRUCT *s = (CREATESTRUCT *)lParam;

			SetWindowLongPtr(wnd, GWLP_USERDATA, (LONG_PTR)s->lpCreateParams);
		}
		if (msg == WM_ERASEBKGND)
		{
			/*
				We willen de achtergrond niet leeg maken, dat gaat knipperen...
			*/
			return FALSE;
		}

		/* 
			Kijk welke sprite bij dit schermpje hoort
		*/
		Sprite *sprite = (Sprite *)GetWindowLongPtr(wnd, GWLP_USERDATA);
		if (sprite)
			/* 
				Als je hebben kunnen bepalen welke sprite er bij het berichtje hoort,
				sturen we hem door
			*/
			sprite->WindowsBerichtje(msg, wParam, lParam);
		

		/*
			Stuur het berichtje door naar de volgende die het wil horen...
		*/
		return CallWindowProc(DefWindowProc, wnd, msg, wParam, lParam);
	}
	static void Voorbereiding()
	/*
		Hier doen we wat voorbereidingen	
	*/
	{

		/*
			Vertel windows dat je een schermpje willen gaan maken
		*/
		WNDCLASS cls = { 0 };
		cls.lpfnWndProc = wndProc;
		cls.lpszClassName = L"DoorzichtigSchermKlasse";
		RegisterClass(&cls);

		/*
			Dit moet je doen voor je bepaalde tekenfuncties kan gebruiken
		*/
		ULONG_PTR token;
		GdiplusStartup(&token, &GdiplusStartupInput(), &GdiplusStartupOutput());

	}
	void WindowsBerichtje(UINT Msg, WPARAM wParam, LPARAM lParam)
	/*
		Hier komen de berichtjes van Windows voor deze sprite binnen
	*/
	{
		// Kijk welk bericht dit is
		switch (Msg)
		{
		case WM_PAINT:
			/*
				Dit is een opdracht om onze sprite opnieuw te tekenen
			*/
			PAINTSTRUCT paint;
			
			/*
				Vertel Windows dat we gaan tekenen, Windows geeft ons
				een velletje (HDC) om op te tekenen
			*/
			HDC velletje = BeginPaint(hWnd, &paint);
			Graphics *PenVoorVelletje = Graphics::FromHDC(velletje);

			/*
				We maken zelf nog een plaatje aan, daar gaan we dan op tekenen.
				Als we klaar zijn met tekenen, kopieeren we hem naar het velletje
				dat we van Windows hebben gekregen.
				Als je rechtstreeks op het velletje van Windows tekent, teken je
				op het beeldscherm en zie je dingen knipperen...
			*/
			Bitmap plaatje (256, 256);
			Graphics *PenVoorPlaatje = Graphics::FromImage(&plaatje);
			

			/*
				Dan gaan we hier lekker tekenen
			*/
			PenVoorPlaatje->Clear(Color::Black);
			PenVoorPlaatje->FillEllipse(&SolidBrush(Color::White), 10, 10, 236, 236);
			PenVoorPlaatje->FillEllipse(&SolidBrush(Color::Black), 30, 30, 196, 196);

			/*
				Hier kopieren we het plaatje naar het velletje
			*/
			PenVoorVelletje->DrawImage(&plaatje, 0, 0);

			/*
				Vertel Windows dat we klaar zijn
			*/
			EndPaint(hWnd, &paint);
			
			/*
				En ruim de pennen die we gemaakt hebben op
			*/
			delete PenVoorVelletje;
			delete PenVoorPlaatje;
			break;
		}
	}
	Sprite()
	/*
		Dit wordt gedaan wanneer we een nieuwe Sprite maken
	*/
	{
		/*
			Zeg tegen Windows dat we een nieuw scherm willen en
			sla de naam op in hWnd
		*/
		hWnd = CreateWindowEx(
			WS_EX_LAYERED, L"DoorzichtigSchermKlasse",
			L"DoorzichtigScherm",
			WS_VISIBLE | WS_POPUP,
			0, 0, 256, 256, nullptr, nullptr, nullptr, this);

		/*
			We hebben een doorzichtig scherm gevraagd,
			nu vertellen we dat de zwarte (RGB(0,0,0)) pixels
			doorzichtig moeten zijn
		*/
		SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
	}
	void BeweegScherm(int x, int y)
	/*
		Hiermee kunnen we de Sprite verplaatsen
	*/
	{
		MoveWindow(hWnd, x, y, 256, 256, TRUE);
	}
	void NieuwPlaatje()
	/*
		Hiermee geven we aan dat de Sprite opnieuw moet worden getekend.
		Windows probeert anders zo lang mogelijk het laatst getekende plaatje te
		gebruiken
	*/
	{
		InvalidateRect(hWnd, nullptr, FALSE);
	}
	static void WaarIsDeMuis(int &x, int &y)
	/*
		Hiermee kunnen we vragen waar de muis is.
	*/
	{
		POINT p;
		GetCursorPos(&p);
		x = p.x;
		y = p.y;
	}
};
#include <math.h>

int main()
{
	/*
		Doe de voorbereidingen die nodig zijn...
	*/
	Sprite::Voorbereiding();

	/*
		Maak een sprite aan
	*/
	Sprite scherm;

	/* In x en y houden we de positie van de sprite bij */
	int x = -256, y = 0;
	while (1)
	{
		/*
			Dit is de berichten pomp.
			Dit zorgt er voor dat berichten van Windows
			naar de schermen wordt gestuurd.
		*/
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}


		int mx, my;
		/* Vraag waar de muis is, dit wordt ingevuld in mx en my */
		Sprite::WaarIsDeMuis(mx, my);

		/* Als de muis mx hoger is dan x, verhoog dan x, anders verlagen we x */
		if (mx > x + 128) x++; else x--;
		/* En we doen het zelfde voor y */
		if (my > y + 128) y++; else y--;
		/* Zet de Sprite op de plek die we net hebben bepaald */
		scherm.BeweegScherm(x, y);
		/* Wacht even */
		Sleep(10);
		/* Teken de Sprite opnieuw, dat is nu niet nodig, maar dan kunnen we het even uitproberen */
		scherm.NieuwPlaatje();
	}
    return 0;
}

