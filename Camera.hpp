//
//  Camera.hpp
//  game_engine
//
//  Created by Barbara Ribeiro on 1/24/24.
//

#ifndef Camera_hpp
#define Camera_hpp
#include "glm/glm.hpp"
#include "rapidjson/document.h"
#include "Actor.hpp"
#include "Input.hpp"
#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>
#include <sstream>

class Camera {
public:
    Camera(int width, int height) : width(width), height(height) {};
    Camera();
    int getWidth();
    int getHeight();
    void update(const glm::vec2& targetPosition, float easeFactor);
    void setOffset(double offsetX, double offsetY);
    void setWidth(int width_in);
    void setHeight(int height_in);
    void setZoomFactor(double zoomFactor);
    void setEaseFactor(float easeFactor);
    void setInitialPosition(glm::vec2 playerStartPos);
    glm::vec2 getPosition() const;
    double getZoomFactor() const;
    float getEaseFactor() const;
private:
    glm::vec2 position = glm::vec2(0,0);
    glm::vec2 offset = glm::vec2(0,0);
    int width = 640;
    int height = 360;
    double zoomFactor = 1.0;
    float easeFactor = 1.0;
};

#endif /* Camera_hpp */
