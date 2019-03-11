#include "Sprite.h"

LRESULT Sprite::wndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

void Sprite::Voorbereiding()
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

void Sprite::WindowsBerichtje(UINT Msg, WPARAM wParam, LPARAM lParam)
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
		Bitmap plaatje(64, 64);
		Graphics *PenVoorPlaatje = Graphics::FromImage(&plaatje);

		/*
		Eerst zorgen we er voor dat het plaatje zo gedraaid gaat worden
		dat we naar de muis kijken
		*/
		PenVoorPlaatje->TranslateTransform(32, 32);
		PenVoorPlaatje->RotateTransform(hoek);
		PenVoorPlaatje->TranslateTransform(-32, -32);


		TekenDeSprite(PenVoorPlaatje); // Teken de sprite met de pen voor het velletje

									   /*
									   Hier kopieren we het plaatje naar het velletje
									   */
		PenVoorVelletje->DrawImage(&plaatje, 0, 0);
		delete PenVoorPlaatje;

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

Sprite::Sprite()
/*
Dit wordt gedaan wanneer we een nieuwe Sprite maken
*/
{
	/*
	Zeg tegen Windows dat we een nieuw scherm willen en
	sla de naam op in hWnd
	*/
	hWnd = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, L"DoorzichtigSchermKlasse",
		L"DoorzichtigScherm",
		WS_VISIBLE | WS_POPUP,
		0, 0, 64, 64, nullptr, nullptr, nullptr, this);

	/*
	We hebben een doorzichtig scherm gevraagd,
	nu vertellen we dat de zwarte (RGB(0,0,0)) pixels
	doorzichtig moeten zijn
	*/
	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
}

Sprite::~Sprite()
/* Dit gaan we doen als we een sprite weggooien */
{
	/* Als de sprite weggegooid word, kan het schermpje ook weg */
	DestroyWindow(hWnd);

	/* Opruimen na de klus is net zo belangrijk als de klus zelf... */
}

void Sprite::BeweegSprite()
/*
Hiermee kunnen we de Sprite verplaatsen
*/
{
	MoveWindow(hWnd, (int)(sx - 32), (int)(sy - 32), 64, 64, TRUE);
}

void Sprite::NieuwPlaatje()
/*
Hiermee geven we aan dat de Sprite opnieuw moet worden getekend.
Windows probeert anders zo lang mogelijk het laatst getekende plaatje te
gebruiken
*/
{
	InvalidateRect(hWnd, nullptr, FALSE);
}

void Sprite::KijkNaar(float x, float y)
/*
Draai de sprite zodat hij naar dit punt kijkt
*/
{
	hoek = atan2f(y - sy, x - sx) * 180 / M_PIf;
}

void Sprite::StapVooruit(float stapgrootte)
/*
Doe een stap in de richting waarin de sprite kijkt
*/
{
	float dx = cosf(M_PIf * hoek / 180.f);
	float dy = sinf(M_PIf * hoek / 180.f);

	sx += stapgrootte * dx;
	sy += stapgrootte * dy;
}

float AfstandTussenSprites(Sprite * sprite1, Sprite * sprite2)
{
	return sqrtf(powf(sprite1->sx - sprite2->sx, 2) + powf(sprite1->sy - sprite2->sy, 2));
}

float Sprite::AfstandTotSprite(Sprite * andere)
{
	return AfstandTussenSprites(andere, this);
}
