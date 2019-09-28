#include "CaptainState.h"
#include <math.h>

CaptainState *CaptainState::__instance = NULL;

CaptainState *CaptainState::GetInstance(Captain *captain)
{
	if (__instance == NULL)
		__instance = new CaptainState(captain);

	return __instance;
}


CaptainState::CaptainState(Captain *captain)
{
	this->captain = captain;
	this->stateCaptain = STATE_JUMPING;
	this->startJumpY == NULL;
	this->startDash == NULL;
	this->time_delay = 0;
	this->isdelay = false;
	this->captain->SetSpeedY(-CAPTAIN_JUMP_SPEED_Y);
}

CaptainState::~CaptainState()
{
	delete anim;
	delete listSprite;
}

//Lấy trạng thái
StateCaptain CaptainState::GetState()
{
	if (this->stateCaptain == STATE_STANDING)
		int a = 0;
	return this->stateCaptain;
}
//Set trạng thái
void CaptainState::SetState(StateCaptain state)
{
	this->stateCaptain = state;

	//Nếu tồn tại thì xóa để dừng nhạc
	//Nhớ cho g = NULL ở trong getInstance nha bạn
	if (g != NULL)
	{
		//có thể stop ở đây nhưng delete luôn cho khỏi nặng ram
		//Sound::GetInstance()->StopSound(g);
		delete g;
		g = NULL;
	}
	switch (state)
	{
	case STATE_DASH:
		g = Sound::GetInstance()->LoadSound((LPTSTR)SOUND_CAPTAIN_DASH);
		break;
	case STATE_JUMPING_ROLE:
		g = Sound::GetInstance()->LoadSound((LPTSTR)SOUND_CAPTAIN_ROLE);
		break;
	case STATE_CROUCH_PUNCH:
	case STATE_PUNCH:
		g = Sound::GetInstance()->LoadSound((LPTSTR)SOUND_CAPTAIN_PUNCH);
		break;
	case STATE_JUMPING_KICK:
		g = Sound::GetInstance()->LoadSound((LPTSTR)SOUND_CAPTAIN_JUMPING_KICK);
		break;
	case STATE_DIEING:
		g = Sound::GetInstance()->LoadSound((LPTSTR)SOUND_CAPTAIN_DIEING);
		break;
	case STATE_SWIMMING:
		g = Sound::GetInstance()->LoadSound((LPTSTR)SOUND_CAPTAIN_SWIM);
		Sound::GetInstance()->PlaySound(g);
		return;
	default:
		return;
	}
	Sound::GetInstance()->LoopSound(g);
}

void CaptainState::state_standing()
{
	//Hàm KeyHandle nếu viết class hàm này chỉ set state mới
	sound_shield = NULL;
	if (Keyboard::GetInstance()->IsKeyDown(KEY_JUMP)) // Bàn phím ấn nút Z thì nhảy
	{
		startJumpY = captain->GetPositionY();
		captain->SetSpeedY(CAPTAIN_JUMP_SPEED_Y);
		this->SetState(STATE_JUMPING); //Chuyển sang trạng thái nhảy
		return;
	}

	if (Keyboard::GetInstance()->IsKeyDown(KEY_DASH) && startDash == NULL)
	{
		startDash = captain->GetPositionX();
		this->SetState(STATE_DASH);
		return;
	}

	if (Keyboard::GetInstance()->IsKeyDown(KEY_ATTACK)) //Tấn công 
	{
		if (captain->IsShield()) // Nếu có khiên
		{
			if (throw_shield != NULL)
			{
				delete throw_shield;
			}
			throw_shield = Sound::GetInstance()->LoadSound((LPTSTR)SOUND_CAPTAIN_THROW_SHIELD);
			Sound::GetInstance()->PlaySound(throw_shield);
			this->SetState(STATE_THROW_SHIELD); //Ném khiên
			this->state_throw_shield();
			captain->SetIsShield(false);
			return;
		}
		else
		{
			this->state_punch(); //Tạm thời thôi chứ hình như là phải xét bên state ném khiên nếu anim->IsDone() thì chuyển sang đấm
			return;
		}

	}

	if (Keyboard::GetInstance()->IsKeyDown(KEY_RIGHT) || Keyboard::GetInstance()->IsKeyDown(KEY_LEFT)) // Bàn phím nhấn nút trái phải
	{
		this->SetState(STATE_WALKING); //Chuyển sang trạng thái di chuyển
		return;//Phải có return để không làm câu lệnh dưới
	}


	if (Keyboard::GetInstance()->IsKeyDown(KEY_UP))
	{
		this->SetState(STATE_STANDING_UP); //Chuyển sang trạng thái nhìn lên trên
		return;
	}

	if (Keyboard::GetInstance()->IsKeyDown(KEY_DOWN))
	{
		this->SetState(STATE_CROUCH); //Chuyển sang trạng thái ngồi
		return;
	}

	//Hàm Update nếu viết class
		//Logic state Standing
	captain->SetSpeedX(0);
	captain->SetSpeedY(-CAPTAIN_JUMP_SPEED_Y);
	anim = captain->GetAnimationsList()[STATE_STANDING];

	//Hàm Colision sau khi kiểm tra va chạm tổng
		//Xử lý riêng;
}

