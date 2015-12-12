//
//  Music.h
//  echoecho
//
//  Created by Martin on 14/11/24.
//  Copyright (c) 2014? Martin. All rights reserved.
//

#ifndef __echoecho__Music__
#define __echoecho__Music__

#include <stdio.h>
#include <stdio.h>
#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>
typedef unsigned int DWORD;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef int LONG;

class SoundManager {
public:
	void init();
	void playMainMenuMusic();
	void playThemeMusic();
	void playHoverSound();
	void playClickSound();
	void playStartSound();
	void playScaredSound();
	void stopBreathFastSound();
	void playBreathFastSound();
	void stopMainMenuMusic();
	void stopThemeMusic();

	int loadWavFile(char* filePath);
	int endWithError(char* msg, int error = 0);
	void loadAllSoundFiles();
	void playSound(int source, float pitch, float gain, ALboolean loop);
	void setListenerPosition(ALfloat* pos);
	void setListenerVelocity(ALfloat* velocitiy);
	void setSourcePosition(int source, ALfloat* pos);
	void setSourceVelocity(int source, ALfloat* velocitiy);
	void clean();
};
#endif /* defined(__echoecho__Music__) */