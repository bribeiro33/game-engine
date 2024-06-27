//
//  Engine.cpp
//  game_engine
//
//  Created by Barbara Ribeiro on 1/23/24.
//

#include "Engine.hpp"

// constructor
Engine::Engine() : camera(13, 9){}

// main function for Engine, runs the game loop on it
void Engine::game_loop() {
    health = 3;
    score = 0;
    
    ss << game_start_message << '\n';
    Actor& player = hardcoded_actors.back();
    Map curr_map = Map(HARDCODED_MAP_WIDTH, HARDCODED_MAP_HEIGHT);
    render(player.getPosition());
    while (true){
        ss << "health : " << health << ", score : " << score << '\n';
        std::string userInput = input();
        update(userInput, player, curr_map);
    }
}

void Engine::render(glm::ivec2 playerPos){
    camera.render(hardcoded_map, playerPos, hardcoded_actors, ss);
}

std::string Engine::input(){
    std::string userInput;
    ss << "Please make a decision...\n";
    ss << "Your options are \"n\", \"e\", \"s\", \"w\", \"quit\"\n";
    std::cout << ss.str();
    ss.str("");
    ss.clear();
    std::cin >> userInput; // read user's line
    return userInput;
}

void Engine::update(const std::string& userInput, Actor& player, Map& curr_map) {
    if (userInput == "quit") {
        ss << game_over_bad_message;
        std::cout << ss.str();
        ss.str("");
        ss.clear();
        exit(0); // terminate the program
    }
    else {
        updateActorPositions(userInput, curr_map);
        glm::ivec2 currPlayerPos = player.getPosition();
        render(currPlayerPos);
        checkAndPrintActorDialogues(currPlayerPos);
        std::cout << ss.str();
        ss.str("");
        ss.clear();
        // check if player has died
        if (health <= 0) {
            ss << "health : " << health << ", score : " << score << '\n';
            ss << game_over_bad_message;
            std::cout << ss.str();
            ss.str("");
            ss.clear();
            exit(0);
        }
    }
}


void Engine::checkAndPrintActorDialogues(glm::ivec2 playerPos) {
    for (auto& actor : hardcoded_actors) {
        if (actor.getActorName() == "player") continue;

        if (isActorInDialogueRange(actor, playerPos)) {
            glm::ivec2 actorPos = actor.getPosition();
            std::string dialogue;
            
            // Determine the appropriate dialogue based on actor's position
            if (actorPos == playerPos) {
                dialogue = actor.getContactDialogue();
            } else {
                dialogue = actor.getNearbyDialogue();
            }

            if (!dialogue.empty()) {
                ss << dialogue << '\n';
                processDialogueCommands(dialogue, actor);
            }
        }
    }
}

bool Engine::isActorInDialogueRange(const Actor& actor, const glm::ivec2& playerPos) {
    glm::ivec2 actorPos = actor.getPosition();
    bool isAdjacent = std::abs(actorPos.x - playerPos.x) <= 1 && std::abs(actorPos.y - playerPos.y) <= 1;

    return actorPos == playerPos || isAdjacent;
}

void Engine::processDialogueCommands(const std::string& dialogue, Actor& actor) {
    if (dialogue.find("health down") != std::string::npos) {
        health--;
    }
    else if (dialogue.find("score up") != std::string::npos && !actor.getScoreIncreased()) {
        score++;
        actor.setScoreIncreased(true);
    }
    else if (dialogue.find("you win") != std::string::npos) {
        ss << "health : " << health << ", score : " << score << '\n';
        ss << game_over_good_message;
        std::cout << ss.str();
        ss.str("");
        ss.clear();
        exit(0);
    }
    else if (dialogue.find("game over") != std::string::npos) {
        ss << "health : " << health << ", score : " << score << '\n';
        ss << game_over_bad_message;
        std::cout << ss.str();
        ss.str("");
        ss.clear();
        exit(0);
    }
}


void Engine::updateActorPositions(const std::string& userInput, Map& curr_map) {
    for (auto& actor : hardcoded_actors) {
        if (actor.getActorName() == "player") {
            // move player based on user input
            actor.move(userInput[0], hardcoded_actors, curr_map);
        } else {
            // update NPC positions
            actor.updateNPCPosition(curr_map, hardcoded_actors);
        }
    }
}



