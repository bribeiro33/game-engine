//
//  KeyInput.cpp
//  game_engine
//
//  Created by Barbara Ribeiro on 2/13/24.
//

#include "KeyInput.hpp"

void KeyInput::Init(){
    // all keys begin in the up state
    for (int code = SDL_SCANCODE_UNKNOWN; code < SDL_NUM_SCANCODES; ++code){
        keyboard_states[static_cast<SDL_Scancode>(code)] = KEY_STATE_UP;
    }
}

void KeyInput::ProcessEvent(const SDL_Event& e){
    if (e.type == SDL_KEYDOWN){
        keyboard_states[e.key.keysym.scancode] = KEY_STATE_JUST_BECAME_DOWN;
        just_became_down_scancodes.push_back(e.key.keysym.scancode);
        
    }
    else if (e.type == SDL_KEYUP){
        keyboard_states[e.key.keysym.scancode] = KEY_STATE_JUST_BECAME_UP;
        just_became_up_scancodes.push_back(e.key.keysym.scancode);
    }
}

void KeyInput::LateUpdate(){
    for (const SDL_Scancode& code : just_became_down_scancodes){
        keyboard_states[code] = KEY_STATE_DOWN;
    }
    just_became_down_scancodes.clear();
    
    for (const SDL_Scancode& code : just_became_up_scancodes){
        keyboard_states[code] = KEY_STATE_UP;
    }
    just_became_up_scancodes.clear();
}

bool KeyInput::GetKey(SDL_Scancode keycode) {
    auto it = keyboard_states.find(keycode);
    if (it != keyboard_states.end()) {
        return it->second == KEY_STATE_DOWN || it->second == KEY_STATE_JUST_BECAME_DOWN;
    }
    return false;
}

bool KeyInput::GetKeyDown(SDL_Scancode keycode) {
    auto it = keyboard_states.find(keycode);
    if (it != keyboard_states.end()) {
        return it->second == KEY_STATE_JUST_BECAME_DOWN;
    }
    return false;
}

bool KeyInput::GetKeyUp(SDL_Scancode keycode) {
    auto it = keyboard_states.find(keycode);
    if (it != keyboard_states.end()) {
        return it->second == KEY_STATE_JUST_BECAME_UP;
    }
    return false;
}

