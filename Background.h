#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "specifications.h"

// Background types
const int MOUNTAINS = 2;
const int SPACE 	= 3;
const int INFERNO 	= 4;
const int ALIEN 	= 5;
const int ICE		= 6;
const int WATER 	= 7;

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
class Background {
public:
	// Constructor / Destructor
	Background();
	~Background();
	
	// Utility Functions
	SDL_Rect SetRect(int x, int y, int w, int h);
	
	// IO Functions
	void Update(void);
	void Draw(SDL_Surface *lpSScreen);
	
	// Setting functions
	void SetType(int backgroundType);
	void SetScrollSpeed(int nScrollSpeed);
	
	// Action functions
	void Stop(void);
	
	// Error flags
	bool initOkay;
	bool drawOkay;
	bool setOkay;
	
private:
	// Graphic variables
	SDL_Surface *lpSBackground;
	SDL_Rect bgSrc1;
	SDL_Rect bgSrc2;
	SDL_Rect bgDest1;
	SDL_Rect bgDest2;
	
	// Speed variable (negative = stopped)
	int scrollSpeed;
	
	// Time variables
	unsigned int scrollTicks;
	
};

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
Background::Background()
{	
	// Default initOkay (if something goes wrong, this goes false)
	initOkay = true;
	
	// Load default background image
	SDL_Surface *temp = SDL_LoadBMP("bg_mountains.bmp");
	if (temp == NULL) {
		printf("bg_mountains.bmp load failed.\n");
		initOkay = false;
	}
	lpSBackground = SDL_DisplayFormat(temp);
	if (lpSBackground == NULL) {
		printf("lpSBackground conversion failed.\n");
		initOkay = false;
	}
	SDL_FreeSurface(temp);
			
	// Set up scrolling rectangles
	bgSrc1 = SetRect(0, 0, lpSBackground->w, lpSBackground->h);
	bgDest1 = SetRect(SCREEN_X/2 - lpSBackground->w/2, 0, bgSrc1.w, bgSrc1.h);

	bgSrc2 = SetRect(0, 0, lpSBackground->w, 1);
	bgDest2 = SetRect(SCREEN_X/2 - lpSBackground->w/2, 0, lpSBackground->w, 1);
	
	// Initizlize scrolling speed
	scrollSpeed = 200;
	
	// Initialise scrolling ticks
	scrollTicks = SDL_GetTicks();
}


// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
Background::~Background()
{
	// Release graphic
	SDL_FreeSurface(lpSBackground);	
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
SDL_Rect Background::SetRect(int x, int y, int w, int h)
{
	SDL_Rect temp;
	temp.x = x;
	temp.y = y;
	temp.w = w;
	temp.h = h;
	
	return temp;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void Background::Update(void)
{
	// Stop updating is scrolling is disabled
	if (scrollSpeed < 0) {
		return;
	}
	
	if (SDL_GetTicks() - scrollTicks > (unsigned int)scrollSpeed) {
		// Scroll background
		bgDest1.y += 1;
			
		// If bgDest1 goes over 480 edge, draw bgDest2 in its place
		if (bgDest1.y + bgDest1.h > SCREEN_Y) {
			bgDest2.y = 0;
			bgDest2.h = bgDest1.y;
			
			bgSrc2.y = SCREEN_Y - bgDest1.y;
			bgSrc2.h = bgDest2.h;
		}
		
		// If bgDest1 goes completely offscreen, draw it back at top
		if (bgDest1.y > SCREEN_Y) {
			bgDest1.y = 0;
			bgDest1.h = SCREEN_Y;
		}
	}

	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void Background::Draw(SDL_Surface *lpSScreen)
{
	// Draw both blocks
	SDL_BlitSurface(lpSBackground, &bgSrc1, lpSScreen, &bgDest1);
	SDL_BlitSurface(lpSBackground, &bgSrc2, lpSScreen, &bgDest2);
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void Background::SetType(int backgroundType)
{
	char *filename;
	
	switch (backgroundType) {
	
	case MOUNTAINS:
		filename = "bg_mountains.bmp";
		break;
	
	case SPACE:
		filename = "bg_space.bmp";
		break;
	
	case INFERNO:
		filename = "bg_inferno.bmp";
		break;
	
	case ALIEN:
		filename = "bg_alien.bmp";
		break;
	
	case ICE:
		filename = "bg_ice.bmp";
		break;
	
	case WATER:
		filename = "bg_water.bmp";
		break;
	}
	
	// Unload old background
	SDL_FreeSurface(lpSBackground);
	
	// Load default background image
	SDL_Surface *temp = SDL_LoadBMP(filename);
	if (temp == NULL) {
		printf("%s load failed.\n", filename);
		setOkay = false;
	}
	lpSBackground = SDL_DisplayFormat(temp);
	if (lpSBackground == NULL) {
		printf("lpSBackground conversion failed.\n");
		setOkay = false;
	}
	SDL_FreeSurface(temp);
	
	return;
}

#endif
