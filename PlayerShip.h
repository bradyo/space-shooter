#ifndef PLAYERSHIP_H
#define PLAYERSHIP_H

#include "specifications.h"

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
class PlayerShip {
public:
	// Constructor / Destructor
	PlayerShip(ProjectileNode *projectileHead);
	~PlayerShip();
	
	// Utility Functions
	SDL_Rect SetRect(int x, int y, int w, int h);
	
	// IO Functions
	void Update(ProjectileNode *enemyProjectiles, SDL_Event event);
	void Draw(SDL_Surface *lpSScreen);
	bool IsActiveGun(void);
	int GetX(void) { return this->spriteDest.x; }
	int GetY(void) { return this->spriteDest.y; }
	
	// Setting functions
	void SetAnimation(int animationType);
	void SetWeapon(int weaponType);
	void SetLives(int nLives);
	void AddLives(int nLives);
	void SetHealth(int nHealth);
	void AddHealth(int nHealth);
	
	// Action functions
	void Stop(void);
	void Move(void);
	void Explode(void);
	void Revive(void);
	void Sheild(void);
	void ActivateGun(void);
	void DeActivateGun(void);
	void Shoot(void);
	
	// Error flags
	bool initOkay;
	bool drawOkay;
	bool updateOkay;
	
private:
	// Sprite variables
	SDL_Surface *lpSSpriteSurface;
	SDL_Surface *lpSProjectileSprite;
	SDL_Rect spriteDest;
	SDL_Rect spriteSrc;
	
	// Velocities
	int xVelocity;
	int yVelocity;
	
	// Speed variables
	unsigned int cellSpeed;	// cell animation speed
	unsigned int moveSpeed;	// movement speed
	int moveRate;			// pixels moved
		
	// Time variables
	unsigned int cellTicks;	// cell animation speed timer
	unsigned int moveTicks;	// movement speed timer
	
	// Ship State variables
	int animationType;
	int weaponType;	
	bool activeGun;
	
	// Active animation flags
	int activeAnimation;
	
	// Animation flag for reveral animation
	bool reverse;
	
	// Collision stuff
	ProjectileNode *projectiles;	
	
	// Player statistics
	int health;
	int sheild;
	int lives;
	int money;
};

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
PlayerShip::PlayerShip(ProjectileNode *projectileHead)
{
	// Default initOkay (if something goes wrong, this goes false)
	initOkay = true;
	
	// copy projectile head
	projectiles = projectileHead;
	
	// Load ship sprite into temp
	SDL_Surface *temp = SDL_LoadBMP("PlayerShip.bmp");
	if (temp == NULL) {
		printf("ship sprite failed to load\n");
		initOkay = false;
	}	
	// Set sprite's color key to black and copy into lpSSpriteSurface
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY | SDL_RLEACCEL, 
		(Uint32)SDL_MapRGB(temp->format, 0, 0, 0));
	lpSSpriteSurface = SDL_DisplayFormat(temp);
	if (lpSSpriteSurface == NULL) {
		initOkay = false;
	}
	SDL_FreeSurface(temp);
	
	// Load projectile sprite image
	temp = SDL_LoadBMP("Weapons.bmp");
	if (temp == NULL) {
		printf("Weapons.bmp load failed.\n");
		initOkay = false;
	}
	// Set sprite's color key to black
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY | SDL_RLEACCEL, 
		(Uint32)SDL_MapRGB(temp->format, 0, 0, 0));
	lpSProjectileSprite = SDL_DisplayFormat(temp);
	if (lpSProjectileSprite == NULL) {
		initOkay = false;
	}
	SDL_FreeSurface(temp);
	
	// Initialize position on screen
	spriteDest = SetRect(SCREEN_X/2 - CELL_SIZE/2,
		SCREEN_Y/2 - CELL_SIZE/2 + 150, CELL_SIZE, CELL_SIZE);
	
	// Initialize starting cell (6, 4)
	spriteSrc = SetRect(6 * CELL_SIZE, 4 * CELL_SIZE, CELL_SIZE, CELL_SIZE);
	
	// Initialize velocities
	xVelocity = 0;
	yVelocity = 0;
	
	// Initialise speeds (milliseconds)
	cellSpeed 	= 50;
	moveSpeed 	= 1;
	moveRate 	= 1;	// pixels / moveSpeed
	
	// Initialize states
	SetAnimation(MOVE);
	SetWeapon(LASER);	
	activeGun = false;
	
	// Initialize active animation
	activeAnimation = MOVE;
	
	// Initialize other variables
	reverse = false;
	
	// Initialize ticks
	cellTicks = SDL_GetTicks();
	moveTicks = SDL_GetTicks();
	
	// Initialize statistics
	health = 100;
	sheild = 100;
	lives = 3;
	money = 0;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
