#ifndef MUSICHANDLER_H
#define MUSICHANDLER_H



#include "MusicHandler.h"


std::map<std::string,  Mix_Music*> MusicHandler::sounds;

void MusicHandler::init()
{
	if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1)
	{
		// ERROR
	}
	Mix_AllocateChannels(16);
}

void MusicHandler::play(std::string name) {
	Mix_PlayMusic(sounds[name],-1);
}

void MusicHandler::loadSound(const char* file, std::string name){
	sounds[name] = Mix_LoadMUS(file);
}


#endif // RESOURCEMANAGER_H