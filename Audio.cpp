//
//  Audio.cpp
//  game_engine
//
//  Created by Barbara Ribeiro on 2/11/24.
//

#include "Audio.hpp"
#include <iostream>
void Audio::allocateChannels(int num){
    AudioHelper::Mix_AllocateChannels498(num);
}

void Audio::loadAudio(const std::string& musicName, Mix_Chunk*& chunk){
    std::string basePath = "resources/audio/";
    std::vector<std::string> extensions = {".wav", ".ogg"};
    // try to load the audio file with supported extensions
    for (const auto& ext : extensions) {
        std::string fullPath = basePath + musicName + ext;
        if (std::filesystem::exists(fullPath)) {
            chunk = AudioHelper::Mix_LoadWAV498(fullPath.c_str());
            if (chunk) {
                // cache the loaded audio
                loaded_audio[musicName] = chunk;
                break; // exit the loop once the file is successfully loaded (won't have file w/ same name but diff ext)
            }
        }
    }
}

void Audio::playMusic(const std::string& musicName, int channel, int numPlays){
    std::string basePath = "resources/audio/";
    std::vector<std::string> extensions = {".wav", ".ogg"};
    Mix_Chunk* chunk = nullptr;

    // check if the audio is already loaded
    auto it = loaded_audio.find(musicName);
    if (it != loaded_audio.end()) {
        // audio is already loaded, get the chunk
        chunk = it->second;
    }
    else {
        loadAudio(musicName, chunk);
    }

    if (chunk == nullptr) {
        std::cout << "error: failed to play audio clip " << musicName;
        exit(0);
    }
    else {
        // Play the audio on channel num, loop indefinitely (-1)
        AudioHelper::Mix_PlayChannel498(channel, chunk, numPlays);
    }
}

void Audio::haltMusic(int channel){
    //if (Mix_Playing(channel)){
        AudioHelper::Mix_HaltChannel498(channel);
    //}
}