PlayerShip::~PlayerShip()
{
	// Release sprite
	SDL_FreeSurface(this->lpSSpriteSurface);	
	SDL_FreeSurface(this->lpSProjectileSprite);
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
SDL_Rect PlayerShip::SetRect(int x, int y, int w, int h)
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
void PlayerShip::Update(ProjectileNode *enemyProjectiles, SDL_Event event)
{
	// Check input for ship movement
	switch (event.type) {
	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_LEFT) {
			xVelocity = -moveRate;
		}
		if (event.key.keysym.sym == SDLK_RIGHT) {
			xVelocity = +moveRate;
		}
		if (event.key.keysym.sym == SDLK_UP) {
			yVelocity = -moveRate;
		}
		if (event.key.keysym.sym == SDLK_DOWN) {
			yVelocity = +moveRate;
		}
		break;
	
	case SDL_KEYUP:
		if (event.key.keysym.sym == SDLK_LEFT && xVelocity == -moveRate) {
			xVelocity = 0;
		}
		if (event.key.keysym.sym == SDLK_RIGHT && xVelocity == +moveRate) {
			xVelocity = 0;
		}
		if (event.key.keysym.sym == SDLK_UP && yVelocity == -moveRate) {
			yVelocity = 0;
		}
		if (event.key.keysym.sym == SDLK_DOWN && yVelocity == +moveRate) {
			yVelocity = 0;
		}
		break;
	}
	
	// See if ship has collided with player's projectiles
	int damage = 0;
	if ((damage = enemyProjectiles->Collided(this->spriteDest)) != 0) {
		// COLLISION!!!! destroy ship if damage takes it's health below zero
		health -= damage;
		if (health < 1) {
			Explode();
		}
	}
	
	// Animate based on current animation type
	switch (animationType) {		
	
	// No animation	
	case OFF:
		break;
	
	// Loopback animation
	case MOVE:
		if (SDL_GetTicks() - cellTicks > cellSpeed) {
			if (spriteSrc.x >= CELL_COUNT_X * CELL_SIZE) {
				spriteSrc.x = 0;	
				break;
			}
			spriteSrc.x += CELL_SIZE;
			cellTicks = SDL_GetTicks();
		}
		break;

	// Reversal animation
	case STATIONARY:
		if (SDL_GetTicks() - cellTicks > cellSpeed) {
			if (reverse == true) {
				spriteSrc.x -= CELL_SIZE;
				if (spriteSrc.x < 0) {
					reverse = false;
				}
			} 
			else {
				spriteSrc.x += CELL_SIZE;
				if (spriteSrc.x >= CELL_COUNT_X * CELL_SIZE) { 
					reverse = true;
				}
			}
			cellTicks = SDL_GetTicks();
		}
		break;
		
	// Single animation		
	case EXPLODE:
		if (SDL_GetTicks() - cellTicks > cellSpeed) {
			if (spriteSrc.x >= CELL_COUNT_X * CELL_SIZE) {
				// do this when player dies
				exit(1);
				break;
			}
			spriteSrc.x += CELL_SIZE;
			cellTicks = SDL_GetTicks();
		}
		break;
		
	// Single animation		
	case REVIVE:
		if (SDL_GetTicks() - cellTicks > cellSpeed) {
			if (spriteSrc.x >= CELL_COUNT_X * CELL_SIZE) {
				// do this when player revives
				SetAnimation(MOVE);				
				break;
			}
			spriteSrc.x += CELL_SIZE;
			cellTicks = SDL_GetTicks();
		}
		break;
		
	case SHEILD:
		if (SDL_GetTicks() - cellTicks > cellSpeed) {
			if (spriteSrc.x >= CELL_COUNT_X * CELL_SIZE) {
				// do this when player puts shields up
				SetAnimation(SHEILD);
				break;
			}
			spriteSrc.x += CELL_SIZE;
			cellTicks = SDL_GetTicks();
		}
		break;
		
	case SHOOT:
		if (SDL_GetTicks() - cellTicks > cellSpeed) {
			if (spriteSrc.x >= CELL_COUNT_X * CELL_SIZE) {
				// do this when player is done shooting
				activeAnimation = MOVE;
				SetAnimation(MOVE);	
				
				// Draw particles based on guns
				switch (weaponType) {
				
				case LASER:
					projectiles->AddNode(
						new ProjectileNode(lpSProjectileSprite, spriteDest.x + spriteDest.w/2 - 2, 
						spriteDest.y, 0, -3, weaponType));
					break;
				
				case DUAL_LASER:
					projectiles->AddNode(
						new ProjectileNode(lpSProjectileSprite, spriteDest.x + spriteDest.w/2 - 11, 
						spriteDest.y - 4, 0, -4, weaponType));
					projectiles->AddNode(
						new ProjectileNode(lpSProjectileSprite, spriteDest.x + spriteDest.w/2 + 6, 
						spriteDest.y, 0, -4, weaponType));
					break;
				
				case PLASMA:
					projectiles->AddNode(
						new ProjectileNode(lpSProjectileSprite, spriteDest.x + spriteDest.w/2 - 5, 
						spriteDest.y, 0, -2, weaponType));
					break;
				
				case BEAM:
					projectiles->AddNode(
						new ProjectileNode(lpSProjectileSprite, spriteDest.x + 4, 
						spriteDest.y - 2, 0, -3, weaponType));
					break;
				
				case BALL:
					projectiles->AddNode(
						new ProjectileNode(lpSProjectileSprite, spriteDest.x + spriteDest.w/2 - 4, 
						spriteDest.y, 0, -2, weaponType));
					projectiles->AddNode(
						new ProjectileNode(lpSProjectileSprite, spriteDest.x + spriteDest.w/2 - 4, 
						spriteDest.y, -1, -1, weaponType));
					projectiles->AddNode(
						new ProjectileNode(lpSProjectileSprite, spriteDest.x + spriteDest.w/2 - 4, 
						spriteDest.y, 1, -1, weaponType));
					projectiles->AddNode(
						new ProjectileNode(lpSProjectileSprite, spriteDest.x + spriteDest.w/2 - 4, 
						spriteDest.y, -1, -3, weaponType));
					projectiles->AddNode(
						new ProjectileNode(lpSProjectileSprite, spriteDest.x + spriteDest.w/2 - 4, 
						spriteDest.y, 1, -3, weaponType));
					projectiles->AddNode(
						new ProjectileNode(lpSProjectileSprite, spriteDest.x + spriteDest.w/2 - 4, 
						spriteDest.y, -2, -1, weaponType));
					projectiles->AddNode(
						new ProjectileNode(lpSProjectileSprite, spriteDest.x + spriteDest.w/2 - 4, 
						spriteDest.y, 2, -1, weaponType));
					break;
				}
				
				if (activeGun == true) {
					Shoot();
				}
				break;
			}
			spriteSrc.x += CELL_SIZE;
			cellTicks = SDL_GetTicks();
		}
		break;
	}
	
	// Update position
	if (SDL_GetTicks() - moveTicks > moveSpeed) {
		spriteDest.x += xVelocity;
		spriteDest.y += yVelocity;
		
		// Keep ship from going out of bounds (note, area is 100px from x edges)
		if (spriteDest.x + spriteDest.w > SCREEN_X - 100)
			spriteDest.x = SCREEN_X - 100 - spriteDest.w;
		if (spriteDest.y + spriteDest.h > SCREEN_Y)
			spriteDest.y = SCREEN_Y - spriteDest.h;
		if (spriteDest.x < 0 + 100)
			spriteDest.x = 0 + 100;
		if (spriteDest.y < 0)
			spriteDest.y = 0;
		
		moveTicks = SDL_GetTicks();
	}		
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void PlayerShip::Draw(SDL_Surface *lpSScreen)
{
	SDL_BlitSurface(lpSSpriteSurface, &spriteSrc, lpSScreen, &spriteDest);
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
bool PlayerShip::IsActiveGun(void)
{
	return activeGun;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void PlayerShip::SetAnimation(int type)
{
	animationType = type;
	
	// Initialize cells for the animation type
	switch (animationType) {
		
	case OFF:
		spriteSrc.x = 0 * CELL_SIZE;
		spriteSrc.y = 0 * CELL_SIZE;
		break;
	
	case MOVE:
		spriteSrc.x = 0 * CELL_SIZE;
		spriteSrc.y = 0 * CELL_SIZE;
		break;
	
	case STATIONARY:
		spriteSrc.x = 0 * CELL_SIZE;
		spriteSrc.y = 0 * CELL_SIZE;
		break;
	
	case EXPLODE:
		spriteSrc.x = 0 * CELL_SIZE;
		spriteSrc.y = 1 * CELL_SIZE;
		break;
	
	case REVIVE:
		spriteSrc.x = 0 * CELL_SIZE;
		spriteSrc.y = 2 * CELL_SIZE;
		break;
	
	case SHEILD:
		spriteSrc.x = 0 * CELL_SIZE;
		spriteSrc.y = 3 * CELL_SIZE;
		break;
	
	case SHOOT:
		// Set different animation for different weapons
		switch (weaponType) {
			
		case NONE:
			break;
		
		case LASER:
			break;
		
		case DUAL_LASER:
			spriteSrc.x = 0 * CELL_SIZE;
			spriteSrc.y = 4 * CELL_SIZE;
			break;
		
		case PLASMA:
			spriteSrc.x = 0 * CELL_SIZE;
			spriteSrc.y = 5 * CELL_SIZE;
			break;
		
		case BEAM:
			spriteSrc.x = 0 * CELL_SIZE;
			spriteSrc.y = 6 * CELL_SIZE;
			break;
		
		case BALL:
			spriteSrc.x = 0 * CELL_SIZE;
			spriteSrc.y = 5 * CELL_SIZE;
			break;
		}
		break;
	}
	
	// Start animation timer
	cellTicks = SDL_GetTicks();
	
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void PlayerShip::SetWeapon(int type)
{
	weaponType = type;	
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void PlayerShip::SetLives(int nLives)
{
	lives = nLives;
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void PlayerShip::AddLives(int nLives)
{
	lives += nLives;
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void PlayerShip::SetHealth(int nHealth)
{
	health = nHealth;
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void PlayerShip::AddHealth(int nHealth)
{
	health += nHealth;
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void PlayerShip::Stop(void)
{
	activeAnimation = OFF;
	SetAnimation(OFF);
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void PlayerShip::Move(void)
{
	if (activeAnimation == MOVE) 
		return;
	else {
		activeAnimation = MOVE;
		SetAnimation(MOVE);
	}
	
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void PlayerShip::Explode(void)
{
	if (activeAnimation == EXPLODE)
		return;
	else {
		activeAnimation = EXPLODE;
		SetAnimation(EXPLODE);
	}
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void PlayerShip::Revive(void)
{
	if (activeAnimation == REVIVE)
		return;
	else {
		activeAnimation = REVIVE;
		SetAnimation(REVIVE);
	}
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void PlayerShip::Sheild(void)
{
	if (activeAnimation == SHEILD)
		return;
	else {
		activeAnimation = SHEILD;
		SetAnimation(SHEILD);
	}
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void PlayerShip::ActivateGun(void)
{
	activeGun = true;
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void PlayerShip::DeActivateGun(void)
{
	activeGun = false;
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void PlayerShip::Shoot(void)
{
	if (activeAnimation == SHOOT)
		return;
	else {
		activeAnimation = SHOOT;
		SetAnimation(SHOOT);
	}
	return;
}


#endif
