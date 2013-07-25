#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "specifications.h"

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
class ProjectileNode {
public:
	// Constructor / Destructor
	ProjectileNode();
	ProjectileNode(SDL_Surface *sprite, int xStart, int yStart, int xVelocity, int yVelocity, int type);
	~ProjectileNode();
	
	// Utility Functions
	SDL_Rect SetRect(int x, int y, int w, int h);
	
	// IO Functions
	void Update(void);
	void Draw(SDL_Surface *lpSScreen);
	int Collided(SDL_Rect object);
	
	// List Functions
	void AddNode(ProjectileNode *node);
	void DeleteThisNode(void);
		
	// List pointers
	ProjectileNode *nextNode;
	ProjectileNode *tempNode;
	bool isAlive;
	
private:
	// Sprite variables
	SDL_Surface *lpSSprite;
	SDL_Rect spriteDest;
	SDL_Rect spriteSrc;
	
	// Width / Height of collision detection
	int wCollision;
	int hCollision;
	int damage;
	
	// Velocity variables
	int xVelocity;
	int yVelocity;
	
	// State variables
	int projectileType;
	
	// Time variables
	unsigned int cellTicks;
	unsigned int moveTicks;
	int moveRate;
	int cellRate;
};

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
ProjectileNode::ProjectileNode()
{	
	// This node is the head.

	// Load Sprite
	this->lpSSprite = NULL;
	
	// Initialize all these for saftey
	spriteDest = SetRect(SCREEN_X/2, SCREEN_Y/2, 1, 1);
	spriteSrc = SetRect(0, 0, 1, 1);
	
	wCollision = 0;
	hCollision = 0;
	
	damage = 0;
	
	// Set initial velocities
	xVelocity = 0;
	yVelocity = 0;
	
	// initialize list pointers
	nextNode = NULL;
	tempNode = NULL;
	isAlive = true;
	
	// set type as none
	projectileType = NONE;
	
	// Start timer
	cellRate = 200;
	moveRate = 100;
	cellTicks = SDL_GetTicks();
	moveTicks = SDL_GetTicks();
	
	printf("Head Node created successfully.\n");
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
ProjectileNode::ProjectileNode(SDL_Surface *sprite, int xStart, int yStart, int nxVelocity, int nyVelocity, int type)
{
	// Load Sprite
	this->lpSSprite = sprite;
	projectileType = type;
	
	// update rects and collisions for projectile
	switch (projectileType) {
	
	case LASER:	
		wCollision = 5;
		hCollision = 10;
	
		spriteSrc.x = 0;
		spriteSrc.y = 0 * CELL_SIZE;
		spriteSrc.w = wCollision;
		spriteSrc.h = hCollision;
	
		damage = 5;
		break;
	
	case BEAM:
		wCollision = 22;
		hCollision = 8;
	
		spriteSrc.x = 0;
		spriteSrc.y = 1 * CELL_SIZE;
		spriteSrc.w = wCollision;
		spriteSrc.h = hCollision;
	
		damage = 6;
		break;
	
	case PLASMA:	
		wCollision = 10;
		hCollision = 9;
	
		spriteSrc.x = 0;
		spriteSrc.y = 2 * CELL_SIZE;
		spriteSrc.w = wCollision;
		spriteSrc.h = hCollision;
	
		damage = 15;
		break;
	
	case BALL:
		wCollision = 9;
		hCollision = 9;
	
		spriteSrc.x = 0;
		spriteSrc.y = 3 * CELL_SIZE;
		spriteSrc.w = wCollision;
		spriteSrc.h = hCollision;
	
		damage = 25;
		break;
	
	default:
		projectileType = LASER;
	
		wCollision = 5;
		hCollision = 10;
	
		spriteSrc.x = 0;
		spriteSrc.y = 0;
		spriteSrc.w = wCollision;
		spriteSrc.h = hCollision;
	
		damage = 10;
		break;
	}	
	
	// Set initial position of projectile
	spriteDest.x = xStart;
	spriteDest.y = yStart;
	spriteDest.w = spriteSrc.w;
	spriteDest.h = spriteSrc.h;
	
	// Set initial velocities
	xVelocity = nxVelocity;
	yVelocity = nyVelocity;
	
	// initialize list pointers
	nextNode = NULL;	
	tempNode = NULL;
	isAlive = true;
	
	// initialize time variables
	cellRate = 200;
	moveRate = 100;
	cellTicks = SDL_GetTicks();
	moveTicks = SDL_GetTicks();
	
	printf("Created Node\n");
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
ProjectileNode::~ProjectileNode()
{
	printf("Deleted Node\n");
	
	if (nextNode != NULL) {
		delete nextNode;
	}
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
SDL_Rect ProjectileNode::SetRect(int x, int y, int w, int h)
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
void ProjectileNode::Update(void)
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
	if (projectileType == NONE) {
		// if any other projectiles in list, update them too
		if (nextNode != NULL) {
			nextNode->Update();
		}
		return;
	}
	
	// Update the particles animation
	if (SDL_GetTicks() - cellTicks > (unsigned int)cellRate) {
		if (spriteSrc.x >= CELL_COUNT_X * CELL_SIZE) {
			spriteSrc.x = 0;	
			cellTicks = SDL_GetTicks();
		}
		else {
			spriteSrc.x += CELL_SIZE;
			cellTicks = SDL_GetTicks();
		}
	}
	
	// Move the particle
	if (SDL_GetTicks() - moveTicks > (unsigned int)moveRate) {
		// update position based on velocity
		spriteDest.x += xVelocity;
		spriteDest.y += yVelocity;
	
		// If projectile goes offscreen, delete it
		if (spriteDest.x < 0 + 100 || spriteDest.x + spriteDest.w >= SCREEN_X - 100)
			DeleteThisNode();
		if (spriteDest.y < 0 || spriteDest.y + spriteDest.h > SCREEN_Y)
			DeleteThisNode();
	}
		
	// if any other projectiles in list, update them too
	if (nextNode != NULL) {
		nextNode->Update();
	}
	
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void ProjectileNode::Draw(SDL_Surface *lpSScreen)
{
	if (projectileType == NONE) {
		// if any other projectiles in list, draw them too
		if (nextNode != NULL) {
			nextNode->Draw(lpSScreen);
		}
		return;
	}
	
	if (lpSSprite != NULL) {
		// Draw projectile 
		//SDL_FillRect(lpSScreen, &spriteDest, (Uint16)SDL_MapRGB(lpSScreen->format, 255, 255, 255));
		SDL_BlitSurface(lpSSprite, &spriteSrc, lpSScreen, &spriteDest);
	}
	
	// if any other projectiles in list, draw them too
	if (nextNode != NULL) {
		nextNode->Draw(lpSScreen);
	}
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
int ProjectileNode::Collided(SDL_Rect object)
{
	if (spriteDest.x > object.x && spriteDest.x < object.x + object.w) {
		if (spriteDest.y > object.y && spriteDest.y < object.y + object.h) {
			DeleteThisNode();
			return damage;		}
	}
	
	if (nextNode != NULL) {
		return nextNode->Collided(object);
	}
	
	return 0;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void ProjectileNode::AddNode(ProjectileNode *node)
{
	if (nextNode != NULL) {
		nextNode->AddNode(node);
	}
	else {
		nextNode = node;
	}
	return;
}

// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
// =====-----=====-----=====-----=====-----=====-----=====-----=====-----=====
void ProjectileNode::DeleteThisNode(void)
{
	this->isAlive = false;
	return;
}

#endif
