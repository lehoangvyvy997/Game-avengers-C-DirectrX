#pragma once
#include "../GameComponents/GameObject.h"
#include "../GameComponents/Constants.h"
#include "../GameComponents/Game.h"

enum ExitTableState
{
	EXIT_TABLE_HIDE,
	EXIT_TABLE_APPEAR
};

class Exit : public GameObject
{
	Exit();

	static Exit *__instance;

	static vector<Animation *> animations;
	DWORD lastFrameTime;

	ExitTableState state;

public:
	static void LoadResources();

	static Exit *GetInstance();

	DWORD GetLastFrameTime() { return this->lastFrameTime; }
	void SetLastFrameTime(DWORD lastFrameTime) { this->lastFrameTime = lastFrameTime; }

	void SetColliderDemension(float width, float height)
	{
		this->collider.width = width;
		this->collider.height = height;
	}

	vector<Animation *> GetAnimationsList() { return animations; }

	void Update(DWORD dt) override;

	void Render() override;
};