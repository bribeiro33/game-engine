//
//  KeyInput.hpp
//  game_engine
//
//  Created by Barbara Ribeiro on 2/13/24.
//

#ifndef KeyInput_hpp
#define KeyInput_hpp

#include <stdio.h>
#include <unordered_map>
#include <vector>
#ifdef __APPLE__
    #include "SDL2/SDL.h"
#else
    #include "SDL.h"
#endif

enum KEY_STATE {
    KEY_STATE_UP,
    KEY_STATE_JUST_BECAME_DOWN,
    KEY_STATE_DOWN,
    KEY_STATE_JUST_BECAME_UP
};

class KeyInput{
public:
    static void Init();
    static bool GetKey(SDL_Scancode keycode);
    static bool GetKeyDown(SDL_Scancode keycode);
    static bool GetKeyUp(SDL_Scancode keycode);
    static void ProcessEvent(const SDL_Event& e);
    static void LateUpdate(); 
private:
    static inline std::unordered_map<SDL_Scancode, KEY_STATE> keyboard_states;
    static inline std::vector<SDL_Scancode> just_became_down_scancodes;
    static inline std::vector<SDL_Scancode> just_became_up_scancodes;
};
#endif /* KeyInput_hpp */
