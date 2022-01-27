#include <SDL.h>
#include <SDL_image.h>
#include "utils.h"
#include "settings.h"
#include "character.h"
#include "Time.h"

using namespace std;

extern SDL_Renderer *renderer;

Character::Character(float xPos, float yPos, int clipX, int clipY)
{
	this->xPos = xPos;
	this->yPos = yPos;
	clip.h = tileSize - 1;
	clip.w = tileSize - 1;
	clip.x = clipX * tileSize;
	clip.y = clipY * tileSize;
	texture = LoadTexture("./Sprites/Skins/PlayerSprite1.png", renderer);
	collisionData = new CollisionData;
}


void Character::Move(int movingHorizontal)
{
	if(collisionData->horizontalCollisionSide == 0 || collisionData->horizontalCollisionSide == -movingHorizontal) 
		xPos += speed * Time::GetDeltaTime() * 100 * movingHorizontal;

}

void Character::Jump()
{
	if (verticalSpeed == 0) {
		jumpSpeed = jumpForce;
		verticalSpeed = 0;
		jumpSpeed += Time::GetDeltaTime() * gravityScale * (9.8 - jumpForce);
	}
}

void Character::CalculateY()
{
	verticalSpeed += 9.8 * Time::GetDeltaTime() * gravityScale;
	yPos += verticalSpeed - jumpSpeed;
	if (collisionData->verticalCollisionSide == 1 && verticalSpeed - jumpSpeed > 0)
	{
		yPos = collisionData->yPos - clip.h * sizeInc;
		verticalSpeed = 0;
		jumpSpeed = 0;
	}
	else if (collisionData->verticalCollisionSide == -1)
	{
		yPos = collisionData->yPos + clip.h * sizeInc + 4;
		verticalSpeed = 0;
		jumpSpeed = 0;
	}

}
