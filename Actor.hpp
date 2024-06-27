//
//  Actor.hpp
//  game_engine
//
//  Created by Barbara Ribeiro on 1/26/24.
//

#ifndef Actor_hpp
#define Actor_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <unordered_set>

#include "glm/glm.hpp"
#include "Directions.hpp"
#ifdef __APPLE__
    #include "SDL2_image/SDL_image.h"
    #include "SDL2_mixer/SDL_mixer.h"
#else
    #include "SDL_image.h"
    #include "SDL_mixer.h"
#endif
#include <optional>

extern int g_uuid; // global var
class Scene;

class Collider {
public:
    float width;
    float height;
    glm::vec2 center;
    
    float top, bottom, left, right;
    
    
    void updateBounds(){
        float halfWidth = width / 2.0f;
        float halfHeight = height / 2.0f;
        top = center.y - halfHeight;
        bottom = center.y + halfHeight;
        left = center.x - halfWidth;
        right = center.x + halfWidth;
    }
    
    // use the scale factor to update the base width and height
    Collider(float w, float h, glm::vec2 c, glm::vec2 scale = glm::vec2(1.0f, 1.0f)) : center(c){
        width = w * std::abs(scale.x);
        height = h * std::abs(scale.y);
        updateBounds();
    }
};

enum class ColliderType {
    Collision,
    Trigger
};

class Actor
{
public:
    Actor(std::string name, SDL_Texture* view_image, SDL_Texture* view_image_back, SDL_Texture* view_image_damage, SDL_Texture* view_image_attack, glm::vec2 position, glm::vec2 initial_velocity, std::string nearby_dialogue, std::string contact_dialogue, glm::vec2 transform_scale, double transform_rotation_degrees, std::optional<int> order = std::nullopt, bool bounce = false)
    : name(name), view_image(view_image), view_image_back(view_image_back), view_image_damage(view_image_damage), view_image_attack(view_image_attack), position(position), velocity(initial_velocity), nearby_dialogue(nearby_dialogue), contact_dialogue(contact_dialogue), transform_scale(transform_scale), transform_rotation_degrees(transform_rotation_degrees),
        render_order(order), bounce(bounce)
    {
        actorID = g_uuid;
        g_uuid++;
    };
    
    Actor() {
        actorID = g_uuid;
        g_uuid++;
    };
    void move(double x, double y, Scene& scene);
    const std::string& getActorName() const;
    const std::string& getContactDialogue() const;
    const std::string& getNearbyDialogue() const;
    const int& getActorID() const;
    const bool& getScoreIncreased() const;
    void setScoreIncreased(bool isIncreased);
    void setPosition(glm::vec2 newPosition);
    const glm::vec2& getVelocity() const;
    const Direction& getXDirection() const;
    const Direction& getYDirection() const;
    
    
    const glm::vec2& getPosition() const;
    SDL_Texture* getViewImage() const;
    const glm::vec2& getTransformScale() const;
    double getTransformRotationDegrees() const;
    const glm::vec2& getPivotOffset() const;
    SDL_Point* getPivotSDLPoint();
    SDL_RendererFlip getFlip();
    std::optional<int> getRenderOrder() const;
    double getSpeed() const;
    const glm::vec2 getAnimationOffset() const;
    
    void setPivotPoint(std::optional<double> pivot_x, std::optional<double> pivot_y);
    void setFlip();
    void setDirection(Direction newDirection);
    void updateCollider(const glm::vec2& potentialPosition, ColliderType type);
    bool isCollidingWith(const Actor& other, ColliderType type) const;
    const Collider& getCollider(ColliderType type) const;
    void setupCollider(float colliderWidth, float colliderHeight, ColliderType type);
    void addActorToColliding(Actor* collidingActor);
    int getCollidingThisFrameNum();
    void cleanCollisionsThisFrame();
    const Collider* getConstCorrectCollider(ColliderType type) const;
    Collider* getCorrectCollider(ColliderType type);
    void damaged();
    void attacked();
    void setNearbyDialogueSFX(std::string sound_effect);
    std::string getNearbyDialogueSFX();
    bool getPlayedDialogueSFX();
    void setPlayedDialogueSFX(bool hasPlayed);
    
private:
    std::string name = "";
    int actorID = 0;
    glm::vec2 velocity = glm::vec2(0.0,0.0);
    std::string nearby_dialogue = "";
    std::string contact_dialogue = "";
    bool scoreIncreased = false;

    glm::vec2 position = glm::vec2(0,0);
    SDL_Texture* view_image = nullptr;
    SDL_Texture* view_image_back = nullptr;
    SDL_Texture* view_image_damage = nullptr;
    SDL_Texture* view_image_attack = nullptr;
    int frameDamaged = -31; // to never accidentally trigger (img shown for 30 frames)
    int frameAttacked = -31;
    glm::vec2 transform_scale = glm::vec2(1.0, 1.0);
    double transform_rotation_degrees = 0.0;
    glm::vec2 view_pivot_offset = glm::vec2(0,0);
    SDL_Point pivotSDLPoint;
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    std::optional<int> render_order;
    double speed = 0.02;
    Direction yDirection = South;
    Direction xDirection = East;
    bool bounce = false;
    glm::vec2 extraViewOffset = glm::vec2(0.0,0.0);
    std::optional<Collider> collisionCollider;
    std::optional<Collider> triggerCollider;
    bool reversedDir = false;
    std::unordered_set<Actor*> collidingActorsThisFrame;
    std::string nearbyDialogueSFX = "";
    bool hasPlayedDialogueSFX = false;
};


#endif /* Actor_hpp */
