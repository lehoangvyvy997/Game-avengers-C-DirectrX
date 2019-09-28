#include "Exit.h"

vector<Animation *> Exit::animations = vector<Animation *>();
Exit *Exit::__instance = NULL;


Exit::Exit()
{	
	this->x = Viewport::GetInstance()->GetPosX() + 48;
	this->y = Viewport::GetInstance()->GetPosY() - 16;

	this->state = ExitTableState::EXIT_TABLE_HIDE;
}

Exit *Exit::GetInstance()
{
	if (__instance == NULL)
		__instance = new Exit();
	return __instance;
}

void Exit::LoadResources()
{
	LoadTXT loadTXT;
	RECT* listSprite = loadTXT.LoadRect((char*)"Resources\\Captain\\Captain.txt");

	//HIDE
	Animation * anim = new Animation(100);
	for (int i = 66; i < 67; i++)
	{
		Sprite * sprite = new Sprite(CAPTAIN_TEXTURE_LOCATION, listSprite[i], TEXTURE_TRANS_COLOR);
		anim->AddFrame(sprite);
	}
	animations.push_back(anim);

	//APPEAR
	anim = new Animation(300);
	for (int i = 65; i < 67; i++)
	{
		Sprite * sprite = new Sprite(CAPTAIN_TEXTURE_LOCATION, listSprite[i], TEXTURE_TRANS_COLOR);
		anim->AddFrame(sprite);
	}
	animations.push_back(anim);
}

void Exit::Update(DWORD dt)
{
	if (Captain::GetInstance()->canGoToNextStage && (STAGE_1 == Game::GetInstance()->GetStage() || STAGE_2 == Game::GetInstance()->GetStage()))
		this->state = ExitTableState::EXIT_TABLE_APPEAR;
	else 
		this->state = ExitTableState::EXIT_TABLE_HIDE;
	
	this->x = Viewport::GetInstance()->GetPosX() + 48;
	this->y = Viewport::GetInstance()->GetPosY() - 16;
}
void Exit::Render()
{
	if (this->state == ExitTableState::EXIT_TABLE_HIDE)
		return;
	
	SpriteData spriteData;

	spriteData.width = this->width;
	spriteData.height = this->height;
	spriteData.x = this->GetPositionX();
	spriteData.y = this->GetPositionY();

	spriteData.scale = 1;
	spriteData.angle = 0;

	this->animations[EXIT_TABLE_APPEAR]->Render(spriteData);
}

