﻿#include "Boss1State.h"
#include <math.h>

Boss1State * Boss1State::__instance = NULL;

Boss1State *Boss1State::GetInstance(Boss1 *boss1)
{
	if (__instance == NULL)
		__instance = new Boss1State(boss1);
	return __instance;
}


Boss1State::Boss1State(Boss1 *boss1)
{
	this->boss1 = boss1;
	this->state_standing_shoot_1();
	this->behaviorBoss1 = BehaviorBoss1::ComboShoot;
	reStateChance = 0;
}

Boss1State::~Boss1State()
{
	delete anim;
}

//Lấy trạng thái
StateBoss1 Boss1State::GetState()
{
	return this->stateBoss1;
}
//Set trạng thái
void Boss1State::SetState(StateBoss1 state)
{
	this->stateBoss1 = state;
}

#pragma region  State

void Boss1State::state_idle()
{
	this->SetState(BOSS1_STATE_IDLE);
	anim = boss1->GetAnimationsList()[BOSS1_STATE_IDLE];
	boss1->SetSpeedX(0);
	boss1->SetSpeedY(0);
}

void Boss1State::state_running()
{
	this->SetState(BOSS1_STATE_RUNNING);
	anim = boss1->GetAnimationsList()[BOSS1_STATE_RUNNING];
	boss1->SetSpeedX(CAPTAIN_WALK_SPEED * (boss1->IsLeft() ? -1 : 1));
}

void Boss1State::state_bleeding()
{
	boss1->SetSpeedX(CAPTAIN_WALK_SPEED * (boss1->IsLeft() ? 1 : -1));
	boss1->SetSpeedY(boss1->GetSpeedY() - CAPTAIN_GRAVITY);

	if (boss1->isGrounded && timeCount >= 200)
	{
		this->state_idle();
		ChangeBossDirection();
		boss1->SetSpeedX(0);
		boss1->SetSpeedY(0);
		behaviorState = 0;
		RandomNextState();
		return;
	}

	/*if (timeCount >= 200)
	{
		this->state_idle();
		return;
	}*/

	this->SetState(BOSS1_STATE_BLEEDING);
	anim = boss1->GetAnimationsList()[BOSS1_STATE_BLEEDING];
}

void Boss1State::state_dead()
{
	this->SetState(BOSS1_STATE_DEAD);
	anim = boss1->GetAnimationsList()[BOSS1_STATE_DEAD];
	boss1->SetSpeedX(0);
	boss1->SetSpeedY(0);
	boss1->SetPositionY(72);

	if (this->deadTimeCount > 2000)
	{
		boss1->disable = true;
	}
}

void Boss1State::state_standing_shoot_1(int type)
{
	this->SetState(BOSS1_STATE_STANDING_SHOOT_1);
	anim = boss1->GetAnimationsList()[BOSS1_STATE_STANDING_SHOOT_1];
	boss1->SetSpeedX(0);

	if (this->shootTimeCount > BOSS1_TIME_STANDING_SHOOT_1)
	{
		this->shootTimeCount = 0;
		if (sound_shoot != NULL)
		{
			delete sound_shoot;
		}
		sound_shoot = Sound::GetInstance()->LoadSound((LPTSTR)SOUND_BOSS1_LAZE);
		Sound::GetInstance()->PlaySound(sound_shoot);
		int direction;
		if (type == 2)
		{
			direction = boss1->IsLeft() ? 1 : 5;
		}
		else
		{
			if (abs(Captain::GetInstance()->GetPositionY() - boss1->GetPositionY()) < 20)
				direction = boss1->IsLeft() ? 1 : 5;
			else
				direction = boss1->IsLeft() ? 8 : 6;
		}
		float offsetX = boss1->IsLeft() ? -8 : 20;
		float offsetY = -14;
		if (type == 1)
			SpawnProjectTile::GetInstance()->SpawnBullet(boss1->GetPositionX() + offsetX, boss1->GetPositionY() + offsetY,
				direction, BulletType::BULLET_NORMAL_BOSS1);
		else
			SpawnProjectTile::GetInstance()->SpawnBullet(boss1->GetPositionX() + offsetX, boss1->GetPositionY() + offsetY,
				direction, BulletType::BULLET_SPECIAL_BOSS1);
	}
}

