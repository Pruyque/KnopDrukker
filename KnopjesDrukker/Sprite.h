#pragma once
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <gdiplus.h>

using namespace Gdiplus;

#define M_PIf 3.141592654f

struct Sprite
	/* Dit wordt is een sprite.
	De dingen die we gaan tekenen zijn allemaal sprites (net als bij Scratch)
	Hierin doen we alle dingen die sprites moeten kunnen.
	*/
{
	// Dit is de naam van het schermpje waar we in tekenen
	HWND hWnd;
	static LRESULT CALLBACK wndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static void Voorbereiding();

	/*
	sx en sy zijn de positie van de sprite
	*/
	float sx, sy;
	float hoek = 0;
	/*
	kx en ky zijn het punt waar de sprite naar moet kijken
	*/
	void WindowsBerichtje(UINT Msg, WPARAM wParam, LPARAM lParam);
	Sprite();
	~Sprite();
	void BeweegSprite();
	void NieuwPlaatje();
	void KijkNaar(float x, float y);

	void StapVooruit(float stapgrootte);
	float AfstandTotSprite(Sprite *andere);

	/*
	De volgende functies, met = 0 erachter betekenen dat alle sprites
	dit moeten kunnen, maar verschillende soorten sprites (zombies, skeletten etc)
	doen dit allemaal anders
	*/
	virtual bool DenkNa() = 0;
	/*
	Laat hier je Sprite nadenken en bewegen.
	Geef 'true' terug als deze sprite moet blijven bestaan.
	Als je 'false' terug geeft, wordt-ie weggegooid
	*/
	virtual void TekenDeSprite(Graphics *PenVoorVelletje) = 0;
	/*
	We maken zelf nog een plaatje aan, daar gaan we dan op tekenen.
	Als we klaar zijn met tekenen, kopieeren we hem naar het velletje
	dat we van Windows hebben gekregen.
	Als je rechtstreeks op het velletje van Windows tekent, teken je
	op het beeldscherm en zie je dingen knipperen...
	*/
};

float AfstandTussenSprites(Sprite *sprite1, Sprite *sprite2);
