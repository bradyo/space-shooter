#ifndef SPECIFICATIONS_H
#define SPECIFICATIONS_H

// Screen
const int SCREEN_X 		= 640;
const int SCREEN_Y 		= 480;

// Sprites
const int CELL_SIZE 	= 33;
const int CELL_COUNT_X 	= 5;

// Animation types
const int OFF			= 0;
const int MOVE			= 4;
const int STATIONARY	= 5;
const int EXPLODE		= 6;
const int REVIVE		= 7;
const int SHEILD		= 8;
const int SHOOT			= 9;

// Weapon types
const int NONE 			= 0;
const int LASER 		= 1;
const int DUAL_LASER	= 2;
const int PLASMA		= 3;
const int BEAM			= 4;
const int BALL			= 5;

// AI types
const int DO_NOTHING			= 1;
const int STATIONARY_SHOOTING 	= 2;
const int FLY_DOWN_SHOOTING		= 3;

#endif
