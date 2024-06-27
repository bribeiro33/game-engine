//
//  Engine.cpp
//  game_engine
//
//  Created by Barbara Ribeiro on 1/23/24.
//
#include "Engine.hpp"
#define CHANNEL_NUM 50

//Initialize
//while (game_running)
//  ProcessInput()
//  SDL_RenderClear(renderer)
//  if game_state is intro
//      RenderIntro()
//  else if game_state is Scene
//      UpdateActors()
//      RenderScene()
//      if (proceed_to_next_scene)
//          LoadScene(next_scene)
//          continue
//  else if game_state is Ending
//      RenderEnding()
//  SDL_RenderPresent
Engine::Engine() :  renderer(), gameState(GameState::NoState){}
// main function for Engine, runs the game loop on it
void Engine::game_loop() {
    IntroHandler intro = IntroHandler(&renderer);
    health = 3;
    score = 0;
    // Set up
    // get info from game and rendering, initial settings
    // load SDL libraries
    renderer.LoadLibraries();
    audio.allocateChannels(CHANNEL_NUM);
    // load config files
    //std::string currSceneStr = loadConfig();
    Input::checkResourcesDirectory();
    rapidjson::Document game_document;
    Input::ReadJsonFile("resources/game.config", game_document);
    Input::readGameFile(renderer, intro, *this, game_document);
    std::string introSongName = Input::getIntroSong(game_document);
    std::string mainSongName = Input::getMainSong(game_document);
    bool mainSongPlaying = false;
    //camera.configCamera();
    Input::readRenderFile(renderer, camera);
    SDL_RenderSetScale(renderer.getRendererSDL(), camera.getZoomFactor(), camera.getZoomFactor());
    // create window
    renderer.Initialize();
    // play intro music
    // -1 is play on loop
    std::string currSceneStr = Input::getInitialScene(game_document);
    Scene* currScene = loadScene(currSceneStr);
    camera.setInitialPosition(currScene->getPlayer()->getPosition());
    currScene->sortActorsByRenderOrder();
    std::string hp_img = Input::getHPImg(game_document);
    KeyInput::Init();
    // error if there's a player error but no hp img
    if (currScene->getPlayer() && hp_img == "" ){
        std::cout << "error: player actor requires an hp_image be defined";
        exit(0);
    }
    if (introSongName != ""){
        audio.playMusic(introSongName, 0, -1);
    }
    while (gameState != GameState::Exiting){
        SDL_Event next_event;
        while (Helper::SDL_PollEvent498(&next_event)) {
            //std::cout << next_event.type << std::endl;
            // if next_event is now filled with data about a new event
            KeyInput::ProcessEvent(next_event);
            if (next_event.type == SDL_QUIT) {
                changeGameState(GameState::Exiting);
            }
            switch (gameState) {
                case GameState::Intro:
                    intro.ProcessEvent(next_event);
                    break;
                case GameState::MainGame:
                    break;
                case GameState::Ending:
                    // Ending sequence event processing
                    break;
                default:
                    break;
            }
        }
        // Transition logic
        if (intro.IsDone() && gameState == GameState::Intro) {
            if (introSongName != ""){
                audio.haltMusic(0);
            }
            gameState = GameState::MainGame;
            // immediate actions upon transition, play new main music song
            if (mainSongName != "") {
                audio.playMusic(mainSongName, 0, -1);
            }
        }
        if (gameState == GameState::MainGame && !mainSongPlaying){
            // immediate actions upon transition, play new main music song
            if (mainSongName != "") {
                audio.playMusic(mainSongName, 0, -1);
            }
            mainSongPlaying = true;
            
        }
        renderer.StartFrame();
        
        switch (gameState) {
            case GameState::NoState:
                changeGameState(GameState::Intro);
            case GameState::Intro:
                intro.Render();
                break;
            case GameState::MainGame:
                render(*currScene, hp_img);
                if (proceedToNextScene){
                    // clear buffer and delete the old scene
                    renderer.StartFrame();
                    delete currScene;
                    
                    // determine scene exists and load it in
                    Input::findScene(nextSceneStr);
                    SDL_RenderSetScale(renderer.getRendererSDL(), camera.getZoomFactor(), camera.getZoomFactor());
                    currScene = loadScene(nextSceneStr);
                    render(*currScene, hp_img);
                    proceedToNextScene = false;
                    
                }
                if (gameOverBad){
                    //clear buffer
                    renderer.StartFrame();
                    changeGameState(GameState::Ending);
                    
                    if (badAudio != ""){
                        audio.haltMusic(0);
                        audio.playMusic(badAudio, 0, 0);
                    }
                    
                    // show image
                    if (badImage != ""){
                        endImage = badImage;
                    }
                    else {
                        exit(0); //end game
                    }
                    
                    renderer.RenderImage(endImage);
                }
                else if (gameOverGood){
                    //clear buffer
                    renderer.StartFrame();
                    changeGameState(GameState::Ending);
                    
                    if (goodAudio != ""){
                        audio.haltMusic(0);
                        audio.playMusic(goodAudio, 0, 0);
                    }
                    
                    // show image
                    if (goodImage != ""){
                        endImage = goodImage;
                    }
                    else {
                        exit(0); //end game
                    }
                    
                    renderer.RenderImage(endImage);
                }
                break;
            case GameState::Ending:
                renderer.RenderImage(endImage);
                break;
            case GameState::Exiting:
                if (previousState == GameState::Intro){
                    intro.Render();
                }
                else if (previousState == GameState::MainGame){
                    render(*currScene, hp_img);
                }
                else if (previousState == GameState::Ending){
                    renderer.RenderImage(endImage);
                }
                renderer.EndFrame();
                renderer.Cleanup();
                exit(0); // quit
                
        }
        renderer.EndFrame();
        //std::cout << "frame number: " << Helper::GetFrameNumber() << std::endl;
        KeyInput::LateUpdate();
    }
}

