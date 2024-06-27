//
//  Actor.cpp
//  game_engine
//
//  Created by Barbara Ribeiro on 1/26/24.
//

#include "Actor.hpp"
#include <algorithm>
#include "Scene.hpp"
#include "KeyInput.hpp"

// keeps track of the unique id for each actor
int g_uuid = 0;

void Actor::move(double x, double y, Scene& scene){
    glm::vec2 newPos = position;
    bool didMove = false;
    
    // if reversed in previous frame, need to update the actor's direction
    // could update all actors but would probably be less efficient
    if (reversedDir){
        if (velocity.x < 0) {
            setDirection(West);
        }
        else {
            setDirection(East);
        }

        if (velocity.y < 0) {
            setDirection(North);
        }
        else {
            setDirection(South);
        }
    }
    // Compute visual offset if intending to move
    if (bounce && (x != 0.0 || y != 0.0)) {
        extraViewOffset = glm::vec2(0, -glm::abs(glm::sin(Helper::GetFrameNumber() * 0.15f)) * 10.0f);
    }
    else {
        extraViewOffset = glm::vec2(0, 0); // Reset if not moving
    }
    
    newPos.x += x;
    newPos.y += y;
    didMove = scene.attemptMove(this, newPos);
    // switch vel direction if were unable to move and wasn't player
    if (!didMove && name != "player"){
        velocity = -velocity;
        reversedDir = true;
    }
}


const std::string& Actor::getActorName() const { return name; }
const glm::vec2& Actor::getPosition() const { return position; }
const glm::vec2& Actor::getVelocity() const { return velocity; }
const std::string& Actor::getContactDialogue() const { return contact_dialogue; }
const std::string& Actor::getNearbyDialogue() const { return nearby_dialogue; }
const bool& Actor::getScoreIncreased() const { return scoreIncreased; }
const int& Actor::getActorID() const { return actorID; }
double Actor::getSpeed() const { return speed; }
void Actor::setScoreIncreased(bool isIncreased) { scoreIncreased = isIncreased; }
void Actor::setPosition(glm::vec2 newPosition) { position = newPosition; }


SDL_Texture* Actor::getViewImage() const {
    if (view_image_damage && Helper::GetFrameNumber() - 30 < frameDamaged){
        return view_image_damage;
    }
    if (view_image_attack && Helper::GetFrameNumber() - 30 < frameAttacked){
        return view_image_attack;
    }
    if (view_image_back && yDirection == North){
        return view_image_back;
    }
    return view_image;
}

const glm::vec2& Actor::getTransformScale() const { return transform_scale; }
double Actor::getTransformRotationDegrees() const { return transform_rotation_degrees; }
const glm::vec2& Actor::getPivotOffset() const { return view_pivot_offset; }
SDL_Point* Actor::getPivotSDLPoint() { return &pivotSDLPoint; }
SDL_RendererFlip Actor::getFlip() { return flip; }
const glm::vec2 Actor::getAnimationOffset() const { return extraViewOffset; }

void Actor::setPivotPoint(std::optional<double> pivot_x, std::optional<double> pivot_y){
    if (!pivot_x.has_value() || !pivot_y.has_value()){
        if (view_image != nullptr) {
            int view_width, view_height;
            SDL_QueryTexture(view_image, nullptr, nullptr, &view_width, &view_height);

            // set default pivot vals if not set in scene
            if (!pivot_x.has_value()) {
                pivot_x = static_cast<double>(view_width) * 0.5;
            }
            if (!pivot_y.has_value()) {
                pivot_y = static_cast<double>(view_height) * 0.5;
            }
        }
        else {
            std::cerr << "Warning: view_image not set before setting pivot point." << std::endl;
            return;
        }
    }

    pivotSDLPoint.x = std::round(pivot_x.value() * std::abs(transform_scale.x));
    pivotSDLPoint.y = std::round(pivot_y.value() * std::abs(transform_scale.y));
}
void Actor::setFlip() {
    if (transform_scale.x < 0) flip = SDL_FLIP_HORIZONTAL;
    if (transform_scale.y < 0) flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);
}

std::optional<int> Actor::getRenderOrder() const {
    return render_order;
}

void Actor::setDirection(Direction newDirection){
    if (newDirection == North || newDirection == South){
        yDirection = newDirection;
    }
    else {
        xDirection = newDirection;
    }
}

const Direction& Actor::getXDirection() const {
    return xDirection;
}

const Direction& Actor::getYDirection() const {
    return yDirection;
}

void Actor::updateCollider(const glm::vec2& potentialPosition, ColliderType type){
    Collider* collider = getCorrectCollider(type);
    if (collider){
        collider->center = potentialPosition;
        collider->updateBounds();
    }
}

const Collider* Actor::getConstCorrectCollider(ColliderType type) const{
    if (type == ColliderType::Collision && collisionCollider.has_value()){
        return &collisionCollider.value();
    }
    else if (type == ColliderType::Trigger && triggerCollider.has_value()){
        return &triggerCollider.value();
    }
    return nullptr;
}

Collider* Actor::getCorrectCollider(ColliderType type){
    if (type == ColliderType::Collision && collisionCollider.has_value()){
        return &collisionCollider.value();
    }
    else if (type == ColliderType::Trigger && triggerCollider.has_value()){
        return &triggerCollider.value();
    }
    return nullptr;
}

bool Actor::isCollidingWith(const Actor& otherActor, ColliderType type) const {
    const Collider* collider = getConstCorrectCollider(type);
    const Collider* otherCollider = otherActor.getConstCorrectCollider(type);
    if (!collider || !otherCollider) return false;
    
    // uncompacted makes it easier to debug
    bool leftCheck = collider->left < otherCollider->right;
    bool rightCheck = collider->right > otherCollider->left;
    bool topCheck = collider->top < otherCollider->bottom;
    bool bottomCheck = collider->bottom > otherCollider->top;

    bool isOverlapping = leftCheck && rightCheck && topCheck && bottomCheck;
    return isOverlapping;
}

const Collider& Actor::getCollider(ColliderType type) const {
    const Collider* collider = getConstCorrectCollider(type);
    return *collider;
}

void Actor::setupCollider(float colliderWidth, float colliderHeight, ColliderType type){
    if (type == ColliderType::Collision){
        collisionCollider = Collider{colliderWidth, colliderHeight, position, transform_scale};
    }
    else if (type == ColliderType::Trigger){
        triggerCollider = Collider{colliderWidth, colliderHeight, position, transform_scale};
    }
    else {
        std::cout << "error: setup of collider incomplete" << std::endl;
    }
}

void Actor::addActorToColliding(Actor* collidingActor){
    collidingActorsThisFrame.insert(collidingActor);
}
int Actor::getCollidingThisFrameNum(){
    return static_cast<int>(collidingActorsThisFrame.size());
}

void Actor::cleanCollisionsThisFrame(){
    collidingActorsThisFrame.clear();
}

void Actor::damaged(){
    frameDamaged = Helper::GetFrameNumber();
}
void Actor::attacked(){
    frameAttacked = Helper::GetFrameNumber();
}

void Actor::setNearbyDialogueSFX(std::string sound_effect){
    nearbyDialogueSFX = sound_effect;
}

std::string Actor::getNearbyDialogueSFX(){
    return nearbyDialogueSFX;
}

bool Actor::getPlayedDialogueSFX(){
    return hasPlayedDialogueSFX;
}
void Actor::setPlayedDialogueSFX(bool hasPlayed){
    hasPlayedDialogueSFX = hasPlayed;
}