void Boss1State::state_standing_shoot_2()
{
	this->SetState(BOSS1_STATE_STANDING_SHOOT_2);
	anim = boss1->GetAnimationsList()[BOSS1_STATE_STANDING_SHOOT_2];
	boss1->SetSpeedX(0);

	if (this->shootTimeCount > BOSS1_TIME_STANDING_SHOOT_1 + BOSS1_TIME_STANDING_SHOOT_2 * 2)
	{
		this->shootTimeCount = 0;
		if (sound_shoot != NULL)
		{
			delete sound_shoot;
		}
		sound_shoot = Sound::GetInstance()->LoadSound((LPTSTR)SOUND_BOSS1_FLYING);
		Sound::GetInstance()->PlaySound(sound_shoot);
		int direction = boss1->IsLeft() ? 1 : 5;
		float offsetX = boss1->IsLeft() ? -16 : 16;
		float offsetY = -18;
		SpawnProjectTile::GetInstance()->SpawnBullet(boss1->GetPositionX() + offsetX, boss1->GetPositionY() + offsetY,
			direction, BulletType::BULLET_SPECIAL_BOSS1);
	}
}

void Boss1State::state_flying()
{
	this->SetState(BOSS1_STATE_FLYING);
	anim = boss1->GetAnimationsList()[BOSS1_STATE_FLYING];
}

void Boss1State::state_flying_shoot()
{
	this->SetState(BOSS1_STATE_FLYING_SHOOT);
	anim = boss1->GetAnimationsList()[BOSS1_STATE_FLYING_SHOOT];

	if (this->shootTimeCount > BOSS1_TIME_FLY_UP_N_DOWN)
	{
		this->shootTimeCount = 0;
		if (sound_shoot != NULL)
		{
			delete sound_shoot;
		}
		sound_shoot = Sound::GetInstance()->LoadSound((LPTSTR)SOUND_BOSS1_FLYING);
		Sound::GetInstance()->PlaySound(sound_shoot);
		int direction = 3;
		float offsetX = boss1->IsLeft() ? -8 : 30;
		float offsetY = -10;
		SpawnProjectTile::GetInstance()->SpawnBullet(boss1->GetPositionX() + offsetX, boss1->GetPositionY() + offsetY,
			direction, BulletType::BULLET_SPECIAL_BOSS1);
	}
}

#pragma endregion

void Boss1State::Behavior_Shoot()
{
	switch (behaviorState)
	{
	case 0:
		this->state_standing_shoot_2();
		this->NextStateIn(BOSS1_TIME_STANDING_SHOOT_2);
		break;
	case 1:
		ChangeBossDirection();
		behaviorState = 0;
		RandomNextState();
		break;
	}
}

void Boss1State::Behavior_ComboShoot()
{	
	srand(time(NULL));
	int type = rand() % 2 + 1;
	switch (behaviorState)
	{
	case 0:
		this->state_running();	
		this->NextStateIn(BOSS1_TIME_RUN);
		this->shootTimeCount = 300;
		break;
	case 1:
		this->state_standing_shoot_1();
		if (this->NextStateIn(BOSS1_TIME_STANDING_SHOOT_1))
			this->shootTimeCount = 300;
		break;
	case 2:
		this->state_standing_shoot_1();
		if (this->NextStateIn(BOSS1_TIME_STANDING_SHOOT_1))
			this->shootTimeCount = 300;
		break;
	case 3:
		this->state_standing_shoot_1(type);
		if (this->NextStateIn(BOSS1_TIME_STANDING_SHOOT_1))
			this->shootTimeCount = 0;
		break;
	default:
		ChangeBossDirection();
		behaviorState = 0;
		RandomNextState();
		break;
	}
}

void Boss1State::Behavior_FlyAndShoot()
{
	// Behavior Flow
	// 0->	1->	|->	2->	3->	End
	//			|->	4->	|->	3->	End
	//					|-> 1->	3->	End
	switch (behaviorState)
	{
	case 0:
		this->state_flying();
		boss1->SetSpeedX(0);
		boss1->SetSpeedY(BOSS1_FLY_SPEED);

		if (boss1->GetPositionY() >= 185)
		{
			this->behaviorState++;
		}

		break;
	case 1:
		this->state_flying();
		boss1->SetSpeedX(CAPTAIN_WALK_SPEED * (boss1->IsLeft() ? -1 : 1));
		boss1->SetSpeedY(0);

		if (abs(boss1->GetPositionX() - Captain::GetInstance()->GetPositionX()) < 16)
		{
			behaviorState = 4;
		}

		if (!boss1->IsLeft() && boss1->GetPositionX() >= 220)
		{
			this->behaviorState++;
		}
		else if (boss1->IsLeft() && boss1->GetPositionX() <= 10)
		{
			this->behaviorState++;
		}

		break;
	case 2:	// Bay xuong
		this->state_flying();
		boss1->SetSpeedX(0);
		boss1->SetSpeedY(-BOSS1_FLY_SPEED);

		//if (boss1->GetPositionY() <= 72)
		if (boss1->isGrounded)
		{
			//boss1->SetPositionY(72);
			this->behaviorState++;
		}
		break;
	case 3:
		ChangeBossDirection();
		boss1->SetSpeedX(0);
		boss1->SetSpeedY(0);
		behaviorState = 0;
		RandomNextState();
		break;
	case 4:
		this->state_flying_shoot();

		boss1->SetSpeedX(CAPTAIN_WALK_SPEED * (boss1->IsLeft() ? -1 : 1));
		boss1->SetSpeedY(0);

		if (anim->IsDone())
		{
			behaviorState = 1;
		}

		if (!boss1->IsLeft() && boss1->GetPositionX() >= Viewport::GetInstance()->GetRect().right - 30)
		{
			behaviorState = 2;
		}
		else if (boss1->IsLeft() && boss1->GetPositionX() <= Viewport::GetInstance()->GetRect().left + 10)
		{
			behaviorState = 2;
		}
		break;
	}
}

