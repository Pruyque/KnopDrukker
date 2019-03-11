// KnopjesDrukker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <gdiplus.h>
#include <vector>

#include "Sprite.h"

#define M_PIf (float)M_PI

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

// Om bepaalde teken dingen te doen moet dit hier staan:
#pragma comment(lib, "gdiplus")


/*
	Dit is een lijst waarin we de gemaakte sprites bijhouden
*/
std::vector<Sprite*> sprites;


struct Zombie : public Sprite
{
	/* 
		We houden een tijd bij voor een zombie, zodat we de voetjes kunnen laten bewegen
	*/
	float tijd = 0;
	virtual void TekenDeSprite(Graphics *PenVoorPlaatje) 
	{
		/*
			Dan gaan we hier lekker tekenen
		*/

		/*
			We doen er een beetje tijd bij, daarmee laten we de voetjes bewegen
		*/
		tijd += 0.1f;

		/*
			Maak alles zwart (doorzichtig)
		*/
		PenVoorPlaatje->Clear(Color::Black);
		// Voetjes
		PenVoorPlaatje->FillRectangle(&SolidBrush(Color::Blue), 32 - 5 + (int)(5 * sin(tijd)), 32 - 10, 10, 10);
		PenVoorPlaatje->FillRectangle(&SolidBrush(Color::Blue), 32 - 5 - (int)(5 * sin(tijd)), 32, 10, 10);
		// Lichaam
		PenVoorPlaatje->FillRectangle(&SolidBrush(Color::Green), 32 - 5, 32 - 10, 10, 20);
		// Armen
		PenVoorPlaatje->FillRectangle(&SolidBrush(Color::Green), 32 - 5, 32 - 20, 30, 10);
		PenVoorPlaatje->FillRectangle(&SolidBrush(Color::Green), 32 - 5, 32 + 10, 30, 10);
		// Hoofd
		PenVoorPlaatje->FillRectangle(&SolidBrush(Color::Brown), 32 - 5, 32 - 7, 15, 15);


	}
	virtual bool DenkNa()
	{
		int mx, my;
		/* Vraag waar de muis is, dit wordt ingevuld in mx en my */
		WaarIsDeMuis(mx, my);

		/* laat de zombie naar de muis kijken */
		KijkNaar((float)mx, (float)my);

		/* bepaal hoe ver we van de muis zijn */
		float dx = mx - sx;
		float dy = my - sy;
		float n = sqrt(dx * dx + dy * dy);

		/* als de zombie dicht bij de muis komt, wordt hij weggegooid */
		if (n < 32)
			return false;

		/* Doe een stapje in de richting van de muis */
		StapVooruit(1);
		/* Zet de Sprite op de plek die we net hebben bepaald */
		BeweegSprite();
		/* Teken de Sprite opnieuw, dat is nu niet nodig, maar dan kunnen we het even uitproberen */
		NieuwPlaatje();

		return true;
	}

};


struct Creeper : public Sprite
{
	/*
		We houden een tijd bij voor een zombie, zodat we de voetjes kunnen laten bewegen
	*/
	float tijd = 0;
	virtual void TekenDeSprite(Graphics *PenVoorPlaatje)
	{
		/*
			Dan gaan we hier lekker tekenen
		*/

		/*
			We doen er een beetje tijd bij, daarmee laten we de voetjes bewegen
		*/
		tijd += 0.3f;
		
		/*
			Maak alles zwart (doorzichtig)
		*/
		PenVoorPlaatje->Clear(Color::Black);
		// Voetjes
		PenVoorPlaatje->FillRectangle(&SolidBrush(Color::Green), 32 - 20 + (int)(4 * sin(tijd)), 32 - 20, 10, 10);
		PenVoorPlaatje->FillRectangle(&SolidBrush(Color::Green), 32 - 20 + (int)(4 * cos(tijd)), 32 + 10, 10, 10);
		PenVoorPlaatje->FillRectangle(&SolidBrush(Color::Green), 32 + 10 + (int)(4 * cos(tijd)), 32 - 20 , 10, 10);
		PenVoorPlaatje->FillRectangle(&SolidBrush(Color::Green), 32 + 10 - (int)(4 * sin(tijd)), 32 + 10, 10, 10);
		// Lichaam
		PenVoorPlaatje->FillRectangle(&SolidBrush(Color::Green), 32 - 10, 32 - 10, 20, 20);
	}
	virtual bool DenkNa()
	{
		int mx, my;
		/* Vraag waar de muis is, dit wordt ingevuld in mx en my */
		WaarIsDeMuis(mx, my);

		/* laat de creeper rondjes om de muis lopen */
		KijkNaar((float)mx, (float)my);
		hoek += 90.f;

		/* bepaal hoe ver we van de muis zijn */
		float dx = mx - sx;
		float dy = my - sy;
		float n = sqrt(dx * dx + dy * dy);

		/* als de zombie dicht bij de muis komt, wordt hij weggegooid */
		if (n < 32)
			return false;

		dx /= n;
		dy /= n;

		/* Doe een stapje in de richting van de muis */
		StapVooruit(3.f);
		/* Zet de Sprite op de plek die we net hebben bepaald */
		BeweegSprite();
		/* Teken de Sprite opnieuw, dat is nu niet nodig, maar dan kunnen we het even uitproberen */
		NieuwPlaatje();

		return true;
	}

};


int main()
{
	printf("Sluit dit scherm om af te sluiten\n");
	/*
		Doe de voorbereidingen die nodig zijn...
	*/
	Sprite::Voorbereiding();

	/*
		Maak sprites aan
	*/
	for (int cx = 0; cx < 10; cx++)
	{
		Sprite *nieuwe_sprite = new Creeper;
		nieuwe_sprite->sx = rand() % 1023;
		nieuwe_sprite->sy = rand() % 1023;
		sprites.push_back(nieuwe_sprite);
	}
	for (int cx = 0; cx < 10; cx++)
	{
		Sprite *nieuwe_sprite = new Zombie;
		nieuwe_sprite->sx = rand() % 1023;
		nieuwe_sprite->sy = rand() % 1023;
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


		/*
			We maken een iterator, dit is een soort vinger die in de lijst van sprites bijhoudt waar we zijn
		*/
		for (std::vector<Sprite *>::iterator iter = sprites.begin() /* laat de vinger naar het begin van de lijst wijzen */;
			iter != sprites.end() /* stop als de vinger naar het eind van de lijst wijst*/;
			iter ++ /* Wijs naar de volgende sprite */)
		/* Doe voor alle sprites */
		{
			// Welke sprite wijst de vinger naar?
			Sprite *sprite = *iter;

			// Laat de sprite nadeken
			if (!sprite->DenkNa())
			{
				// Haal deze sprite uit de lijst en laat de vinger naar de volgende sprite wijzen
				iter = sprites.erase(iter);

				// Deze sprite moet worden weggegooid
				delete sprite;

				// Als de vinger naar het einde van de lijst wijst moeten we stoppen
				if (iter == sprites.end())
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
		
		// Hier slapen we even, zodat de computer ondertussen wat anders kan doen, en de sprites niet te stel gaan
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

