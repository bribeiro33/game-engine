//
//  Scene.hpp
//  game_engine
//
//  Created by Barbara Ribeiro on 2/1/24.
//

#ifndef Scene_hpp
#define Scene_hpp

#include <stdio.h>
#include <unordered_set>
#include <optional>
#include <vector>
#include <utility>
#include "Utility.hpp"
#include "Actor.hpp"
#include "glm/glm.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Audio.hpp"
#define SCENE_UNIT 100

class Engine;
class Scene {
public:
    Scene(const std::string& sceneFilePath);
    std::vector<Actor>& getActors();
    Actor* getPlayer();
    void setPlayer(Actor* newPlayer);
    std::string getScenePath();
    
    void sortMovingActors();
    bool attemptMove(Actor* actor, const glm::vec2& newPosition);
    void updateActors(double playerSpeed, Audio& audio, std::string stepAudio);
    void addMovingActor(Actor* actor);
    void updateBlockingActors(glm::vec2& pos, const int& actor_id);
    void updateLocationToActors(glm::vec2& pos, Actor& actor);
    
    void renderActors(Renderer& renderer, Camera& camera);
    void sortActorsByRenderOrder();
    void handlePlayerMovement(Actor* player, double playerSpeed, Audio& audio, std::string stepAudio);
    
    void addCollisionActor(Actor* actor);
    void addTriggerActor(Actor* actor);
    const std::vector<std::pair<std::string, Actor*>>& getContactDialogues();
    const std::vector<std::pair<std::string, Actor*>>& getNearbyDialogues();
    void clearDialogues();
    void collectTriggerText();
private:
    std::vector<Actor> actors;
    Actor* player = nullptr;
    std::string sceneFilePath = "";
    std::vector<Actor*> sortedRenderActors;
    std::vector<Actor*> movingActors;
    std::vector<Actor*> collisionActors;
    std::vector<Actor*> triggerActors;
    std::vector<std::pair<std::string, Actor*>> contactDialogues;
    std::vector<std::pair<std::string, Actor*>> nearbyDialogues;
};

#endif /* Scene_hpp */
