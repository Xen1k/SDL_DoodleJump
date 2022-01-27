#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include "settings.h"
#include <vector>
#include "GameObject.h"

class Character;



class Tile : public GameObject
{

public:

	bool isPlatform = false;

	Tile(int xPos, int yPos, int clipRow = 0, int clipColumn = 0, bool isPlatform = true);
	int CheckVerticalCollision(Character &player);
	int CheckHorizontalCollision(Character &player);
	
};

class TilesRow
{
public:


	const static int maxTilesInTheRow = (SCREEN_WIDTH / tileSize / sizeInc);
	const int tilesIncrementor = 2;
	std::vector<double> filledTilesMatrix;
	std::vector<Tile> tiles;
	bool enableHorizontalCollisions = false;
	TilesRow(int posY = 0);
	void Render();
	int FindVerticalCollisionTile(Character &player);
	int FindHorizontalCollisionTile(Character &player);

	void DestroyTilesTextures()
	{
		for (int i = 0; i < tiles.size(); i++)
		{
			tiles[i].DestroyTexture();
		}
	}
};