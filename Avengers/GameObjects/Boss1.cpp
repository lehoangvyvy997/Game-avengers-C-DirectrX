#include "Boss1.h"

vector<Animation *> Boss1::animations = vector<Animation *>();
Boss1 *Boss1::__instance = NULL;


Boss1::Boss1()
{
	LoadResources();

	state = Boss1State::GetInstance(this);

	this->x = 170;
	this->y = 72;
	this->width = BOSS1_SPRITE_WIDTH;
	this->height = BOSS1_SPRITE_HEIGHT;

	collider.x = x;
	collider.y = y;
	collider.vx = 0;
	collider.vy = 0;
	collider.width = BOSS1_SPRITE_WIDTH;
	collider.height = BOSS1_SPRITE_HEIGHT;

	this->HP = 10;
}

Boss1 *Boss1::GetInstance()
{
	if (__instance == NULL)
		__instance = new Boss1();
	return __instance;
}

void Boss1::LoadResources()
{
	LoadTXT loadTXT;
	RECT* listSprite = loadTXT.LoadRect((char*)"Resources\\Enemies\\Boss1.txt");

	// BOSS1_ANI_IDLE
	Animation * anim = new Animation(100);
	for (int i = 0; i < 1; i++)
	{
		Sprite * sprite = new Sprite(BOSS1_TEXTURE_LOCATION, listSprite[i], TEXTURE_TRANS_COLOR);
		anim->AddFrame(sprite);
	}
	animations.push_back(anim);

	// BOSS1_ANI_RUNNING
	anim = new Animation(100);
	for (int i = 2; i < 5; i++)
	{
		Sprite * sprite = new Sprite(BOSS1_TEXTURE_LOCATION, listSprite[i], TEXTURE_TRANS_COLOR);
		anim->AddFrame(sprite);
	}
	animations.push_back(anim);

	// BOSS1_ANI_BLEEDING
	anim = new Animation(100);
	for (int i = 5; i < 6; i++)
	{
		Sprite * sprite = new Sprite(BOSS1_TEXTURE_LOCATION, listSprite[i], TEXTURE_TRANS_COLOR);
		anim->AddFrame(sprite);
	}
	animations.push_back(anim);

	// BOSS1_ANI_DEAD
	anim = new Animation(300);
	for (int i = 6; i < 8; i++)
	{
		Sprite * sprite = new Sprite(BOSS1_TEXTURE_LOCATION, listSprite[i], TEXTURE_TRANS_COLOR);
		anim->AddFrame(sprite);
	}
	animations.push_back(anim);

	// BOSS1_ANI_STANDING_SHOOT_1
	anim = new Animation(100);
	for (int i = 8; i < 12; i++)
	{
		Sprite * sprite = new Sprite(BOSS1_TEXTURE_LOCATION, listSprite[i], TEXTURE_TRANS_COLOR);
		switch (i)
		{
		case 9:
			sprite->SetOffSetX(14);
			break;
		case 10:
			sprite->SetOffSetX(16);
			break;
		case 11:
			sprite->SetOffSetX(8);
			break;
		}
		anim->AddFrame(sprite,100);
	}
	animations.push_back(anim);

	// BOSS1_ANI_STANDING_SHOOT_2
	anim = new Animation(50);
	Sprite * sprite1 = new Sprite(BOSS1_TEXTURE_LOCATION, listSprite[8], TEXTURE_TRANS_COLOR);
	anim->AddFrame(sprite1);
	for (int i = 12; i < 14; i++)
	{
		Sprite * sprite = new Sprite(BOSS1_TEXTURE_LOCATION, listSprite[i], TEXTURE_TRANS_COLOR);
		switch (i)
		{
		case 12:
			sprite->SetOffSetX(13);
			break;
		case 13:
			sprite->SetOffSetX(5);
			break;
		}
		anim->AddFrame(sprite,250);
	}
	animations.push_back(anim);

	// BOSS1_ANI_FLYING
	anim = new Animation(100);
	for (int i = 14; i < 15; i++)
	{
		Sprite * sprite = new Sprite(BOSS1_TEXTURE_LOCATION, listSprite[i], TEXTURE_TRANS_COLOR);
		anim->AddFrame(sprite);
	}
	animations.push_back(anim);

	// BOSS1_ANI_FLYING_SHOOT
	anim = new Animation(100);
	for (int i = 15; i < 18; i++)
	{
		Sprite * sprite = new Sprite(BOSS1_TEXTURE_LOCATION, listSprite[i], TEXTURE_TRANS_COLOR);
		switch (i)
		{
		case 16:
			sprite->SetOffSetY(-4);
			sprite->SetOffSetX(-1);
			break;
		case 17:
			sprite->SetOffSetY(-4);
			break;
		}
		anim->AddFrame(sprite);
	}
	animations.push_back(anim);
}

void Boss1::Update(DWORD dt)
{
	if (this->disable)
		return;

	// Collide with brick
	vector<ColliedEvent*> coEvents;
	vector<ColliedEvent*> coEventsResult;

	vector<Tile2 *> tiles = Grid2::GetInstance()->GetNearbyTiles(this->GetRect());

	coEvents.clear();
	this->SetDt(dt);
	this->UpdateObjectCollider();
	this->MapCollisions(tiles, coEvents);

	if (coEvents.size() == 0)
	{
		float moveX = trunc(this->GetSpeedX()* dt);
		float moveY = trunc(this->GetSpeedY()* dt);
		this->SetPositionX(this->GetPositionX() + moveX);
		this->SetPositionY(this->GetPositionY() + moveY);
		this->isGrounded = false;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;

		Collision::GetInstance()->GetNearestCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);

		float moveX = min_tx * this->GetSpeedX() * dt + nx * 0.4;
		float moveY = min_ty * this->GetSpeedY() * dt + ny * 0.4;

		this->SetPositionX(this->GetPositionX() + moveX);
		this->SetPositionY(this->GetPositionY() + moveY);

		if (coEventsResult[0]->collisionID == 1)
		{
			if (ny == 1)
			{
				this->isGrounded = true;
			}
		}
	}
	for (UINT i = 0; i < coEvents.size(); i++)
		delete coEvents[i];

	//this->UpdateObjectCollider();

	state->Colision();
	state->Update(dt);
}
void Boss1::Render()
{
	if (this->disable)
		return;
	
	state->Render();
}

void Boss1::OnCollision()
{
	if (((Boss1State*)state)->GetState() == StateBoss1::BOSS1_STATE_BLEEDING
		|| ((Boss1State*)state)->GetState() == StateBoss1::BOSS1_STATE_DEAD)
		return;

	this->HP -= 1;
	((Boss1State*)state)->timeCount = 0;
	
	if (this->HP > 0)
	{
		((Boss1State*)state)->SetState(BOSS1_STATE_BLEEDING);
		return;
	}
	if (this->HP <= 0)
	{
		((Boss1State*)state)->deadTimeCount = 0;
		((Boss1State*)state)->SetState(BOSS1_STATE_DEAD);
		return;
	}
}