void Boss1State::Behavior_Fly()
{
	float walkSpeed = (0.125f * (boss1->IsLeft() ? -1 : 1)) /2;
	float acceleration = (timeCount / BOSS1_TIME_FLY_UP_N_DOWN) * (BOSS1_FLY_SPEED / 2);
	float acceleration2 = (timeCount / BOSS1_TIME_FLY_UP_N_DOWN);
	float posX;
	float posY;

	switch (behaviorState)
	{
	case 0:
		this->state_flying();
		a = 0.1;
		if (!boss1->IsLeft())
			h = (BOSS1_PARABOL_JUMP_H + boss1->GetPositionX());
		else
			h = (boss1->GetPositionX() - BOSS1_PARABOL_JUMP_H);

		k = BOSS1_PARABOL_JUMP_K + boss1->GetPositionY();

		if (h + BOSS1_PARABOL_JUMP_H > Viewport::GetInstance()->GetRect().right ||
			h - BOSS1_PARABOL_JUMP_H < Viewport::GetInstance()->GetRect().left)
			this->behaviorState = 2;
		else
			this->behaviorState++;
		break;
	case 1:
		this->state_flying();
		posX = walkSpeed + boss1->GetPositionX();
		posY = -a * (posX - h) * (posX - h) + k;
		boss1->SetSpeedX(walkSpeed);
		boss1->SetPositionY(posY);

		if (posX > h + BOSS1_PARABOL_JUMP_H && !boss1->IsLeft())
			this->behaviorState++;
		else if (posX < h - BOSS1_PARABOL_JUMP_H && boss1->IsLeft())
			this->behaviorState++;
		break;
	/*case 2:
		posX = boss1->GetPositionX() - walkSpeed;
		posY = -a * (posX - h) * (posX - h) + k;

		boss1->SetSpeedX(-walkSpeed);
		boss1->SetPositionY(posY);

		if (posX < h - 32)
			this->behaviorState--;

		break;*/
	case 2:
		ChangeBossDirection();
		boss1->SetSpeedX(0);
		boss1->SetSpeedY(0);
		boss1->SetPositionY(72);
		behaviorState = 0;

		RandomNextState();
		break;
	}
}

