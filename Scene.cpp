//
//  Scene.cpp
//  game_engine
//
//  Created by Barbara Ribeiro on 2/1/24.
//

#include "Scene.hpp"
#include "Utility.hpp"
#include "Engine.hpp"
#include "KeyInput.hpp"

std::vector<Actor>& Scene::getActors(){
    return actors;
}

Actor* Scene::getPlayer(){
    return player;
}

void Scene::setPlayer(Actor* newPlayer){
    player = newPlayer;
}

Scene::Scene(const std::string& sceneFilePath){
    this->sceneFilePath = sceneFilePath;
}

std::string Scene::getScenePath(){
    return sceneFilePath;
}

//might not need
void Scene::sortMovingActors(){    
    std::sort(movingActors.begin(), movingActors.end(), [](const Actor* a, const Actor* b) {
        return a->getActorID() < b->getActorID();
    });
}

bool Scene::attemptMove(Actor* actor, const glm::vec2& newPosition) {
    glm::vec2 originalPosition = actor->getPosition();
    actor->Actor::updateCollider(newPosition, ColliderType::Collision);
    for (Actor* other : collisionActors){
        if (actor->getActorID() != other->getActorID() && actor->isCollidingWith(*other, ColliderType::Collision)){
            // if collision occurs, add to collidingActorsThisFrame vec of both actors
            actor->addActorToColliding(other);
            other->addActorToColliding(actor);
            this->contactDialogues.push_back(std::pair(other->getContactDialogue(), other));
        }
    }
    
    // should really be funcs within actor but this works for now
    if (actor->getCollidingThisFrameNum() > 0){
        // collision occurs, don't update to new position and revert collider
        actor->Actor::updateCollider(originalPosition, ColliderType::Collision);
        return false;
    }
    else {
        // if no collision occurs, move actor and keep new collider
        actor->setPosition(newPosition);
        return true;
    }
    
}

void Scene::updateActors(double playerSpeed, Audio& audio, std::string stepAudio){
    //int currFrame = Helper::GetFrameNumber();
    for (Actor* actor : movingActors) {
        if (actor->getActorName() == "player"){
            handlePlayerMovement(actor, playerSpeed, audio, stepAudio);
        }
        // NPC movement attempt
        else {
            glm::vec2 vec = actor->getVelocity();
            actor->move(vec.x, vec.y, *this);
        }
    }
    
    // clean up collision this frame sets
    for (Actor* actor : collisionActors){
        actor->cleanCollisionsThisFrame();
    }
}

void Scene::addMovingActor(Actor* actor) {
	// add actor to the list of moving actors
	movingActors.push_back(actor);
}

void Scene::renderActors(Renderer& renderer, Camera& camera) {
    double zoomFactor = camera.getZoomFactor();
    SDL_RenderSetScale(renderer.getRendererSDL(), zoomFactor, zoomFactor);
    for (int i = 0; i < sortedRenderActors.size(); ++i){
        Actor* actor = sortedRenderActors[i];
        if (actor) {
            renderer.RenderActor(*actor, camera.getPosition(), zoomFactor);
        }
    }
//    for (int i = 0; i < collisionActors.size(); ++i){
//        Actor* actor = collisionActors[i];
//        renderer.RenderCollider(*actor, camera.getPosition(), actor->getCollider(ColliderType::Collision));
//    }
//    for (int i = 0; i < triggerActors.size(); ++i){
//        Actor* actor = triggerActors[i];
//        renderer.RenderCollider(*actor, camera.getPosition(), actor->getCollider(ColliderType::Trigger));
//    }
}

void Scene::sortActorsByRenderOrder() {
    sortedRenderActors.clear();
    
    // populate sortedRenderActors w/ pointers to the actors
    for (Actor& actor : actors) {
        sortedRenderActors.push_back(&actor);
    }
    std::sort(sortedRenderActors.begin(), sortedRenderActors.end(), [](const Actor* a, const Actor* b) {
        // Compare using render_order if available; else use transform_position_y
        int aOrder = a->getRenderOrder().value_or(static_cast<int>(a->getPosition().y));
        int bOrder = b->getRenderOrder().value_or(static_cast<int>(b->getPosition().y));

        if (aOrder != bOrder) {
            return aOrder < bOrder;
        }
        // if render_order is the same, use y position
        else if (a->getPosition().y != b->getPosition().y) {
            return a->getPosition().y < b->getPosition().y;
        }
        // if both render_order and y pos are the same, break tie with actorID
        else {
            return a->getActorID() < b->getActorID();
        }
    });
}

void Scene::handlePlayerMovement(Actor* player, double playerSpeed, Audio& audio, std::string stepAudio){
    glm::vec2 movement(0.0f, 0.0f);

    if (KeyInput::GetKey(SDL_SCANCODE_UP) || KeyInput::GetKey(SDL_SCANCODE_W)) {
        movement.y -= 1;
        player->setDirection(North);
    }
    if (KeyInput::GetKey(SDL_SCANCODE_DOWN) || KeyInput::GetKey(SDL_SCANCODE_S)) {
        movement.y += 1;
        player->setDirection(South);
    }
    if (KeyInput::GetKey(SDL_SCANCODE_LEFT) || KeyInput::GetKey(SDL_SCANCODE_A)) {
        movement.x -= 1;
        player->setDirection(West);
    }
    if (KeyInput::GetKey(SDL_SCANCODE_RIGHT) || KeyInput::GetKey(SDL_SCANCODE_D)) {
        movement.x += 1;
        player->setDirection(East);
    }

    // Normalize the movement vector
    if (movement.x != 0 || movement.y != 0) { // Avoid normalizing the zero vector
        movement = glm::normalize(movement);
        // when moving, play sound effect (if it exists) every 20th frame
        if (stepAudio != "" && Helper::GetFrameNumber() % 20 == 0){
            int channel_num = Helper::GetFrameNumber() % 48 + 2;
            audio.playMusic(stepAudio, channel_num, 0);
        }
    }
    
    movement *= playerSpeed;
    player->move(movement.x, movement.y, *this);
}

void Scene::addCollisionActor(Actor* actor) {
    collisionActors.push_back(actor);
}
void Scene::addTriggerActor(Actor* actor) {
    triggerActors.push_back(actor);
}

const std::vector<std::pair<std::string, Actor*>>& Scene::getContactDialogues(){
    return contactDialogues;
}
const std::vector<std::pair<std::string, Actor*>>& Scene::getNearbyDialogues(){
    collectTriggerText();
    return nearbyDialogues;
}

void Scene::clearDialogues(){
    nearbyDialogues.clear();
    contactDialogues.clear();
}

void Scene::collectTriggerText(){
    player->updateCollider(player->getPosition(), ColliderType::Trigger);
    for (Actor* other : triggerActors){
        const std::string& currDialogue = other->getNearbyDialogue();
        if (player->getActorID() != other->getActorID() && currDialogue != "" && player->isCollidingWith(*other, ColliderType::Trigger)){
            // if their trigger boxes overlap and nearbyDialogue exists, collect their trigger dialogue
            this->nearbyDialogues.push_back(std::pair(currDialogue, other));
        }
    }
}