void CaptainState::state_standing_up()
{
	//HandleKey
	if (Keyboard::GetInstance()->IsKeyDown(KEY_RIGHT) || Keyboard::GetInstance()->IsKeyDown(KEY_LEFT) ||
		Keyboard::GetInstance()->IsKeyDown(KEY_JUMP) || Keyboard::GetInstance()->IsKeyDown(KEY_ATTACK) || //Các state ưu tiên đưa ra trước
		!Keyboard::GetInstance()->IsKeyDown(KEY_UP)) //Không bấm lên thì đổi state
	{
		this->SetState(STATE_STANDING);
		return;
	}

	//Update state
	captain->SetSpeedX(0);
	captain->SetSpeedY(-CAPTAIN_JUMP_SPEED_Y);
	anim = captain->GetAnimationsList()[STATE_STANDING_UP];
}

void CaptainState::state_walking()
{
	captain->SetSpeedY(-CAPTAIN_JUMP_SPEED_Y);
	if (Keyboard::GetInstance()->IsKeyDown(KEY_RIGHT) || Keyboard::GetInstance()->IsKeyDown(KEY_LEFT)) //Ưu tiên qua lại nên để trước
	{
		if (Keyboard::GetInstance()->IsKeyDown(KEY_JUMP) || Keyboard::GetInstance()->IsKeyDown(KEY_ATTACK) || (Keyboard::GetInstance()->IsKeyDown(KEY_DASH) && startDash == NULL))//Đang đi mà nhảy, tấn công hay dash thì chuyển state
		{
			this->SetState(STATE_STANDING);
			return;
		}

		anim = captain->GetAnimationsList()[STATE_WALKING];
		return;
	}

	this->SetState(STATE_STANDING); //Không bấm gì hết
}

void CaptainState::state_jumping()
{
	//HandleKey
	if (!Keyboard::GetInstance()->IsKeyDown(KEY_JUMP))
	{
		captain->SetSpeedY(-CAPTAIN_JUMP_SPEED_Y);
	}

	if (Keyboard::GetInstance()->IsKeyDown(KEY_ATTACK) && !isdelay)//Đá
	{
		this->SetState(STATE_JUMPING_KICK);
		return;
	}

	//Update
	if (this->startJumpY != NULL && captain->GetPositionY() - this->startJumpY >= CAPTAIN_JUMP_MAX)//Nhảy đủ cao thì role
	{
		this->SetState(STATE_JUMPING_ROLE);
		this->state_jumping_role();
		return;
	}
	anim = captain->GetAnimationsList()[STATE_JUMPING];
}

void CaptainState::state_jumping_role()
{
	//HandleKey
	if (Keyboard::GetInstance()->IsKeyDown(KEY_DOWN) && captain->IsShield())//Ngồi lên khiên
	{
		this->state_crouch_shield();
		return;
	}

	if (Keyboard::GetInstance()->IsKeyDown(KEY_ATTACK) && !isdelay)//Đá
	{
		this->SetState(STATE_JUMPING_KICK);
		return;
	}
	//Update
	captain->SetSpeedY(captain->GetSpeedY() - CAPTAIN_GRAVITY);
	if (captain->GetPositionY() - this->startJumpY < CAPTAIN_JUMP_MAX)
	{
		captain->SetSpeedY(-CAPTAIN_JUMP_SPEED_Y);//Rơi xuống
		this->SetState(STATE_JUMPING);
		this->startJumpY == NULL;
	}
	anim = captain->GetAnimationsList()[STATE_JUMPING_ROLE];
}

