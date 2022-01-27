#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <ctime>
#include "utils.h"
#include "Time.h"
#include "tilemap.h"
#include "settings.h"
#include "character.h"
#include "fileManager.h"
#include <string> 
#include "GameObject.h"

using namespace std;

FileWithRecords *records = nullptr;

enum Scenes { Menu, Game, InputName };
Scenes currentScene;

SDL_Window *window;
SDL_Renderer *renderer;

SDL_Event e;

short movingHorizontal = 0;

SDL_Rect backgroundDst;

vector<TilesRow*> rows;

SDL_Texture *background, *gameOverWindow, *top5Background;

Character *player;

GameObject *viewTutorialButton;


GameObject *pointsCounter, *menuWindow, *tutorialWindow;

SDL_Rect counterClip;
TTF_Font *font;

string playerName = "_";

float fallSpeed = 2;
const float startFallSpeed = fallSpeed;
const float fallAcceleration = 0.005;

int verticalCollisionTile, horizontalCollisionTile;
void CheckVerticalCollision();
void CheckHorizontalCollision();

SDL_Color levelFontColor = { 255,255,255 }, menuFontColor = { 247, 185, 162 };
SDL_Texture *text;

void MoveTiles(int enableAcceleration = 1)
{
	for (int i = 0; i < rows.size(); i++) {
		for (int j = 0; j < rows[i]->tiles.size(); j++)
		{
			fallSpeed += fallAcceleration * Time::GetDeltaTime() * enableAcceleration;
			rows[i]->tiles[j].yPos += fallSpeed * Time::GetDeltaTime() * 100;
		}
	}

	if (rows[0]->tiles[0].yPos >= SCREEN_HEIGHT + tileSize)
	{
		rows.push_back(new TilesRow(-1));
		rows[0]->DestroyTilesTextures();
		rows.erase(rows.begin());
	}
}

void ResetGame()
{
	records->SetNewRecord((int)((fallSpeed - startFallSpeed) * 10), playerName);
	cout << fallSpeed;
	player->isAlive = true;
	player->yPos = 0;
	player->xPos = SCREEN_WIDTH / 2;
	movingHorizontal = 0;
	fallSpeed = startFallSpeed;
	for (int i = 0; i < rows.size(); i++) {
		rows[i]->DestroyTilesTextures();
	}
	rows.clear();
	for (int i = SCREEN_HEIGHT / (tileSize * sizeInc); i >= 0; i--) {
		rows.push_back(new TilesRow(i));
	}
}



bool GameScene();
bool MenuScene();
bool InputNameScene();
bool Update()
{
	Time::SetNow(SDL_GetPerformanceCounter());

	SDL_RenderClear(renderer);
	if (currentScene == Game && !GameScene())
		return false;
	else if (currentScene == Menu && !MenuScene())
		return false;
	else if (currentScene == InputName && !InputNameScene())
		return false;

	SDL_RenderPresent(renderer);

	Time::Calculate();

	return true;
}

bool InputNameScene()
{
	RenderText("Input your name:", font, menuFontColor, 28 * sizeInc, renderer, 17, 2,
		SCREEN_WIDTH / 2 - 17 * 30 / 2,
		SCREEN_HEIGHT / 4);
	RenderText(playerName, font, menuFontColor, 28 * sizeInc, renderer, playerName.length(), 2,
		SCREEN_WIDTH / 2 - playerName.length() * 30 / 2,
		SCREEN_HEIGHT / 2);

	string navText1 = "SPACE - continue";
	string navText2 = "BACKSPACE - erase";

	RenderText(navText1, font, menuFontColor, 28 * sizeInc, renderer, navText1.size(), 2,
		SCREEN_WIDTH / 2 - navText1.size() * 30 / 2,
		SCREEN_HEIGHT - SCREEN_HEIGHT / 4);
	RenderText(navText2, font, menuFontColor, 28 * sizeInc, renderer, navText2.size(), 2,
		SCREEN_WIDTH / 2 - navText2.size() * 30 / 2,
		SCREEN_HEIGHT - SCREEN_HEIGHT / 8);
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_KEYDOWN)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_BACKSPACE:
			{
				playerName = "_";
				break;
			}
			case SDLK_SPACE:
			{
				if (playerName.size() < 7)
				{
					if (playerName == "_")
						playerName = "Unkwn";
					currentScene = Menu;
				}
				else
					playerName = "Length < 7";
				break;
			}
			}
		}
		else if (e.type == SDL_TEXTINPUT)
		{
			if (playerName == "_")
				playerName = "";
			playerName += e.text.text;
		}

		if (e.type == SDL_QUIT)
			return false;
	}

	return true;
}

