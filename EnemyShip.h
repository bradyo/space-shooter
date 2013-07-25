#ifndef ENEMYSHIP_H
#define ENEMYSHIP_H

#include "specifications.h"

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
class EnemyShip {
public:
	// Constructor / Destructor
	EnemyShip(ProjectileNode *headNode);
	EnemyShip(ProjectileNode *projectileHead, SDL_Surface *projectileSprite, char *spriteFilename, 
		int xStart, int yStart, int nHealth, int nAiType);
	~EnemyShip();
	
	// Utility Functions
	SDL_Rect SetRect(int x, int y, int w, int h);
	
	// IO Functions
	void Update(ProjectileNode *projectileList);
	void Draw(SDL_Surface *lpSScreen);
	
	// Action functions
	void Explode(void);
	
	// List Functions
	void AddNode(EnemyShip *node);
	void AddNode(char *spriteFilename, int xStart, int yStart, int nHealth, int nAiType);
	void DeleteThisNode(void);
		
	// List pointers
	EnemyShip *nextNode;
	EnemyShip *tempNode;
	bool isAlive;
	
	// error flags
	bool initOkay;
	
private:
	// Sprite variables
	SDL_Surface *lpSSprite;
	SDL_Surface *lpSProjectileSprite;
	SDL_Rect spriteDest;
	SDL_Rect spriteSrc;
	
	// hit points
	int health;
	
	// Velocity variables
	int xVelocity;
	int yVelocity;
	
	// projectiles
	ProjectileNode *projectiles;
	
	// State variables
	int animationType;
	int aiType;
	