void CaptainState::state_crouch()
{
	//HandleKey
	if (Keyboard::GetInstance()->IsKeyDown(KEY_JUMP))//bay xuống
	{
		if (STAGE_BOSS_1 == Game::GetInstance()->GetStage() || STAGE_BOSS_2 == Game::GetInstance()->GetStage())
			return;

		captain->SetPositionY(captain->GetPositionY() - 0.5);
		return;
	}

	if (Keyboard::GetInstance()->IsKeyDown(KEY_ATTACK))//ngồi đấm
	{
		this->SetState(STATE_CROUCH_PUNCH);
		return;
	}

	if (Keyboard::GetInstance()->IsKeyDown(KEY_RIGHT) || Keyboard::GetInstance()->IsKeyDown(KEY_LEFT) ||
		Keyboard::GetInstance()->IsKeyDown(KEY_JUMP) || //Các state ưu tiên đưa ra trước
		!Keyboard::GetInstance()->IsKeyDown(KEY_DOWN)) //Không bấm lên thì đổi state
	{
		this->SetState(STATE_STANDING);
		return;
	}

	//Update state
	captain->SetSpeedX(0);
	captain->SetSpeedY(-CAPTAIN_JUMP_SPEED_Y);
	anim = captain->GetAnimationsList()[STATE_CROUCH];
}

void CaptainState::state_swimming()
{
	if (Keyboard::GetInstance()->IsKeyDown(KEY_JUMP)) // Bàn phím ấn nút Z thì nhảy
	{
		Sound::GetInstance()->PlaySound(Sound::GetInstance()->LoadSound((LPTSTR)SOUND_CAPTAIN_SWIM));
		startJumpY = captain->GetPositionY();
		captain->SetSpeedY(CAPTAIN_JUMP_SPEED_Y);
		this->SetState(STATE_JUMPING); //Chuyển sang trạng thái nhảy
		captain->SetIsSwimming(false);
		return;
	}

	if (Keyboard::GetInstance()->IsKeyDown(KEY_DOWN) && !Keyboard::GetInstance()->IsKeyDown(KEY_JUMP)) // Lặn
	{
		this->state_diving();
		return;
	}

	anim = captain->GetAnimationsList()[STATE_SWIMMING];
}

void CaptainState::state_throw_shield()
{
	captain->SetSpeedX(0);
	captain->SetSpeedY(-CAPTAIN_JUMP_SPEED_Y);
	anim = captain->GetAnimationsList()[STATE_THROW_SHIELD];

	if (anim->IsDone())//Hàm isDone hình như bị lỗi
	{
		this->SetState(STATE_PUNCH); //Đấm
		return;
	}
}

void CaptainState::state_punch()
{
	if (Keyboard::GetInstance()->IsKeyDown(KEY_JUMP))
	{
		this->SetState(STATE_JUMPING);
		return;
	}

	//Update
	captain->SetSpeedX(0);
	captain->SetSpeedY(-CAPTAIN_JUMP_SPEED_Y);//Lúc nào nó cũng hướng xuống để xét va chạm đất
	anim = captain->GetAnimationsList()[STATE_PUNCH];
	if (anim->IsDone())
		this->SetState(STATE_STANDING);
}

void CaptainState::state_jumping_kick()
{
	//Update
	if (this->startJumpY != NULL && captain->GetPositionY() - this->startJumpY >= CAPTAIN_JUMP_MAX)//Nhảy đủ cao thì role
	{
		captain->SetSpeedY(-CAPTAIN_JUMP_SPEED_Y);
	}

	if (anim->IsDone())
	{
		this->SetState(STATE_JUMPING);
		this->time_delay = 200;
		this->isdelay = true;
		return;
	}
	anim = captain->GetAnimationsList()[STATE_JUMPING_KICK];//Cái này làm cho có thêm thời gian tấn công với event upKey vô
}
void CaptainState::state_crouch_punch()
{
	//Update
	captain->SetSpeedX(0);
	captain->SetSpeedY(-CAPTAIN_JUMP_SPEED_Y);
	anim = captain->GetAnimationsList()[STATE_CROUCH_PUNCH];

	if (anim->IsDone())
	{
		this->SetState(STATE_CROUCH);
	}
}

void CaptainState::state_crouch_shield()
{
	this->SetState(STATE_CROUCH_SHIELD);

	if (!Keyboard::GetInstance()->IsKeyDown(KEY_DOWN))
	{
		this->SetState(STATE_STANDING);
		this->state_standing();
		return;
	}

	captain->SetSpeedY(-CAPTAIN_JUMP_SPEED_Y);
	captain->SetSpeedX(0);
	anim = captain->GetAnimationsList()[STATE_CROUCH_SHIELD];
}

