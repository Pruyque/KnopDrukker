// KnopjesDrukker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <math.h>
#include <gdiplus.h>

using namespace Gdiplus;

void WaarIsDeMuis(int &x, int &y)
/*
	Hiermee kunnen we vragen waar de muis is.
*/
{
	POINT p;
	GetCursorPos(&p);
	x = p.x;
	y = p.y;
}


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
	float sx, sy;
	float tijd = 0;
	virtual void TekenDeSprite(Graphics *PenVoorVelletje)
	{
		/*
			We maken zelf nog een plaatje aan, daar gaan we dan op tekenen.
			Als we klaar zijn met tekenen, kopieeren we hem naar het velletje
			dat we van Windows hebben gekregen.
			Als je rechtstreeks op het velletje van Windows tekent, teken je
			op het beeldscherm en zie je dingen knipperen...
		*/
		Bitmap plaatje(64, 64);
		Graphics *PenVoorPlaatje = Graphics::FromImage(&plaatje);


		/*
			Dan gaan we hier lekker tekenen
		*/

		/*
			Eerst zorgen we er voor dat het plaatje zo gedraaid gaat worden
			dat we naar de muis kijken
		*/
		PenVoorPlaatje->TranslateTransform(32, 32);
		int mx, my;
		WaarIsDeMuis(mx, my);
		float Hoek = atan2(my - sy, mx - sx);
		PenVoorPlaatje->RotateTransform(180 * Hoek / 3.141592653);
		PenVoorPlaatje->TranslateTransform(-32, -32);

		/*
			We doen er een beetje tijd bij, daarmee laten we de voetjes bewegen
		*/
		tijd += 0.1;

		/*
			Maak alles zwart (doorzichtig)
		*/
		PenVoorPlaatje->Clear(Color::Black);
		// Voetjes
		PenVoorPlaatje->FillRectangle(&SolidBrush(Color::Blue), 32 - 5 + 5 * sin(tijd), 32 - 10, 10, 10);
		PenVoorPlaatje->FillRectangle(&SolidBrush(Color::Blue), 32 - 5 - 5 * sin(tijd), 32, 10, 10);
		// Lichaam
		PenVoorPlaatje->FillRectangle(&SolidBrush(Color::Green), 32 - 5, 32 - 10, 10, 20);
		// Armen
		PenVoorPlaatje->FillRectangle(&SolidBrush(Color::Green), 32 - 5, 32 - 20, 30, 10);
		PenVoorPlaatje->FillRectangle(&SolidBrush(Color::Green), 32 - 5, 32 + 10, 30, 10);
		// Hoofd
		PenVoorPlaatje->FillRectangle(&SolidBrush(Color::Brown), 32 - 5, 32 - 7.5, 15, 15);

		/*
			Hier kopieren we het plaatje naar het velletje
		*/
		PenVoorVelletje->DrawImage(&plaatje, 0, 0);
		delete PenVoorPlaatje;

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
			TekenDeSprite(PenVoorVelletje); // Teken de sprite met de pen voor het velletje
			/*
				Vertel Windows dat we klaar zijn
			*/
			EndPaint(hWnd, &paint);
			
			/*
				En ruim de pennen die we gemaakt hebben op
			*/
			delete PenVoorVelletje;

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
			WS_EX_LAYERED|WS_EX_TOPMOST|WS_EX_TOOLWINDOW, L"DoorzichtigSchermKlasse",
			L"DoorzichtigScherm",
			WS_VISIBLE|WS_POPUP,
			0, 0, 64, 64, nullptr, nullptr, nullptr, this);

		/*
			We hebben een doorzichtig scherm gevraagd,
			nu vertellen we dat de zwarte (RGB(0,0,0)) pixels
			doorzichtig moeten zijn
		*/
		SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
	}
	~Sprite()
		/* Dit gaan we doen als we een sprite weggooien */
	{
		/* Als de sprite weggegooid word, kan het schermpje ook weg */
		DestroyWindow(hWnd);

		/* Opruimen na de klus is net zo belangrijk als de klus zelf... */
	}
	void BeweegSprite()
	/*
		Hiermee kunnen we de Sprite verplaatsen
	*/
	{
		MoveWindow(hWnd, sx - 32, sy - 32, 64, 64, TRUE);
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
	virtual bool DenkNa()
	/*
		Laat hier je Sprite nadenken en bewegen.
		Geef 'true' terug als deze sprite moet blijven bestaan.
		Als je 'false' terug geeft, wordt-ie weggegooid
	*/
	{
		int mx, my;
		/* Vraag waar de muis is, dit wordt ingevuld in mx en my */
		WaarIsDeMuis(mx, my);
		
		/* bepaal welke kant we op moeten kijken */
		float dx = mx - sx;
		float dy = my - sy;
		float n = sqrt(dx * dx + dy * dy);

		if (n < 32)
			return false;

		dx /= n;
		dy /= n;

		/* Doe een stapje in de richting van de muis */
		sx += dx;
		sy += dy;
		/* Zet de Sprite op de plek die we net hebben bepaald */
		BeweegSprite();
		/* Teken de Sprite opnieuw, dat is nu niet nodig, maar dan kunnen we het even uitproberen */
		NieuwPlaatje();

		return true;
	}
};

float AfstandTussenSprites(Sprite *sprite1, Sprite *sprite2)
{
	return sqrtf(pow(sprite1->sx - sprite2->sx, 2) + pow(sprite1->sy - sprite2->sy, 2));
}

#include <math.h>
#include <vector>

int main()
{
	printf("Sluit dit scherm om af te sluiten\n");
	/*
		Doe de voorbereidingen die nodig zijn...
	*/
	Sprite::Voorbereiding();

	/*
		Maak een sprite aan
	*/
	std::vector<Sprite*> sprites;
	for (int cx = 0; cx < 10; cx++)
	{
		Sprite *nieuwe_sprite = new Sprite;
		nieuwe_sprite->sx = rand() & 1023;
		nieuwe_sprite->sy = rand() & 1023;
		sprites.push_back(nieuwe_sprite);
	}

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

		for (std::vector<Sprite *>::iterator sprite = sprites.begin(); sprite != sprites.end(); sprite ++)
			/* Doe voor alle sprites */
		{
			if (!(*sprite)->DenkNa())
			{
				delete *sprite;
				sprite = sprites.erase(sprite);
				if (sprite == sprites.end())
					break;
			}
		}

		/* Als de sprites te dicht bij elkaar komen, moeten ze elkaar weg duwen */
		for (Sprite *sprite1: sprites)
			for (Sprite *sprite2: sprites)
				if (sprite1 != sprite2)
				{
					float Afstand = AfstandTussenSprites(sprite1, sprite2);
					if (Afstand < 64)
					{
						sprite2->sx = sprite1->sx + 64 * (sprite2->sx - sprite1->sx) / Afstand;
						sprite2->sy = sprite1->sy + 64 * (sprite2->sy - sprite1->sy) / Afstand;
					}
				}
		Sleep(50);
		if (sprites.size() == 0)
			/* Als er geen sprites meer over zijn
				gaan we uit het loopje
				*/
			break;
	}
	/*
		Aan het eind van ons programma, moeten we netjes onze zooi opruimen
	*/
	for (Sprite *sprite : sprites)
		delete sprite;

	/*
		Einde van het programma
	*/
    return 0;
}

