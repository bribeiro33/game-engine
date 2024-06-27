//
//  IntroHandler.hpp
//  game_engine
//
//  Created by Barbara Ribeiro on 2/9/24.
//

#ifndef IntroHandler_hpp
#define IntroHandler_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include "Camera.hpp"

#ifdef __APPLE__
    #include "SDL2/SDL.h"
    #include "SDL2_image/SDL_image.h"
    #include "SDL2_ttf/SDL_ttf.h"
    #include "SDL2_mixer/SDL_mixer.h"
#else
    #include "SDL.h"
    #include "SDL_image.h"
    #include "SDL_ttf.h"
    #include "SDL_mixer.h"
#endif

class Engine;
class Renderer;
class IntroHandler {
private:
    Renderer* renderer = nullptr;
    std::vector<std::string> introImageNames;
    std::vector<std::string> introTexts;
    Mix_Chunk* introSong = nullptr;
    
    size_t currIntroIndex = 0;
    bool done = false;

public:
    IntroHandler(Renderer* renderer);

    void ProcessEvent(const SDL_Event& e);
    void Render();
    void setIntroImageNames(std::vector<std::string> in_imageNames);
    void setIntroText(std::vector<std::string> in_texts);
    bool IsDone();
};


#endif /* IntroHandler_hpp */