void CaptainState::state_dash()
{
	captain->SetSpeedX(CAPTAIN_WALK_SPEED * (captain->IsLeft() ? -4 : 4));

	if (startDash != NULL && abs(captain->GetPositionX() - startDash) >= 100)
	{
		this->SetState(STATE_STANDING);
		return;
	}

	if (Keyboard::GetInstance()->IsKeyUp(KEY_DASH)) // Nhả phím dash
	{
		startDash = NULL;
		this->SetState(STATE_STANDING); //Chuyển sang trạng thái đứng
		return;
	}

	this->SetState(STATE_DASH);
	anim = captain->GetAnimationsList()[STATE_DASH];
}

void CaptainState::state_throw_shield_2()
{
	this->SetState(STATE_THROW_SHIELD_2);
	anim = captain->GetAnimationsList()[STATE_THROW_SHIELD_2];
}

void CaptainState::state_bleeing()
{
	this->SetState(STATE_BLEEING);
	anim = captain->GetAnimationsList()[STATE_BLEEING];
	captain->SetSpeedX(CAPTAIN_WALK_SPEED * (captain->IsLeft() ? 1 : -1));
	captain->SetSpeedY(-CAPTAIN_JUMP_SPEED_Y);

	if (timeCount >= 100)
	{
		captain->SetSpeedX(0);
		this->state_bleeing_2();
		return;
	}
}

void CaptainState::state_dieing()
{
	this->SetState(STATE_DIEING);
	anim = captain->GetAnimationsList()[STATE_DIEING];

	if (timeCount >= 1000)
	{
		timeCount = 0;
		captain->Reset();
		this->SetState(STATE_STANDING);
		return;
	}

	captain->SetSpeedX(0);

}

void CaptainState::state_diving()
{
	captain->SetSpeedX(0);

	this->SetState(STATE_DIVING);
	anim = captain->GetAnimationsList()[STATE_DIVING];
}

void CaptainState::state_swing()
{
	if (Keyboard::GetInstance()->IsKeyDown(KEY_JUMP))
	{
		this->SetState(STATE_STANDING);
		this->state_standing();
		captain->SetIsSwing(false);
		return;
	}

	if (Keyboard::GetInstance()->IsKeyDown(KEY_DOWN))
	{
		this->SetState(STATE_JUMPING);
		captain->SetIsSwing(false);
		return;
	}

	if (captain->IsBleeding())
	{
		this->SetState(STATE_BLEEING);
		captain->SetIsSwing(false);
		return;
	}

	anim = captain->GetAnimationsList()[STATE_SWING];

	captain->SetSpeedX(0);
	captain->SetSpeedY(0);
}

void CaptainState::state_bleeing_2()
{
	this->SetState(STATE_BLEEING_2);
	captain->SetSpeedY(-CAPTAIN_JUMP_SPEED_Y);
	anim = captain->GetAnimationsList()[STATE_BLEEING_2];

	if (Keyboard::GetInstance()->IsKeyDown(KEY_JUMP) || Keyboard::GetInstance()->IsKeyDown(KEY_ATTACK))
	{
		captain->SetIsBleeding(false);
		this->SetState(STATE_STANDING);
		this->state_standing();
		return;
	}

	if (timeCount >= 300)
	{
		timeCount = 0;
		this->SetState(STATE_STANDING);
		captain->SetIsBleeding(false);
		return;
	}

	captain->SetSpeedX(0);
}

void CaptainState::KeyHandle()
{
	//Do viết hàm nên không có lệnh
}

void CaptainState::Colision()
{
	if (Keyboard::GetInstance()->IsKeyUp(KEY_DASH)) // Nhả phím dash
	{
		startDash = NULL;
	}

	//Không chạm đất thì rơi
	if (!captain->IsGrounded())
	{
		if (this->GetState() != STATE_JUMPING && this->GetState() != STATE_BLEEING && 
			this->GetState() != STATE_CROUCH_SHIELD && this->GetState() != STATE_SWIMMING 
			&& this->GetState() != STATE_DIVING && this->GetState() != STATE_JUMPING_KICK && this->GetState() != STATE_JUMPING_ROLE)
		{
			captain->SetSpeedY(-CAPTAIN_JUMP_SPEED_Y);
			this->SetState(STATE_JUMPING);
		}
	}
	else
	{
		if (this->GetState() == STATE_CROUCH_SHIELD)
		{
			if (sound_shield == NULL)
			{
				sound_shield = Sound::GetInstance()->LoadSound((LPTSTR)SOUND_CAPTAIN_CROUCH_SHIELD);
				Sound::GetInstance()->PlaySound(sound_shield);
			}
		}
		else if (this->GetState() == STATE_JUMPING || this->GetState() == STATE_JUMPING_ROLE || this->GetState() == STATE_JUMPING_KICK)
		{
			if (captain->GetSpeedY() < 0)
			{
				this->SetState(STATE_STANDING);
				this->state_standing();
			}
		}
	}
	//Nếu bị thương
	if (captain->IsBleeding())
	{
		this->SetState(STATE_BLEEING);
	}

	//Nếu chạm sông thì bơi
	if (captain->IsSwimming() && this->GetState() != STATE_SWIMMING)
	{
		this->SetState(STATE_SWIMMING);
	}

	//Đu dây
	if (captain->IsSwing() && this->GetState() != STATE_SWING)
	{
		this->SetState(STATE_SWING);
	}

	//Chết
	if (captain->isDead && this->GetState() != STATE_DIEING)
	{
		this->SetState(STATE_DIEING);
	}

	captain->SetIsGrounded(false);
}

