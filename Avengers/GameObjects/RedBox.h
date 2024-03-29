#pragma once
#include "../GameComponents/GameObject.h"
#include "RedBoxState.h"
#include "../GameComponents/Constants.h"
#include "../GameComponents/Game.h"
#include "State.h"
#include "../GameComponents/SpawnProjectTile.h"

enum RedBoxType {
	MAP_1,
	MAP_1_EXIT,
	MAP_2,
	MAP_2_EXIT
};

class RedBox : public GameObject
{
	RedBox();

	static RedBox *__instance;

	State *state;

	static vector<Animation *> animations;
	DWORD lastFrameTime;

	RedBoxType type;

public:
	RedBox(float x, float y, RedBoxType type);
	static void LoadResources();

	static RedBox *GetInstance();

	RedBoxType GetType() { return this->type; }

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

	void OnCollision();
};