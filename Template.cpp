//
//  Template.cpp
//  game_engine
//
//  Created by Barbara Ribeiro on 2/4/24.
//

#include "Template.hpp"

Template::Template(const rapidjson::Document& doc, Renderer& renderer)
{
    if (doc.HasMember("name") && doc["name"].IsString()) {
        this->name = doc["name"].GetString();
    }

    if (doc.HasMember("x") && doc["x"].IsDouble()) {
        x = doc["x"].GetDouble();
    }

    if (doc.HasMember("y") && doc["y"].IsDouble()) {
        y = doc["y"].GetInt();
    }

    if (doc.HasMember("vel_x") && doc["vel_x"].IsFloat()) {
		vel_x = doc["vel_x"].GetFloat();
	}

	if (doc.HasMember("vel_y") && doc["vel_y"].IsFloat()) {
        vel_y = doc["vel_y"].GetFloat();
    }

    if (doc.HasMember("nearby_dialogue") && doc["nearby_dialogue"].IsString()) {
        this->nearby_dialogue = doc["nearby_dialogue"].GetString();
    }

    if (doc.HasMember("contact_dialogue") && doc["contact_dialogue"].IsString()) {
        this->contact_dialogue = doc["contact_dialogue"].GetString();
    }
    
//    SDL_Texture* view_image = nullptr;
//    SDL_Texture* view_image_back = nullptr;
//    double transform_scale_x = 1.0;
//    double transform_scale_y = 1.0;
//    double transform_rotation_degrees = 0.0;
//    std::optional<double> view_pivot_offset_x;
//    std::optional<double> view_pivot_offset_y;
//    std::optional<int> render_order;
//    bool bounce = false;
//    float collider_width = -1;
//    float collider_height = -1;
//    float trigger_width = -1;
//    float trigger_height = -1;
    
    if (doc.HasMember("view_image") && doc["view_image"].IsString()){
        this->view_image = renderer.getImagePointer(doc["view_image"].GetString());
    }
    if (doc.HasMember("view_image_back") && doc["view_image_back"].IsString()){
        this->view_image_back = renderer.getImagePointer(doc["view_image_back"].GetString());
    }
    if (doc.HasMember("view_image_damage") && doc["view_image_damage"].IsString()){
        // load in actor damaged
        view_image_damage = renderer.getImagePointer(doc["view_image_damage"].GetString());
    }
    if (doc.HasMember("view_image_attack") && doc["view_image_attack"].IsString()){
        // load in actor attacked img
        view_image_attack = renderer.getImagePointer(doc["view_image_attack"].GetString());
    }
    if (doc.HasMember("transform_scale_x") && doc["transform_scale_x"].IsNumber()){
        this->transform_scale_x = doc["transform_scale_x"].GetDouble();
    }
    if (doc.HasMember("transform_scale_y") && doc["transform_scale_y"].IsNumber()){
        this->transform_scale_y = doc["transform_scale_y"].GetDouble();
    }
    if (doc.HasMember("transform_rotation_degrees") && doc["transform_rotation_degrees"].IsNumber()){
        this->transform_rotation_degrees = doc["transform_rotation_degrees"].GetDouble();
    }
    if (doc.HasMember("view_pivot_offset_x") && doc["view_pivot_offset_x"].IsNumber()){
        this->view_pivot_offset_x = doc["view_pivot_offset_x"].GetDouble();
    }
    if (doc.HasMember("view_pivot_offset_y") && doc["view_pivot_offset_y"].IsNumber()){
        this->view_pivot_offset_y = doc["view_pivot_offset_y"].GetDouble();
    }
    if (doc.HasMember("render_order") && doc["render_order"].IsNumber()){
        this->render_order = doc["render_order"].GetInt();
    }
    if (doc.HasMember("movement_bounce_enabled") && doc["movement_bounce_enabled"].IsBool()){
        this->bounce =  doc["movement_bounce_enabled"].GetBool();
    }
    if (doc.HasMember("box_collider_width") && doc["box_collider_width"].IsNumber())
        this->collider_width = doc["box_collider_width"].GetFloat();
    
    if (doc.HasMember("box_collider_height") && doc["box_collider_height"].IsNumber())
        this->collider_height = doc["box_collider_height"].GetFloat();
    
    if (doc.HasMember("box_trigger_width") && doc["box_trigger_width"].IsNumber())
        this->trigger_width = doc["box_trigger_width"].GetFloat();
    
    if (doc.HasMember("box_trigger_height") && doc["box_trigger_height"].IsNumber())
        this->trigger_height = doc["box_trigger_height"].GetFloat();
}
