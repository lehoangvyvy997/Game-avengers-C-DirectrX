#pragma once
#include "../GameComponents/GameObject.h"
#include "Boss1State.h"
#include "../GameComponents/Constants.h"
#include "../GameComponents/Game.h"
#include "../GameComponents/SpawnProjectTile.h"
#include "State.h"

class Boss1 : public GameObject
{
	Boss1();

	static Boss1 *__instance;

	State *state;

	static vector<Animation *> animations;
	DWORD lastFrameTime;

public:
	void LoadResources();

	bool isGrounded = false;

	static Boss1 *GetInstance();

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