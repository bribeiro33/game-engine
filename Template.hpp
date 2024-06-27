//
//  Template.hpp
//  game_engine
//
//  Created by Barbara Ribeiro on 2/4/24.
//

#ifndef Template_hpp
#define Template_hpp

#include <stdio.h>
#include <string>
#include <optional>
#include "glm/glm.hpp"
#include "rapidjson/document.h"
#include "Input.hpp"
#include "Renderer.hpp"

class Template
{
public:
    Template() = default;
    //explicit Template(const std::string& templatePath);
    explicit Template(const rapidjson::Document& doc, Renderer& renderer);

//private:
    // change to protected later and make template a parent class of actor
    std::string name = "";
    double x = 0;
    double y = 0;
    float vel_x = 0;
    float vel_y = 0;
    std::string nearby_dialogue = "";
    std::string contact_dialogue = "";
    SDL_Texture* view_image = nullptr;
    SDL_Texture* view_image_back = nullptr;
    SDL_Texture* view_image_damage = nullptr;
    SDL_Texture* view_image_attack = nullptr;
    double transform_scale_x = 1.0;
    double transform_scale_y = 1.0;
    double transform_rotation_degrees = 0.0;
    std::optional<double> view_pivot_offset_x;
    std::optional<double> view_pivot_offset_y;
    std::optional<int> render_order;
    bool bounce = false;
    std::optional<float> collider_width;
    std::optional<float> collider_height;
    std::optional<float> trigger_width;
    std::optional<float> trigger_height;
};

#endif /* Template_hpp */
