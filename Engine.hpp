//
//  Engine.hpp
//  game_engine
//
//  Created by Barbara Ribeiro on 1/23/24.
//

#ifndef Engine_hpp
#define Engine_hpp

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <unordered_map>
#ifdef __APPLE__
    #include "SDL2/SDL.h"
    #include "SDL2/SDL_events.h"
    #include "SDL2_image/SDL_image.h"
    #include "SDL2_ttf/SDL_ttf.h"
#else
    #include "SDL.h"
    #include "SDL_events.h"
    #include "SDL_image.h"
    #include "SDL_ttf.h"
#endif
#include "External/Helper.h"
#include "Actor.hpp"
#include "Audio.hpp"
#include "Camera.hpp"
#include "Input.hpp"
#include "IntroHandler.hpp"
#include "KeyInput.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Template.hpp"


#define IN_GAME_CELL_SIZE 100
class IntroHandler;
class Engine {
public:
    Engine();
    void game_loop();
    std::string input();
    std::string loadConfig(IntroHandler& intro);
    Scene* loadScene(const std::string& sceneStr);
    void render(Scene& curr_scene, std::string& hp_img);
    //void renderMap(Scene& curr_scene);
    void processDialogueCommands(const std::string& dialogue, Actor& actor, Actor* player);
    void printOutput();
    bool isInTemplates(const std::string& templateName) const;
    void addTemplate(const std::string& templateName, const Template& newTemplate);
    Template& getTemplate(const std::string& templateName);
    rapidjson::Document gameConfig;
    enum class GameState {
        NoState,
        Intro,
        MainGame,
        Ending,
        Exiting
    };
    void changeGameState(GameState newState);
    
    void setBadAudio(std::string badAudio);
    void setGoodAudio(std::string goodAudio);
    
    void setScoreSFX(std::string scoreAudio);
    void setDamageSFX(std::string damageAudio);
    void setStepSFX(std::string stepAudio);
    
    void setBadImage(std::string badImage);
    void setGoodImage(std::string goodImage);
    
    void setPlayerSpeed(double speed);
private:
    int health = 3;
    int score = 0;
    std::stringstream ss;
    Camera camera;
    Renderer renderer;
    Audio audio;
    std::unordered_map<std::string, Template> templates;
    bool proceedToNextScene = false;
    std::string nextSceneStr = "";
    bool gameOverBad = false;
    bool gameOverGood = false;
    GameState gameState = GameState::NoState;
    GameState previousState = GameState::NoState;
    int lastHealthDownFrame = -181; // to make sure can still lose health first (0) frame
    std::string badImage = "";
    std::string badAudio = "";
    std::string goodImage = "";
    std::string goodAudio = "";
    std::string endImage = "";
    double playerSpeed = 0.02;
    std::string scoreAudio = "";
    std::string damageAudio = "";
    std::string stepAudio = "";
};

#endif /* Engine_hpp */

