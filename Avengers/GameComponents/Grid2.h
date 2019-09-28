#pragma once
#include "GameObject.h"
#include "SpawnProjectTile.h"
#include "Constants.h" 
#include "../GameObjects/Captain.h"
#include "../GameObjects/RunningMan.h"
#include "../GameObjects/Domesto.h"
#include "../GameObjects/Gigi.h"
#include "../GameObjects/Boss1.h"
#include "../GameObjects/Boss2.h"
#include "../GameObjects/RedBox.h"
#include "../GameObjects/Tank.h"
#include "../GameObjects/Elevator.h"
#include "../GameObjects/Bat.h"
#include "../GameObjects/TwinBricks.h"
#include "../GameObjects/Exit.h"
#include "../GameObjects/HPBar.h"

#include <vector>

#include <fstream>
#include <string>

using namespace std;

#define GRID_SIZE_BY_TILE (int)(GRID_SIZE / TILE_SIZE)

struct OnUpdateObject
{
	GameObject* object;
	Tile2* tile;
	bool disable = false;
	float delaySpawn = 10000;
	float timeCount = 0001;
};

struct GridData
{
	vector<Tile2*> tiles;
	int x;
	int y;
	vector<Tile2*> hasSpawnTiles;
	vector<OnUpdateObject> listObject;

	void Render()
	{
		for (int y = 0; y < GRID_SIZE_BY_TILE; y++)
			for (int x = 0; x < GRID_SIZE_BY_TILE; x++)
			{
				TileMap2::GetInstance()->Render(tiles.at(x + y * GRID_SIZE_BY_TILE));
			}
	}
};

class Boss1;
class Boss2;
class Exit;
class HPBar;
class Grid2
{
private:
	static Grid2* _instance;

	int mapSize;

	Viewport* viewport;

	Captain* captain;

	Boss1* boss1;
	Boss2* boss2;
	Exit* exit;
	//HPBar* hpbar;

	vector <OnUpdateObject> listObject;
	

	void UpdateCurrentTiles();

	float timeCount;

	bool bDisableRespawn;
	int spawnMode;		//1: Inside Screen Spawn, 2: OnDead Creature Spawn
	int killAmount;	//Condition to get out spawn area
	
public:
	static Grid2* GetInstance();

	Grid2();

	void InitializeMapGrid(TileMap2 *tileMap2);

	void GetCameraPosOnGrid(int &l, int &r, int &t, int &b);

	vector<Tile2*> currentTile;
	vector<Tile2 *> GetCurTiles() { return this->currentTile; }

	vector<Tile2 *> GetNearbyTiles(int l, int r, int t, int b);
	vector<Tile2 *> GetNearbyTiles(RECT rect);

	GridData* listCell;

	GridData* GetCell(int x, int y)
	{
		return (listCell + x + y * mapSize);
	}

	void SpawnObject(int ObjectID, Tile2* tile);
	bool CheckObjectInsideCamera(GameObject* object);
	bool CheckTileInsideCamera(Tile2* tile);
	vector <OnUpdateObject> GetListUpdateObject() { return listObject; }

	void Update(DWORD dt);
	void SpawnUpdate(DWORD dt);
	void Render();

	bool spawnboss;
	bool isDisableBoss2;
	void DisableAllObject()
	{
		for (int i = 0; i < listObject.size(); i++)
		{
			listObject.at(i).disable = true;
		}
	}
};