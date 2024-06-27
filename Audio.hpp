//
//  Audio.hpp
//  game_engine
//
//  Created by Barbara Ribeiro on 2/11/24.
//

#ifndef Audio_hpp
#define Audio_hpp

#include <stdio.h>
#include <unordered_map>
#include <string>
#include <iostream>
#include <filesystem>
#ifdef __APPLE__
    #include "SDL2/SDL.h"
    #include "SDL2_mixer/SDL_mixer.h"
#else
    #include "SDL.h"
    #include "SDL_mixer.h"
#endif
#include "External/AudioHelper.h"

class Audio {
private:
    std::unordered_map<std::string, Mix_Chunk*> loaded_audio;
public:
    void allocateChannels(int num);
    void loadAudio(const std::string& musicName, Mix_Chunk*& chunk);
    void playMusic(const std::string& musicName, int channel, int numPlays);
    void haltMusic(int channel);
};
#endif /* Audio_hpp */