void CaptainState::Update(DWORD dt)
{
	if (isdelay)
	{
		time_delay -= dt;
		if (time_delay < 0)
			isdelay = false;
	}
	timeCount += dt;
	//Kiểm tra tốc độ trục X
	if (Keyboard::GetInstance()->IsKeyDown(KEY_RIGHT))
	{
		captain->setIsLeft(false);
		if (this->GetState() == STATE_SWIMMING)
			captain->SetSpeedX(CAPTAIN_WALK_SPEED * (captain->IsLeft() ? -1.5 : 1.5));
		else
			captain->SetSpeedX(CAPTAIN_WALK_SPEED * (captain->IsLeft() ? -1 : 1));
	}
	else if (Keyboard::GetInstance()->IsKeyDown(KEY_LEFT))
	{
		captain->setIsLeft(true);
		if (this->GetState() == STATE_SWIMMING)
			captain->SetSpeedX(CAPTAIN_WALK_SPEED * (captain->IsLeft() ? -1.5 : 1.5));
		else
			captain->SetSpeedX(CAPTAIN_WALK_SPEED * (captain->IsLeft() ? -1 : 1));
	}
	else
		captain->SetSpeedX(0);

	if (captain->GetPositionX() < 0)
		captain->SetPositionX(0);

	switch (Game::GetInstance()->GetStage())
	{
	case STAGE_BOSS_1:
	case STAGE_BOSS_2:
		if (!captain->canGoToNextStage)
		{
			if (captain->GetPositionX() > 256 - 30)
				captain->SetPositionX(256 - 30);
		}
		break;
	case STAGE_2:
		if (!captain->canGoToNextStage)
		{
			if (captain->GetPositionX() > 1280 - 30)
				captain->SetPositionX(1280 - 30);

			if (captain->GetPositionX() < 256)
				captain->SetPositionX(256);
		}
		break;
	}

	//Update theo state
	switch (stateCaptain)
	{
	case STATE_STANDING:
		this->state_standing();
		break;

	case STATE_STANDING_UP:
		this->state_standing_up();
		break;

	case STATE_WALKING:
		this->state_walking();
		break;

	case STATE_JUMPING:
		this->state_jumping();
		break;

	case STATE_JUMPING_ROLE:
		this->state_jumping_role();
		break;

	case STATE_CROUCH:
		this->state_crouch();
		break;

	case STATE_SWIMMING:
		this->state_swimming();
		break;

	case STATE_THROW_SHIELD:
		this->state_throw_shield();
		break;

	case STATE_PUNCH:
		this->state_punch();
		break;

	case STATE_JUMPING_KICK:
		this->state_jumping_kick();
		break;

	case STATE_CROUCH_PUNCH:
		this->state_crouch_punch();
		break;

	case STATE_CROUCH_SHIELD:
		this->state_crouch_shield();
		break;

	case STATE_DASH:
		this->state_dash();
		break;

	case STATE_THROW_SHIELD_2:
		this->state_throw_shield_2();
		break;

	case STATE_BLEEING:
		this->state_bleeing();
		break;

	case STATE_DIEING:
		this->state_dieing();
		break;

	case STATE_DIVING:
		this->state_diving();
		break;

	case STATE_BLEEING_2:
		this->state_bleeing_2();
		break;

	case STATE_SWING:
		this->state_swing();
		break;

	default:
		break;
	}
}

void CaptainState::Render()
{
	SpriteData spriteData;
	if (this->captain != NULL)
	{
		spriteData.width = CAPTAIN_SPRITE_WIDTH;
		spriteData.height = CAPTAIN_SPRITE_HEIGHT;
		spriteData.x = captain->GetPositionX();
		spriteData.y = captain->GetPositionY();
		spriteData.scale = 1;
		spriteData.angle = 0;
		spriteData.isLeft = captain->IsLeft();
		spriteData.isFlipped = captain->IsFlipped();
	}

	anim->Render(spriteData);
}

