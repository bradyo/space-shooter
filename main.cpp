#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Background.h"
#include "Projectile.h"
#include "PlayerShip.h"
#include "EnemyShip.h"


SDL_Surface *lpSScreen = NULL;

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
int Play(void)
{		
	srand(time(NULL));
	for (int i = 0; i < rand()%20+10; i++) {
		rand();
	}
	
	// Create the background image (scrolls)
	Background background;
	if (background.initOkay != true) {
		printf("Background background init failed\n");
		return 1;
	}
	background.SetType(SPACE);

	// Create Projectile list HEAD NODE
	ProjectileNode projectileList;
	ProjectileNode enemyProjectileList;
	
	// Create Ship
	PlayerShip ship(&projectileList);
	
	
	EnemyShip enemyList(&enemyProjectileList);
	for (int i = 0; i < 10; i ++) {
		enemyList.AddNode("Enemy01.bmp", 100+rand()%(440-CELL_SIZE), rand()%450+10, 25, FLY_DOWN_SHOOTING);
	}
	
	
	SDL_Event event;
	while (1) {
		
		SDL_PollEvent(&event);
		
		switch (event.type) {
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_1) {
				ship.SetWeapon(LASER);
			}
			if (event.key.keysym.sym == SDLK_2) {
				ship.SetWeapon(DUAL_LASER);
			}
			if (event.key.keysym.sym == SDLK_3) {
				ship.SetWeapon(PLASMA);
			}
			if (event.key.keysym.sym == SDLK_4) {
				ship.SetWeapon(BEAM);
			}
			if (event.key.keysym.sym == SDLK_5) {
				ship.SetWeapon(BALL);
			}
			if (event.key.keysym.sym == SDLK_6) {
				ship.SetWeapon(1);
			}
			if (event.key.keysym.sym == SDLK_LCTRL) {
				ship.Shoot();
				ship.ActivateGun();
			}
			if (event.key.keysym.sym == SDLK_q) {
				exit(0);
			}
			break;
			
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_LCTRL) {
				ship.DeActivateGun();
			}
			break;
		
		case SDL_QUIT:
			return 0;
		}
		
		background.Update();
		background.Draw(lpSScreen);
		
		enemyList.Update(&projectileList);
		enemyList.Draw(lpSScreen);
		
		ship.Update(&enemyProjectileList, event);
		ship.Draw(lpSScreen);
		
		projectileList.Update();
		projectileList.Draw(lpSScreen);
		
		enemyProjectileList.Update();
		enemyProjectileList.Draw(lpSScreen);
		
		SDL_Flip(lpSScreen);
	}
	
	return 0;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
int main (int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("ERROR: SDL_Init() failed.\n");
		return 1;
	}
	
	atexit(SDL_Quit);
	
	lpSScreen = SDL_SetVideoMode(640, 480, 16, SDL_DOUBLEBUF);
	if (lpSScreen == NULL) {
		printf("ERROR: SDL_SetVideoMode() failed.\n");
		return 1;
	}
	
	printf("Entering Play\n");
	if (Play() != 0) { 
		printf("Play returned fail.\n");
		return 1; 
	}
	
	return 0;
}