bool tutorialActiveSelf = false;
bool MenuScene()
{
	int menuTopCoordinate = SCREEN_HEIGHT / 2 - tileSize * sizeInc * 7 / 2;
	menuWindow->Render(sizeInc * 7);
	viewTutorialButton->Render();

	if (tutorialActiveSelf)
		tutorialWindow->Render(sizeInc * 7);

	RenderTexture(top5Background, renderer,
		0,
		menuTopCoordinate,
		sizeInc, nullptr, 3, 5);

	string recordString;
	for (int i = 0; i < 5; i++) {
		
		recordString = records->GetRecordString(i);
		RenderText(recordString, font, menuFontColor, 28 * sizeInc, renderer, recordString.length() / 2, 1,
			20,
			menuTopCoordinate + tileSize * sizeInc / 4 + i * tileSize * sizeInc);
	}
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_KEYDOWN)
		{

			switch (e.key.keysym.sym)
			{
			case SDLK_SPACE:
			{
				ResetGame();
				currentScene = Game;
				break;
			}
			case SDLK_ESCAPE:
			{
				return false;
				break;
			}
			}
		}
		
		if (e.button.button == SDL_BUTTON_LEFT)
		{
			if (viewTutorialButton->IsHovering())
				tutorialActiveSelf = true;
		}

		if (e.type == SDL_MOUSEBUTTONUP)
			if (e.button.button == SDL_BUTTON_LEFT)
				tutorialActiveSelf = false;

		if (e.type == SDL_QUIT)
			return false;
	}

	return true;
}

void Rerender()
{
	SDL_RenderCopy(renderer, background, nullptr, &backgroundDst);
	for (int i = 0; i < rows.size(); i++)
	{
		rows[i]->Render();
	}
	player->Render();
	pointsCounter->Render();
	if ((int)((fallSpeed - startFallSpeed) * 10) < 10)
		RenderText(to_string((int)((fallSpeed - startFallSpeed) * 10)), font, levelFontColor, 28 * sizeInc, renderer, 0.5, 1, counterClip.w / 1.25, counterClip.h / 2.5);
	else
		RenderText(to_string((int)((fallSpeed - startFallSpeed) * 10)), font, levelFontColor, 28 * sizeInc, renderer, 0.85, 1, counterClip.w / 2, counterClip.h / 2.5);
}


bool GameScene()
{
	Rerender();


	if (player->isAlive) {
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_KEYDOWN)
			{

				if (!e.key.repeat)
				{
					switch (e.key.keysym.sym)
					{
					case SDLK_SPACE:
					{
						player->Jump();
						break;
					}
					case SDLK_d:
					{
						player->facesRight = true;
						movingHorizontal = 1;
						break;
					}
					case SDLK_a:
					{
						player->facesRight = false;
						movingHorizontal = -1;
						break;
					}
					case SDLK_ESCAPE:
					{
						ResetGame();
						currentScene = Menu;
						break;
					}
					}
				}
			}
			else if (e.type == SDL_KEYUP)
			{
				if (!e.key.repeat)
				{
					switch (e.key.keysym.sym)
					{
					case SDLK_d:
					{
						movingHorizontal = 0;
						break;
					}
					case SDLK_a:
					{
						movingHorizontal = 0;
						break;
					}
					}
				}
			}
			if (e.type == SDL_QUIT)
				return false;
		}

		player->Move(movingHorizontal);

		MoveTiles();

		CheckVerticalCollision();
		CheckHorizontalCollision();

		player->CalculateY();
		
		if (player->yPos >= SCREEN_HEIGHT + tileSize)
		{
			player->isAlive = false;
		}
	}
	else
	{

		player->Render();

		RenderTexture(gameOverWindow, renderer,
			SCREEN_WIDTH / 2 - tileSize * sizeInc * 7 / 2,
			SCREEN_HEIGHT / 2 - tileSize * sizeInc * 7 / 2,
			sizeInc * 7);


		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_KEYDOWN)
			{
				if (!e.key.repeat)
				{
					switch (e.key.keysym.sym)
					{

					case SDLK_r:
					{
						ResetGame();
						break;
					}
					case SDLK_m:
					{
						ResetGame();
						currentScene = Menu;
						break;
					}

					}
				}
			}
			if (e.type == SDL_WINDOWEVENT)
			{
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					int screen_width, screen_height;
					SDL_GetWindowSize(window, &screen_width, &screen_height);
				}
				break;
			}
			if (e.type == SDL_QUIT)
			{
				return false;
			}
		}

		if ((int)((fallSpeed - startFallSpeed) * 10) < 10)
			RenderText(to_string((int)((fallSpeed - startFallSpeed) * 10)), font, levelFontColor, 28 * sizeInc, renderer, 0.5, 1, counterClip.w / 1.25, counterClip.h / 2.5);
		else
			RenderText(to_string((int)((fallSpeed - startFallSpeed) * 10)), font, levelFontColor, 28 * sizeInc, renderer, 0.85, 1, counterClip.w / 2, counterClip.h / 2.5);

	}
}

