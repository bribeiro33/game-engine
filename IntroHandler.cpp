//
//  IntroHandler.cpp
//  game_engine
//
//  Created by Barbara Ribeiro on 2/9/24.
//

#include "IntroHandler.hpp"
#include "Renderer.hpp"
#include <iostream>
#include "Engine.hpp"

IntroHandler::IntroHandler(Renderer* renderer)
    : renderer(renderer) {
}

void IntroHandler::ProcessEvent(const SDL_Event& e) {
    // Check for left click event
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        currIntroIndex++;
    }
    // Check for spacebar or enter event
    else if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.scancode == SDL_SCANCODE_SPACE || e.key.keysym.scancode == SDL_SCANCODE_RETURN) {
            currIntroIndex++;
        }
    }
}

void IntroHandler::Render() {
    if (!done){
        if (!introImageNames.empty()){
            if (introImageNames.size() > currIntroIndex){
                renderer->RenderImage(introImageNames[currIntroIndex]);
            }
            // If have gone through all, keep displaying the last one to wait for text to catch up
            else {
                renderer->RenderImage(introImageNames[introImageNames.size() - 1]);
            }
        }
        if (!introTexts.empty()){
            // Same thing for texts
            if (introTexts.size() > currIntroIndex){
                // render text at (25, height - 50)
                renderer->RenderText(introTexts[currIntroIndex], 25, renderer->getHeight() - 50);
            }
            else {
                renderer->RenderText(introTexts[introTexts.size() - 1], 25, renderer->getHeight() - 50);
            }
        }
    }
}

void IntroHandler::setIntroImageNames(std::vector<std::string> in_imageNames){
    introImageNames = in_imageNames;
    if (introImageNames.empty()){
        done = true;
    }
}

void IntroHandler::setIntroText(std::vector<std::string> in_texts){
    introTexts = in_texts;
}


bool IntroHandler::IsDone(){
    // check to see if need to update to true if have gone through all images and texts or if have gone through all images and there are no texts
    if ((currIntroIndex >= introImageNames.size() && currIntroIndex >= introTexts.size()) || (currIntroIndex >= introImageNames.size() && introTexts.empty())) {
        done = true; // done with intro sequence
        // also halt background music
    }
    return done;
}
