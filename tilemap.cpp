#include <SDL.h>
#include <SDL_image.h>
#include "tilemap.h"
#include "utils.h"
#include "settings.h"
#include "character.h"
#include <vector>
#include <ctime>
#include "time.h"

using namespace std;

extern SDL_Renderer *renderer;


TilesRow::TilesRow(int posY)
{

	int placesQuantity = rand() % maxTilesInTheRow / 6 + tilesIncrementor;

	filledTilesMatrix.resize(maxTilesInTheRow);

	int tileXPos;
	for (int i = 0; i < placesQuantity; i++)
	{
		tileXPos = rand() % (SCREEN_WIDTH / tileSize / sizeInc)* tileSize * sizeInc;
		tiles.push_back(Tile(tileXPos, posY * tileSize * sizeInc, 3));
		filledTilesMatrix[tileXPos / tileSize / sizeInc] = 1;
	}

}

int TilesRow::FindHorizontalCollisionTile(Character &player)
{
	for (int i = 0; i < tiles.size(); i++)
	{
		if (tiles[i].CheckHorizontalCollision(player) != 0)
		{
			return i;
		}
	}
	if(enableHorizontalCollisions)
		return -1;
	else
		return 0;
}

int TilesRow::FindVerticalCollisionTile(Character &player)
{
	for (int i = 0; i < tiles.size(); i++)
	{

		if (tiles[i].CheckVerticalCollision(player) != 0)
		{
			return i;
		}
	}
	return -1;
}

void TilesRow::Render()
{
	for (int i = 0; i < tiles.size(); i++)
	{
		tiles[i].Render();
	}
}

Tile::Tile(int xPos, int yPos, int clipX, int clipY, bool isPlatform)
{
	this->xPos = xPos;
	this->yPos = yPos;
	this->isPlatform = isPlatform;
	clip.h = tileSize;
	clip.w = tileSize;
	clip.x = clipX * tileSize;
	clip.y = clipY * tileSize;
	texture = LoadTexture("./Sprites/DesertTileset.png", renderer);
}



int Tile::CheckVerticalCollision(Character &player)
{
	int playerSkinDeltaX = 15;

	int playerBottom = player.yPos + player.GetSpriteHeight() * sizeInc;
	int playerTop = player.yPos;
	int tileBottom = yPos + clip.h * sizeInc;
	int tileTop = yPos;

	int playerRight = player.xPos + player.GetSpriteWidth() * sizeInc - playerSkinDeltaX;
	int playerLeft = player.xPos + playerSkinDeltaX;
	int tileRight = xPos + clip.w * sizeInc;
	int tileLeft = xPos;

	if (tileRight > playerLeft && tileLeft < playerRight) {

		if (playerBottom >= tileTop - 10 && playerBottom <= tileTop + 10) {
			return 1;
		}

		if (!isPlatform && playerTop <= tileBottom + 1 && playerTop >= tileBottom - 1) {
			return -1;
		}
	}
	return 0;
}

int Tile::CheckHorizontalCollision(Character &player)
{
	int playerBottom = player.yPos + player.GetSpriteHeight() * sizeInc;
	int playerTop = player.yPos;
	int tileBottom = yPos + clip.h * sizeInc;
	int tileTop = yPos;

	int playerRight = player.xPos + player.GetSpriteWidth() * sizeInc;
	int playerLeft = player.xPos;
	int tileRight = xPos + clip.w * sizeInc;
	int tileLeft = xPos;

	if (playerBottom > tileTop && playerTop < tileBottom) 
	{
		if (playerRight <= tileLeft + 1 && playerRight >= tileLeft - 1)
			return 1;
		if (playerLeft <= tileRight + 1 && playerLeft >= tileLeft - 1)
			return -1;
	}

	return 0;
}