std::string Engine::loadConfig(IntroHandler& intro){
    Input::checkResourcesDirectory();
    rapidjson::Document game_document;
    Input::ReadJsonFile("resources/game.config", game_document);
    Input::readGameFile(renderer, intro, *this, game_document);
    Input::getIntroSong(game_document);
    return Input::getInitialScene(game_document);
}

Scene* Engine::loadScene(const std::string& sceneStr) {
    // create scene
    Scene* scene = new Scene(sceneStr);
    // get info about scene to populate actors
    Input::readScene(*scene, *this, renderer);
    // sort so they're in order for movement update
    //scene->sortMovingActors();
    return scene;
}

std::string Engine::input(){
    std::string userInput;
    // print out game state and ask for input
    ss << "health : " << health << ", score : " << score << '\n';
    ss << "Please make a decision...\n";
    ss << "Your options are \"n\", \"e\", \"s\", \"w\", \"quit\"\n";
    printOutput();

    std::cin >> userInput; // read user's line

    // if input is quit, quit program
    if (userInput == "quit") {
        printOutput();
        exit(0); // terminate the program
    }
    // else, return the input to be used in player update
    return userInput;
}

void Engine::render(Scene& curr_scene, std::string& hp_img) {
    Actor* player = curr_scene.getPlayer();
    glm::vec2 playerPos = glm::vec2(0.0, 0.0); // default if player doesn't exist
    curr_scene.updateActors(playerSpeed, audio, stepAudio);
    if (player){
        playerPos = player->getPosition();
        camera.update(playerPos, camera.getEaseFactor());
    }
    curr_scene.sortActorsByRenderOrder();
    if (!player){
        curr_scene.renderActors(renderer, camera);
    }
    if (player){
        // collect and render dialogues
        // contact/collision dialogues --> just collect and process
        const std::vector<std::pair<std::string, Actor*>> contactDialogues = curr_scene.getContactDialogues();
        for (int i = 0; i < contactDialogues.size(); ++i){
            processDialogueCommands(contactDialogues[i].first, *contactDialogues[i].second, player);
        }
        
        curr_scene.renderActors(renderer, camera);
        
        // nearby/trigger dialogues --> collect, process, and print
        const std::vector<std::pair<std::string, Actor*>> nearbyDialogues = curr_scene.getNearbyDialogues();
        int numDialogues = static_cast<int>(nearbyDialogues.size());
        for (int i = 0; i < numDialogues; ++i) {
            Actor* actor = nearbyDialogues[i].second;
            processDialogueCommands(nearbyDialogues[i].first, *actor, player);
            // if the current dialogue's actor has a dialogue sfx, play it once
            std::string dialogueSFX = actor->getNearbyDialogueSFX();
            if (dialogueSFX != "" && !actor->getPlayedDialogueSFX()){
                actor->setPlayedDialogueSFX(true);
                int channel_num = Helper::GetFrameNumber() % 48 + 2;
                audio.playMusic(dialogueSFX, channel_num, 0);
            }
            int posY = camera.getHeight() - 50 - (numDialogues - 1 - i) * 50;
            renderer.RenderText(nearbyDialogues[i].first, 25, posY);
        }
        curr_scene.clearDialogues();
        // render hud (score, lives)
        renderer.RenderHUD(hp_img, health, score);
    }
}

