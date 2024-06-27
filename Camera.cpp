//
//  Camera.cpp
//  game_engine
//
//  Created by Barbara Ribeiro on 1/24/24.
//

#include "Camera.hpp"

Camera::Camera() {
}

//void Camera::configCamera(){
//    Input::readRenderFile(width, height);
//}

void Camera::update(const glm::vec2& targetPosition, float easeFactor) {
    //position = playerPosition + offset;
    // Calculate the target position by adding the offset to the player's position
    glm::vec2 targetWithOffset = targetPosition + offset;
    
    // Use glm::mix to smoothly interpolate the camera's position towards the target position
    position = glm::mix(position, targetWithOffset, easeFactor);
}

// Set the camera offset
void Camera::setOffset(double offsetX, double offsetY) {
    offset = glm::vec2(offsetX, offsetY);
}

void Camera::setWidth(int width_in){
    width = width_in;
}

void Camera::setHeight(int height_in){
    height = height_in;
}

// Get the current camera position
glm::vec2 Camera::getPosition() const {
    return position;
}

int Camera::getWidth() {
    return width;
}

int Camera::getHeight() {
    return height;
}

void Camera::setZoomFactor(double zoomFactor_in) {
    zoomFactor = zoomFactor_in;
}

void Camera::setEaseFactor(float easeFactor_in) {
    easeFactor = easeFactor_in;
}

double Camera::getZoomFactor() const {
	return zoomFactor;
}

float Camera::getEaseFactor() const {
    return easeFactor;
}

void Camera::setInitialPosition(glm::vec2 playerStartPos)
{
    position = playerStartPos + offset;
}