void Boss1State::RandomNextState()
{
	if (this->GetState() == BOSS1_STATE_DEAD)
	{
		this->state_dead();
		return;
	}
	
	if (this->GetState() == BOSS1_STATE_BLEEDING)
	{
		this->state_bleeding();
		return;
	}
	
	srand(time(NULL));
	int randomState = rand() % 10 + 1;

	/*switch (randomState)
	{
	case 1:
		behaviorBoss1 = BehaviorBoss1::ComboShoot;
		break;
	case 2:
		behaviorBoss1 = BehaviorBoss1::Shoot;
		break;
	case 3:
		behaviorBoss1 = BehaviorBoss1::Fly;
		break;
	case 4: 
		behaviorBoss1 = BehaviorBoss1::FlyNShoot;
		break;
	}*/

	switch (behaviorBoss1)
	{
	case Shoot:
		if (randomState < reStateChance)
		{
			behaviorBoss1 = BehaviorBoss1::Shoot;
			reStateChance--;
		}
		else if (randomState <= 5)
		{
			behaviorBoss1 = BehaviorBoss1::ComboShoot;
			reStateChance = BOSS1_COMBOSHOOT_RESTATE_CHANCE;
		}
		else if (randomState <= 7)
		{
			behaviorBoss1 = BehaviorBoss1::Fly;
			reStateChance = BOSS1_FLY_RESTATE_CHANCE;
		}
		else if (randomState <= 10)
		{
			behaviorBoss1 = BehaviorBoss1::FlyNShoot;
			reStateChance = BOSS1_FLYNSHOOT_RESTATE_CHANCE;
		}
		break;
	case ComboShoot:
		if (randomState < reStateChance)
		{
			behaviorBoss1 = BehaviorBoss1::ComboShoot;
			reStateChance--;
		}
		else if (randomState <= 4)
		{
			behaviorBoss1 = BehaviorBoss1::Shoot;
			reStateChance = BOSS1_SHOOT_RESTATE_CHANCE;
		}
		else if (randomState <= 7)
		{
			behaviorBoss1 = BehaviorBoss1::Fly;
			reStateChance = BOSS1_FLY_RESTATE_CHANCE;
		}
		else if (randomState <= 10)
		{
			behaviorBoss1 = BehaviorBoss1::FlyNShoot;
			reStateChance = BOSS1_FLYNSHOOT_RESTATE_CHANCE;
		}
		break;
	case FlyNShoot:
		if (randomState < reStateChance)
		{
			behaviorBoss1 = BehaviorBoss1::FlyNShoot;
			reStateChance--;
		}
		else if (randomState <= 5)
		{
			behaviorBoss1 = BehaviorBoss1::ComboShoot;
			reStateChance = BOSS1_COMBOSHOOT_RESTATE_CHANCE;
		}
		else if (randomState <= 5)
		{
			behaviorBoss1 = BehaviorBoss1::Shoot;
			reStateChance = BOSS1_SHOOT_RESTATE_CHANCE;
		}
		else if (randomState <= 5)
		{
			behaviorBoss1 = BehaviorBoss1::Fly;
			reStateChance = BOSS1_FLY_RESTATE_CHANCE;
		}
		break;
	case Fly:
		if (randomState < reStateChance)
		{
			behaviorBoss1 = BehaviorBoss1::Fly;
			reStateChance--;
		}
		else if (randomState <= 5)
		{
			behaviorBoss1 = BehaviorBoss1::ComboShoot;
			reStateChance = BOSS1_COMBOSHOOT_RESTATE_CHANCE;
		}
		else if (randomState <= 10)
		{
			behaviorBoss1 = BehaviorBoss1::Shoot;
			reStateChance = BOSS1_SHOOT_RESTATE_CHANCE;
		}
		break;
	default:
		break;
	}
}

bool Boss1State::NextStateIn(float time)
{
	if (timeCount > time)
	{
		timeCount = 0;
		behaviorState++;
		return true;
	}
	return false;
}

bool Boss1State::SetStateIn(int behaviorState, float time)
{
	if (timeCount > time)
	{
		timeCount = 0;
		this->behaviorState = behaviorState;
		return true;
	}
	return false;
}

void Boss1State::ChangeBossDirection()
{
	if (Captain::GetInstance()->GetPositionX() < boss1->GetPositionX())
	{
		boss1->setIsLeft(true);
	}
	else
	{
		boss1->setIsLeft(false);
	}
}

void Boss1State::Colision()
{

}

void Boss1State::Update(DWORD dt)
{
	timeCount += dt;
	this->shootTimeCount += dt;
	this->deadTimeCount += dt;

	if (this->GetState() == BOSS1_STATE_DEAD)
	{
		this->state_dead();
		return;
	}

	if (this->GetState() == BOSS1_STATE_BLEEDING)
	{
		this->state_bleeding();
		return;
	}

	switch (this->behaviorBoss1)
	{
	case Shoot:
		this->Behavior_Shoot();
		break;
	case ComboShoot:
		this->Behavior_ComboShoot();
		break;
	case FlyNShoot:
		this->Behavior_FlyAndShoot();
		break;
	case Fly:
		this->Behavior_Fly();
		break;
	}

	//Update theo state
	/*switch (stateBoss1)
	{
	case BOSS1_STATE_IDLE:
		this->state_idle();
		break;

	case BOSS1_STATE_RUNNING:
		this->state_running();
		break;

	case BOSS1_STATE_BLEEDING:
		this->state_bleeding();
		break;

	case BOSS1_STATE_DEAD:
		this->state_dead();
		break;

	case BOSS1_STATE_STANDING_SHOOT_1:
		this->state_standing_shoot_1();
		break;

	case BOSS1_STATE_STANDING_SHOOT_2:
		this->state_standing_shoot_2();
		break;

	case BOSS1_STATE_FLYING:
		this->state_flying();
		break;

	case BOSS1_STATE_FLYING_SHOOT:
		this->state_flying_shoot();
		break;

	default:
		break;
	}*/
}

void Boss1State::Render()
{
	SpriteData spriteData;
	if (this->boss1 != NULL)
	{
		spriteData.width = BOSS1_SPRITE_WIDTH;
		spriteData.height = BOSS1_SPRITE_HEIGHT;
		spriteData.x = boss1->GetPositionX();
		spriteData.y = boss1->GetPositionY();
		spriteData.scale = 1;
		spriteData.angle = 0;
		spriteData.isLeft = boss1->IsLeft();
		spriteData.isFlipped = boss1->IsFlipped();
	}

	anim->Render(spriteData);
}