void Engine::processDialogueCommands(const std::string& dialogue, Actor& actor, Actor* player) {
    int currFrame = Helper::GetFrameNumber();
    if (dialogue.find("health down") != std::string::npos && currFrame >= lastHealthDownFrame + 180) {
        player->damaged();
        actor.attacked();
        health--;
        lastHealthDownFrame = currFrame;// update last health down frame to restart cooldown
        if (damageAudio != ""){
            // +2 to avoid clobbering channel 0 or 1
            int channel_num = Helper::GetFrameNumber() % 48 + 2;
            audio.playMusic(damageAudio, channel_num, 0);
        }
    }
    if ((dialogue.find("game over") != std::string::npos && currFrame >= lastHealthDownFrame + 180) || health <= 0) {
        gameOverBad = true;
    }
    else if (dialogue.find("score up") != std::string::npos && !actor.getScoreIncreased()) {
        score++;
        actor.setScoreIncreased(true);
        if (scoreAudio != ""){
            audio.playMusic(scoreAudio, 1, 0);
        }
    }
    else if (dialogue.find("you win") != std::string::npos) {
        gameOverGood = true;
    }
    else if (dialogue.find("proceed to") != std::string::npos) {
        // find scene name (proceed to X)
        std::string sceneName = Input::obtain_word_after_phrase(dialogue, "proceed to");
        proceedToNextScene = true;
        nextSceneStr = "resources/scenes/" + sceneName + ".scene";
    }
}

void Engine::printOutput(){
    // Convert the stringstream to a string
    std::string content = ss.str();

    // Print the modified content
    std::cout << content;
    ss.str("");
    ss.clear();
    return;
}

bool Engine::isInTemplates(const std::string& templateName) const {
    return templates.find(templateName) != templates.end();
}

void Engine::addTemplate(const std::string& templateName, const Template& newTemplate) {
    templates[templateName] = newTemplate;
}

Template& Engine::getTemplate(const std::string& templateName) {
    return templates[templateName];
}

void Engine::changeGameState(GameState newState){
    previousState = gameState;
    gameState = newState;
}

void Engine::setBadAudio(std::string badAudio_in){
    badAudio = badAudio_in;
}
void Engine::setGoodAudio(std::string goodAudio_in){
    goodAudio = goodAudio_in;
}

void Engine::setBadImage(std::string badImage_in){
    badImage = badImage_in;
}
void Engine::setGoodImage(std::string goodImage_in){
    goodImage = goodImage_in;
}

void Engine::setPlayerSpeed(double speed){
    playerSpeed = speed;
}

void Engine::setScoreSFX(std::string scoreAudio){
    this->scoreAudio = scoreAudio;
}
void Engine::setDamageSFX(std::string damageAudio){
    this->damageAudio = damageAudio;
}
void Engine::setStepSFX(std::string stepAudio){
    this->stepAudio = stepAudio;
}
