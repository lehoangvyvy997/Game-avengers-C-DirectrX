#pragma once
#include "../GameComponents/GameObject.h"
#include "../GameComponents/Constants.h"
#include "../GameComponents/Game.h"

class HPBar : public GameObject
{
	static HPBar *__instance;

	static vector<Animation *> animations;
	DWORD lastFrameTime;

public:
	int index;
	bool disable = false;

	HPBar(int index);

	static void LoadResources();

	void IsDisable(bool isDisable) { this->disable = isDisable; };

	static HPBar *GetInstance(int index);

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