	// Time variables
	unsigned int cellTicks;
	unsigned int moveTicks;
	int moveRate;
	int cellRate;
};

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
EnemyShip::EnemyShip(ProjectileNode *headNode)
{	
	initOkay = true;
	
	// This node is the head
	lpSSprite = NULL;
	
	// Load projectile sprite image
	SDL_Surface *temp = SDL_LoadBMP("Weapons.bmp");
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

	// copy projectile head
	projectiles = headNode;
		
	// Initialize all these for saftey
	spriteDest = SetRect(SCREEN_X/2, SCREEN_Y/2, 1, 1);
	spriteSrc = SetRect(0, 0, 1, 1);
	
	health = 1;
	
	// Set initial velocities
	xVelocity = 0;
	yVelocity = 0;
	
	// initialize list pointers
	nextNode = NULL;
	tempNode = NULL;
	isAlive = true;
	
	// set type as none
	aiType = NONE;
	animationType = NONE;
	
	// Start timer
	cellRate = 200;
	moveRate = 100;
	cellTicks = SDL_GetTicks();
	moveTicks = SDL_GetTicks();
	
	printf("Head Node created successfully.\n");
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
EnemyShip::EnemyShip(ProjectileNode *projectileHead, SDL_Surface *projectileSprite, char *spriteFilename, 
	int xStart, int yStart, int nHealth, int nAiType)
{
	// copy projectile head
	projectiles = projectileHead;
	
	lpSProjectileSprite = projectileSprite;
	
	// Load projectile sprite image
	SDL_Surface *temp = SDL_LoadBMP(spriteFilename);
	if (temp == NULL) {
		printf("load failed.\n");
		initOkay = false;
	}
	// Set sprite's color key to black
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY | SDL_RLEACCEL, 
		(Uint32)SDL_MapRGB(temp->format, 0, 0, 0));
	lpSSprite = SDL_DisplayFormat(temp);
	if (lpSSprite == NULL) {
		initOkay = false;
	}
	SDL_FreeSurface(temp);
	
	// set states
	aiType = nAiType;
	animationType = MOVE;
	
	spriteSrc.x = 0;
	spriteSrc.y = 0;
	spriteSrc.w = lpSSprite->w / (CELL_COUNT_X+1);
	spriteSrc.h = lpSSprite->h = spriteSrc.w;
	
	// Set initial position of projectile
	spriteDest.x = xStart;
	spriteDest.y = yStart;
	spriteDest.w = spriteSrc.w;
	spriteDest.h = spriteSrc.h;
	
	// Set initial velocities
	xVelocity = 0;
	yVelocity = 0;

	// initialize time variables
	cellRate = 200;
	moveRate = 200;
	cellTicks = SDL_GetTicks();
	moveTicks = SDL_GetTicks();
	
	// initialize list pointers
	nextNode = NULL;	
	tempNode = NULL;
	isAlive = true;
	
	health = nHealth;
	
	printf("+) Created Enemy Ship\n");
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
EnemyShip::~EnemyShip()
{
	printf("-) Deleted Enemy Ship\n");
	
	if (nextNode != NULL) {
		delete nextNode;
	}
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
SDL_Rect EnemyShip::SetRect(int x, int y, int w, int h)
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
void EnemyShip::Update(ProjectileNode *projectileList)
{		
	// Check for node deletion
	if (nextNode != NULL && nextNode->isAlive == false) {
		if (this->nextNode->nextNode != NULL) {
			this->tempNode = this->nextNode->nextNode;
		}
		else {
			this->tempNode = NULL;
		}
		this->nextNode->nextNode = NULL;
		
		delete this->nextNode;
		this->nextNode = this->tempNode;
		this->tempNode = NULL;
	}
	
	// dont do if there is no projectile
	if (aiType == NONE) {
		// if any other projectiles in list, update them too
		if (nextNode != NULL) {
			nextNode->Update(projectileList);
		}
		return;
	}
	
	// See if ship has collided with player's projectiles
	int damage = 0;
	if ((damage = projectileList->Collided(this->spriteDest)) != 0) {
		// COLLISION!!!! destroy ship if damage takes it's health below zero
		health -= damage;
		if (health < 1) {
			Explode();
		}
		// if any other projectiles in list, update them too
		if (nextNode != NULL) {
			nextNode->Update(projectileList);
		}
		return;
	}
	
	// Update the particles animation
	switch (animationType) {
	case MOVE:
	case STATIONARY:
		if (SDL_GetTicks() - cellTicks > (unsigned int)cellRate) {
			if (spriteSrc.x > lpSSprite->w - spriteSrc.w - 5) {
				spriteSrc.x = 0;	
				cellTicks = SDL_GetTicks();
			}
			else {
				spriteSrc.x += CELL_SIZE;
				cellTicks = SDL_GetTicks();
			}
		}
		break;
	case EXPLODE:
		if (SDL_GetTicks() - cellTicks > (unsigned int)cellRate) {
			if (spriteSrc.x > lpSSprite->w - spriteSrc.w - 5) {
				DeleteThisNode();
			}
			else {
				spriteSrc.x += CELL_SIZE;
				cellTicks = SDL_GetTicks();
			}
		}
		break;
	}
	
	// Move the particle based on AI
	if (SDL_GetTicks() - moveTicks > (unsigned int)moveRate) {
		switch (aiType) {
		
		case DO_NOTHING:
			xVelocity = 0;
			yVelocity = 0;
			break;

		case STATIONARY_SHOOTING:
			// update position based on velocity
			moveRate = rand() % 5000 + 200;
			projectiles->AddNode(
				new ProjectileNode(lpSProjectileSprite, spriteDest.x + spriteDest.w/2 - 2, 
				spriteDest.y+10, 0, +2, BALL));
			xVelocity = 0;
			yVelocity = 0;
			break;
		
		case FLY_DOWN_SHOOTING:			
			// update position based on velocity
			if (rand()%20 + 1 > 15) {
				projectiles->AddNode(
					new ProjectileNode(lpSProjectileSprite, spriteDest.x + spriteDest.w/2 - 2, 
					spriteDest.y+10, 0, +2, BALL));
			}
			xVelocity = 0;
			yVelocity = 1;
			break;
		}
		
		// update position based on velocity
		spriteDest.x += xVelocity;
		spriteDest.y += yVelocity;
		
		xVelocity = 0;
		yVelocity = 0;
		
		// If projectile goes offscreen, delete it
		if (spriteDest.x < 0 + 100 || spriteDest.x + spriteDest.w >= SCREEN_X - 100) {
			AddNode("Enemy01.bmp", 100+rand()%(440-CELL_SIZE), rand()%100, 25, FLY_DOWN_SHOOTING);
			AddNode("Enemy01.bmp", 100+rand()%(440-CELL_SIZE), rand()%100, 25, FLY_DOWN_SHOOTING);
			DeleteThisNode();
		}
		if (spriteDest.y < 0 || spriteDest.y + spriteDest.h > SCREEN_Y) {
			AddNode("Enemy01.bmp", 100+rand()%(440-CELL_SIZE), rand()%100, 25, FLY_DOWN_SHOOTING);
			AddNode("Enemy01.bmp", 100+rand()%(440-CELL_SIZE), rand()%100, 25, FLY_DOWN_SHOOTING);
			DeleteThisNode();
		}
		
		moveTicks = SDL_GetTicks();
	}
		
		
	// if any other projectiles in list, update them too
	if (nextNode != NULL) {
		nextNode->Update(projectileList);
	}
	
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void EnemyShip::Draw(SDL_Surface *lpSScreen)
{
	if (aiType == NONE) {
		// if any other projectiles in list, draw them too
		if (nextNode != NULL) {
			nextNode->Draw(lpSScreen);
		}
		return;
	}
	
	// Draw projectile 
	SDL_BlitSurface(lpSSprite, &spriteSrc, lpSScreen, &spriteDest);

	// if any other projectiles in list, draw them too
	if (nextNode != NULL) {
		nextNode->Draw(lpSScreen);
	}
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void EnemyShip::Explode(void)
{
	if (animationType == EXPLODE) {
		return;
	}
	
	spriteSrc.y = spriteSrc.h;
	spriteSrc.x = 0;

	animationType = EXPLODE;
	cellTicks = SDL_GetTicks();
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void EnemyShip::AddNode(EnemyShip *node)
{
	if (nextNode != NULL) {
		nextNode->AddNode(node);
	}
	else {
		nextNode = node;
	}
	return;
}

void EnemyShip::AddNode(char *spriteFilename, int xStart, int yStart, int nHealth, int nAiType)
{
	if (nextNode != NULL) {
		nextNode->AddNode(new EnemyShip(this->projectiles, this->lpSProjectileSprite, spriteFilename, 
			xStart, yStart, nHealth, nAiType));
	}
	else {
		nextNode = new EnemyShip(this->projectiles, this->lpSProjectileSprite, spriteFilename, 
			xStart, yStart, nHealth, nAiType);
	}
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void EnemyShip::DeleteThisNode(void)
{
	this->isAlive = false;
	return;
}



#endif
