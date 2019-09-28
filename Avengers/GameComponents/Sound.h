#pragma once

#include "dsutil.h"

class Sound
{
protected:
	HWND hwnd;
	GSoundManager *dsound;
	static Sound *__instance;
public:
	~Sound();

	bool Init_DirectSound(HWND hwnd);
	GSound *LoadSound(LPTSTR);
	void PlaySound(GSound *);
	void LoopSound(GSound *);
	void StopSound(GSound *);
	static Sound *GetInstance();
};

