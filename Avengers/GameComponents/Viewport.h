#pragma once
#include "d3d9.h"
#include "d3dx9.h"
#include "Constants.h"
#include "Sound.h"

class GameObject;
class Viewport
{
	static Viewport *__instance;
	Viewport();
	int width;
	int height;
	int x;
	int y;
	GSound* sound_lock;
public:
	static Viewport *GetInstance();
	~Viewport();

	RECT GetRect();

	int GetPosX() { return this->x; }
	int GetPosY() { return this->y; }

	void Reset();
	void Update(DWORD dt);

	bool isLock = false;
	bool canLock = true;

	bool IsObjectInCamera(GameObject *gameobject);
	void SetRenderData(D3DXVECTOR2 &center, D3DXVECTOR2 &translate, D3DXVECTOR2 &scaling);
};