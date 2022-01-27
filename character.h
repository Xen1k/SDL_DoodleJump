#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "tilemap.h"
#include "GameObject.h"



class Character : public GameObject
{
private:
	float jumpSpeed = 0;
	float verticalSpeed = 0;
public:


	bool isAlive = true;

	struct CollisionData
	{
		int verticalCollisionSide = 0;
		int horizontalCollisionSide = 0;
		int yPos;
		int xPos;
	};


	CollisionData *collisionData;

	float jumpForce = gravityScale * 3.7;
	int speed = 4;

	Character(float xPos, float yPos, int clipRow = 0, int clipColumn = 0);

	void Move(int movingHorizontal);
	void CalculateY();
	void Jump();

};