void CheckVerticalCollision()
{
	for (int i = 0; i < rows.size(); i++) {
		verticalCollisionTile = rows[i]->FindVerticalCollisionTile(*player);

		if (verticalCollisionTile == -1) {
			player->collisionData->verticalCollisionSide = 0;
			continue;
		}
		player->collisionData->verticalCollisionSide = rows[i]->tiles[verticalCollisionTile].CheckVerticalCollision(*player);
		if (player->collisionData->verticalCollisionSide != 0) {
			player->collisionData->yPos = rows[i]->tiles[verticalCollisionTile].yPos;
		}
		if (verticalCollisionTile != -1)
			break;
	}
}

void CheckHorizontalCollision()
{
	for (int i = 0; i < rows.size(); i++) {
		horizontalCollisionTile = rows[i]->FindHorizontalCollisionTile(*player);
		if (horizontalCollisionTile == -1) {
			player->collisionData->horizontalCollisionSide = 0;
			continue;
		}
		player->collisionData->horizontalCollisionSide = rows[i]->tiles[horizontalCollisionTile].CheckHorizontalCollision(*player);
		if (horizontalCollisionTile != -1)
			break;
	}

}

int main(int argc, char** argv)
{
	window = CreateWindow();
	renderer = CreateRenderer(window);
	SDL_RenderClear(renderer);
	srand(time(nullptr));
	TTF_Init();

	records = new FileWithRecords;

	currentScene = InputName;



	font = TTF_OpenFont("./progressPixel.ttf", 40);

	for (int i = SCREEN_HEIGHT / (tileSize * sizeInc); i >= 0; i--) {
		rows.push_back(new TilesRow(i));
	}

	counterClip.h = tileSize;
	counterClip.w = tileSize;
	counterClip.x = 4 * tileSize;
	counterClip.y = 0 * tileSize;

	backgroundDst.x = 0;
	backgroundDst.y = 0;
	backgroundDst.w = SCREEN_WIDTH;
	backgroundDst.h = SCREEN_HEIGHT;

	player = new Character(SCREEN_WIDTH / 2, 0);

	viewTutorialButton = new GameObject(SCREEN_WIDTH - tileSize * sizeInc, 0, "./Sprites/DesertTileset.png");

	pointsCounter = new GameObject(0, 0, "./Sprites/DesertTileset.png", 4, 0);
	menuWindow = new GameObject(SCREEN_WIDTH / 2 - tileSize * sizeInc * 7 / 2, SCREEN_HEIGHT / 2 - tileSize * sizeInc * 7 / 2, "./Sprites/MenuWindow.png", 0, 0, 7, 7);
	tutorialWindow = new GameObject(SCREEN_WIDTH / 2 - tileSize * sizeInc * 7 / 2, SCREEN_HEIGHT / 2 - tileSize * sizeInc * 7 / 2, "./Sprites/TutorialWindow.png", 0, 0, 7, 7);

	background = LoadTexture("./Sprites/background.png", renderer);
	gameOverWindow = LoadTexture("./Sprites/GameOverWindow.png", renderer);
	top5Background = LoadTexture("./Sprites/Top5.png", renderer);

	while (Update());

	return 0;
}

