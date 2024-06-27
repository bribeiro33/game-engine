//
//  Input.cpp
//  game_engine
//
//  Created by Barbara Ribeiro on 1/31/24.
//

#include "Input.hpp"
#include "Scene.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "IntroHandler.hpp"
#include "rapidjson/filereadstream.h"
#include <unordered_map>
#include <iostream>
#include <vector>
#include <typeinfo>
#include <optional>


void Input::ReadJsonFile(const std::string& path, rapidjson::Document & out_document)
{
    FILE* file_pointer = nullptr;
    #ifdef _WIN32
    fopen_s(&file_pointer, path.c_str(), "rb");
    #else
    file_pointer = fopen(path.c_str(), "rb");
    #endif
    char buffer[65536];
    rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
    out_document.ParseStream(stream);
    std::fclose(file_pointer);

    if (out_document.HasParseError()) {
        rapidjson::ParseErrorCode errorCode = out_document.GetParseError();
        std::cout << "error parsing json at [" << path << "]" << std::endl;
        exit(0);
    }
}

void Input::checkResourcesDirectory() {
    // check if the resources directory exists
    if (!std::filesystem::exists("resources")) {
        // print error message and exit
        std::cout << "error: resources/ missing";
        exit(0);
    }
    
    // check if the resources/game.config file exists
    if (!std::filesystem::exists("resources/game.config")) {
        std::cout << "error: resources/game.config missing";
        exit(0);
    }
}

void Input::readGameFile(Renderer& window, IntroHandler& intro, Engine& engine, const rapidjson::Document& document) {

    // get game title for window
    if (document.HasMember("game_title") && document["game_title"].IsString()) {
        window.setGameTitle(document["game_title"].GetString());
    }
    // get font
    std::string fontName = "";
    if (document.HasMember("font") && document["font"].IsString()) {
        fontName = document["font"].GetString();
        std::string fontPath = "resources/fonts/" + fontName + ".ttf";
        if (!std::filesystem::exists(fontPath)) {
            std::cout << "error: font " << fontName << " missing";
            exit(0);
        }
        window.setFont(LoadFont(fontPath));
    }
    // ending sequence vars
    if (document.HasMember("game_over_good_audio") && document["game_over_good_audio"].IsString()) {
        std::string goodAudio = document["game_over_good_audio"].GetString();
        std::string imgPathWAV = "resources/audio/" + goodAudio + ".wav";
        std::string imgPathOGG = "resources/audio/" + goodAudio + ".ogg";
        if (!std::filesystem::exists(imgPathWAV) && !std::filesystem::exists(imgPathOGG)) {
            exit(0);
        }
        engine.setGoodAudio(goodAudio);
    }
    
    if (document.HasMember("game_over_bad_audio") && document["game_over_bad_audio"].IsString()) {
        std::string badAudio = document["game_over_bad_audio"].GetString();
        std::string imgPathWAV = "resources/audio/" + badAudio + ".wav";
        std::string imgPathOGG = "resources/audio/" + badAudio + ".ogg";
        if (!std::filesystem::exists(imgPathWAV) && !std::filesystem::exists(imgPathOGG)) {
            exit(0);
        }
        engine.setBadAudio(badAudio);
    }
    
    engine.setBadImage(readStringFromJSON(document, "game_over_bad_image", ""));
    engine.setGoodImage(readStringFromJSON(document, "game_over_good_image", ""));
    
    double speed = readDoubleFromJSON(document, "player_movement_speed", 0.02);
    engine.setPlayerSpeed(speed);
    
    engine.setScoreSFX(loadInAudio(document, "score_sfx"));
    
    // load in intro images if exist
    std::vector<std::string> temp_img = readStringArrayFromJSON(document, "intro_image");
    
    // don't process any intro text if there are no intro imgs
    if (temp_img.empty()){
        // also set gameState to MainGame
        engine.changeGameState(Engine::GameState::MainGame);
        return;
    }
    else {
        engine.changeGameState(Engine::GameState::Intro);
    }
    
    intro.setIntroImageNames(temp_img);
    
    std::vector<std::string> temp_text = readStringArrayFromJSON(document, "intro_text");
    // load in intro text if exists
    intro.setIntroText(temp_text);
    
    // If intro_text is specified in game.config but no font is also specified
    if (!temp_text.empty() && fontName == ""){
        std::cout << "error: text render failed. No font configured";
        exit(0);
    }
    
}

std::string Input::loadInAudio(const rapidjson::Document& document, const std::string audio_property){
    if (document.HasMember(audio_property.c_str()) && document[audio_property.c_str()].IsString()) {
        std::string sound_effect = document[audio_property.c_str()].GetString();
        std::string imgPathWAV = "resources/audio/" + sound_effect + ".wav";
        std::string imgPathOGG = "resources/audio/" + sound_effect + ".ogg";
        if (!std::filesystem::exists(imgPathWAV) && !std::filesystem::exists(imgPathOGG)) {
            exit(0);
        }
        return sound_effect;
    }
    return "";
}

std::string Input::getIntroSong(rapidjson::Document& document){
    // if intro background music is specd
    if (document.HasMember("intro_bgm") && document["intro_bgm"].IsString()) {
        std::string intro_bgm = document["intro_bgm"].GetString();
        return intro_bgm;
    }
    return "";
}

std::string Input::getMainSong(rapidjson::Document& document){
    // if main game background music is specd
    if (document.HasMember("gameplay_audio") && document["gameplay_audio"].IsString()) {
        std::string gameplay_audio = document["gameplay_audio"].GetString();
        return gameplay_audio;
    }
    return "";
}

std::string Input::getHPImg(rapidjson::Document& document){
    if (document.HasMember("hp_image") && document["hp_image"].IsString()) {
        std::string hp_image = document["hp_image"].GetString();
        return hp_image;
        
    }
    return "";
}


void Input::readRenderFile(Renderer& renderer, Camera& camera) {
    rapidjson::Document document;
    std::string path = "resources/rendering.config";
    
    // if file doesn't exist, exit and use default values
    if (!std::filesystem::exists(path)) {
        return;
    }

    ReadJsonFile(path, document);
    
    renderer.setWidth(readIntFromJSON(document, "x_resolution", 640));
    renderer.setHeight(readIntFromJSON(document, "y_resolution", 360));
    renderer.setMovementFlip(readBoolFromJSON(document, "x_scale_actor_flipping_on_movement", false));
    
    camera.setWidth(readIntFromJSON(document, "x_resolution", 640));
    camera.setHeight(readIntFromJSON(document, "y_resolution", 360));
    camera.setOffset(readDoubleFromJSON(document, "cam_offset_x", 0.0), readDoubleFromJSON(document, "cam_offset_y", 0.0));
    //double num = readDoubleFromJSON(document, "zoom_factor", 1.0);
    camera.setZoomFactor(readDoubleFromJSON(document, "zoom_factor", 1.0));
    camera.setEaseFactor(readFloatFromJSON(document, "cam_ease_factor", 1.0));
    
    int color_r = readIntFromJSON(document, "clear_color_r", 255);
    int color_g = readIntFromJSON(document, "clear_color_g", 255);
    int color_b = readIntFromJSON(document, "clear_color_b", 255);
    renderer.setClearColors(color_r, color_g, color_b);
}

std::string Input::getInitialScene(const rapidjson::Document& document) {
    
    // determine the initial scene
    if (!document.HasMember("initial_scene") || !document["initial_scene"].IsString()) {
        std::cout << "error: initial_scene unspecified";
        exit(0);
    }
    // determine if scene exists
    std::string sceneName = document["initial_scene"].GetString();
    std::string sceneFilePath = "resources/scenes/" + sceneName + ".scene";
    findScene(sceneFilePath);
    return sceneFilePath;
}

void Input::findScene(std::string scenePath) {
    // determine if scene exists
    std::string sceneName = extractSceneName(scenePath);
    if (!std::filesystem::exists(scenePath)) {
        std::cout << "error: scene " << sceneName << " is missing";
        exit(0);
    }
}

void Input::readScene(Scene& scene, Engine& engine, Renderer& renderer){
    rapidjson::Document document;
    ReadJsonFile(scene.getScenePath(), document);
    
    std::vector<Actor>& actors = scene.getActors();
    
    // reserve space in vector for number of actors (+ 1 in case)
    actors.reserve(document["actors"].Size());

    // iterate over all actor json descs, create new objects and add to vector
    for (auto& v: document["actors"].GetArray()) {
        bool isPlayer = false;

        // initialize all properties to default values
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
        std::optional<double> view_pivot_offset_x; // default is actor_view.w * 0.5
        std::optional<double> view_pivot_offset_y;
        std::optional<int> render_order;
        bool bounce = false;
        std::optional<float> collider_width;
        std::optional<float> collider_height;
        std::optional<float> trigger_width;
        std::optional<float> trigger_height;
        


        // see if actor has a template
        if (v.HasMember("template") && v["template"].IsString()) {
		    std::string templateName = v["template"].GetString();
            // Check if the template is already loaded
            if (!engine.isInTemplates(templateName)) {
                std::string templatePath = "resources/actor_templates/" + templateName + ".template";
                // if the template file is missing, print error message and exit
				if (!std::filesystem::exists(templatePath)) {
					std::cout << "error: template " << templateName << " is missing";
					exit(0);
				}
                // read the template file and add it to the engine
                rapidjson::Document templateDoc;
                Input::ReadJsonFile(templatePath, templateDoc);
                Template newTemplate(templateDoc, renderer);
				engine.addTemplate(templateName, newTemplate);
            }
            // get the template from the engine
            Template& curr_template = engine.getTemplate(templateName);
            // Apply properties from template
            name = curr_template.name;
            x = curr_template.x;
            y = curr_template.y;
            vel_x = curr_template.vel_x;
            vel_y = curr_template.vel_y;
            nearby_dialogue = curr_template.nearby_dialogue;
            contact_dialogue = curr_template.contact_dialogue;
            view_image = curr_template.view_image;
            view_image_back = curr_template.view_image_back;
            view_image_damage = curr_template.view_image_damage;
            view_image_attack = curr_template.view_image_attack;
            transform_scale_x = curr_template.transform_scale_x;
            transform_scale_y = curr_template.transform_scale_y;
            transform_rotation_degrees = curr_template.transform_rotation_degrees;
            view_pivot_offset_x = curr_template.view_pivot_offset_x;
            view_pivot_offset_y = curr_template.view_pivot_offset_y;
            render_order = curr_template.render_order;
            bounce = curr_template.bounce;
            collider_width = curr_template.collider_width;
            collider_height = curr_template.collider_height;
            trigger_width = curr_template.trigger_width;
            trigger_height = curr_template.trigger_height;
            
		} 

        // override with actor-specific properties
        if (v.HasMember("name") && v["name"].IsString()) name = v["name"].GetString();
        //if (v.HasMember("view") && v["view"].IsString()) view = v["view"].GetString()[0];
        if (v.HasMember("transform_position_x") && v["transform_position_x"].IsNumber()) x = v["transform_position_x"].GetDouble();
        if (v.HasMember("transform_position_y") && v["transform_position_y"].IsNumber()) y = v["transform_position_y"].GetDouble();
        if (v.HasMember("vel_x") && v["vel_x"].IsFloat()) vel_x = v["vel_x"].GetFloat();
        if (v.HasMember("vel_y") && v["vel_y"].IsFloat()) vel_y = v["vel_y"].GetFloat();
        if (v.HasMember("nearby_dialogue") && v["nearby_dialogue"].IsString()) nearby_dialogue = v["nearby_dialogue"].GetString();
        if (v.HasMember("contact_dialogue") && v["contact_dialogue"].IsString()) contact_dialogue = v["contact_dialogue"].GetString();
        if (v.HasMember("view_image") && v["view_image"].IsString()){
            // load in actor image
            view_image = renderer.getImagePointer(v["view_image"].GetString());
        }
        if (v.HasMember("view_image_back") && v["view_image_back"].IsString()){
            // load in actor back image
            view_image_back = renderer.getImagePointer(v["view_image_back"].GetString());
        }
        if (v.HasMember("view_image_damage") && v["view_image_damage"].IsString()){
            // load in actor damaged
            view_image_damage = renderer.getImagePointer(v["view_image_damage"].GetString());
        }
        if (v.HasMember("view_image_attack") && v["view_image_attack"].IsString()){
            // load in actor attacked img
            view_image_attack = renderer.getImagePointer(v["view_image_attack"].GetString());
        }
        if (v.HasMember("transform_scale_x") && v["transform_scale_x"].IsNumber()){
            transform_scale_x = v["transform_scale_x"].GetDouble();
        }
        if (v.HasMember("transform_scale_y") && v["transform_scale_y"].IsNumber()){
            transform_scale_y = v["transform_scale_y"].GetDouble();
        }
        if (v.HasMember("transform_rotation_degrees") && v["transform_rotation_degrees"].IsNumber()){
            transform_rotation_degrees = v["transform_rotation_degrees"].GetDouble();
        }
        if (v.HasMember("view_pivot_offset_x") && v["view_pivot_offset_x"].IsNumber()){
            view_pivot_offset_x = v["view_pivot_offset_x"].GetDouble();
        }
        if (v.HasMember("view_pivot_offset_y") && v["view_pivot_offset_y"].IsNumber()){
            view_pivot_offset_y = v["view_pivot_offset_y"].GetDouble();
        }
        if (v.HasMember("render_order") && v["render_order"].IsNumber()){
            render_order = v["render_order"].GetInt();
        }
        if (v.HasMember("movement_bounce_enabled") && v["movement_bounce_enabled"].IsBool()){
            bounce =  v["movement_bounce_enabled"].GetBool();
        }
        if (v.HasMember("box_collider_width") && v["box_collider_width"].IsNumber()) collider_width = v["box_collider_width"].GetFloat();
        if (v.HasMember("box_collider_height") && v["box_collider_height"].IsNumber()) collider_height = v["box_collider_height"].GetFloat();
        
        if (v.HasMember("box_trigger_width") && v["box_trigger_width"].IsNumber()) trigger_width = v["box_trigger_width"].GetFloat();
        if (v.HasMember("box_trigger_height") && v["box_trigger_height"].IsNumber()) trigger_height = v["box_trigger_height"].GetFloat();
        
        if (v.HasMember("damage_sfx") && v["damage_sfx"].IsString()) {
            std::string sound_effect = v["damage_sfx"].GetString();
            std::string imgPathWAV = "resources/audio/" + sound_effect + ".wav";
            std::string imgPathOGG = "resources/audio/" + sound_effect + ".ogg";
            if (!std::filesystem::exists(imgPathWAV) && !std::filesystem::exists(imgPathOGG)) {
                exit(0);
            }
            engine.setDamageSFX(sound_effect);
        }
        
        if (v.HasMember("step_sfx") && v["step_sfx"].IsString()) {
            std::string sound_effect = v["step_sfx"].GetString();
            std::string imgPathWAV = "resources/audio/" + sound_effect + ".wav";
            std::string imgPathOGG = "resources/audio/" + sound_effect + ".ogg";
            if (!std::filesystem::exists(imgPathWAV) && !std::filesystem::exists(imgPathOGG)) {
                exit(0);
            }
            engine.setStepSFX(sound_effect);
        }
        
        std::string nearbyDialogueSFX = "";
        if (v.HasMember("nearby_dialogue_sfx") && v["nearby_dialogue_sfx"].IsString()) {
            std::string sound_effect = v["nearby_dialogue_sfx"].GetString();
            std::string imgPathWAV = "resources/audio/" + sound_effect + ".wav";
            std::string imgPathOGG = "resources/audio/" + sound_effect + ".ogg";
            if (!std::filesystem::exists(imgPathWAV) && !std::filesystem::exists(imgPathOGG)) {
                exit(0);
            }
            nearbyDialogueSFX = sound_effect;
        }
        
        // combining vel_x and vel_y into velocity
        glm::vec2 velocity(vel_x, vel_y);
        // combining x and y into position
        glm::vec2 position(x, y);
        // Set transform scale
        glm::vec2 transform_scale(transform_scale_x, transform_scale_y);
        
        // work around since name doesn't work after
        if (name == "player"){
            isPlayer = true;
        }

        // create the Actor object
        Actor newActor = Actor(name, view_image, view_image_back, view_image_damage, view_image_attack, position, velocity, nearby_dialogue, contact_dialogue, transform_scale, transform_rotation_degrees, render_order, bounce);

        // add the new Actor object to the vector
        actors.push_back(std::move(newActor));

        Actor& newActorRef = actors.back();
        // Set flip
        newActorRef.setFlip();
        // set pivot position
        newActorRef.setPivotPoint(view_pivot_offset_x, view_pivot_offset_y);
        
        // set initial direction
        if (vel_x < 0) {
            newActorRef.setDirection(West);
        }
        if (velocity.y < 0) {
            newActorRef.setDirection(North);
        }
        

        // if the actor has a non-0 velocity, add them to the moving actors list
        if (vel_x != 0 || vel_y != 0) {
	        scene.addMovingActor(&newActorRef);
		}
        
        // set NPC dialogue sfx
        newActorRef.setNearbyDialogueSFX(nearbyDialogueSFX);
        
        // check if it has collider width and height --> calculate box collider

        if (collider_width.has_value() && collider_height.has_value()){
            newActorRef.setupCollider(collider_width.value(), collider_height.value(), ColliderType::Collision);
            scene.addCollisionActor(&newActorRef);
        }
        
        if (trigger_width.has_value() && trigger_height.has_value()){
            newActorRef.setupCollider(trigger_width.value(), trigger_height.value(), ColliderType::Trigger);
            scene.addTriggerActor(&newActorRef);
        }

    	// if the actor is the player, set the player pointer in the scene and add them to the moving actors list
        if (isPlayer){
            scene.setPlayer(&newActorRef);
            scene.addMovingActor(&newActorRef);
        }
    }
}

bool Input::checkTemplates(){
    rapidjson::Document document;
    std::string path = "resources/actor_templates/";
    
    // if file doesn't exist, return false as there are no templates specified
    if (!std::filesystem::exists(path)) {
        return false;
    }
    
    return true;
    
}


std::string Input::obtain_word_after_phrase(const std::string& input, const std::string& phrase) { 
    // Find the position of the phrase in the string
    size_t pos = input.find(phrase);
    // If the phrase is not found, return an empty string
    if (pos == std::string::npos) {
		    return "";
	    }
    // Find the starting position of the next word (skip spaces after the phrase)
    pos += phrase.length();
    while (pos < input.size() && std::isspace(input[pos])) {
        ++pos;
    }

    // If we're at the end of the string, return an empty string
    if (pos == input.size()) {
	    return "";
    }

    // find the end position of the word (until a space or the end of the string)
    size_t end_pos = pos;
    while (end_pos < input.size() && !std::isspace(input[end_pos])) {
	    ++end_pos;
    }

    // extract the word from the string and return it
    return input.substr(pos, end_pos - pos);
}
   
std::string Input::extractSceneName(const std::string& path) {
    size_t lastSlashPos = path.rfind("/");
    size_t lastDotPos = path.rfind(".scene");

    if (lastSlashPos == std::string::npos || lastDotPos == std::string::npos) {
        return "";
    }

    // Extract the scene name by taking the substring between the last slash and ".scene"
    std::string sceneName = path.substr(lastSlashPos + 1, lastDotPos - lastSlashPos - 1);

    return sceneName;
}
bool Input::readBoolFromJSON(const rapidjson::Document& document, const std::string& memberName, bool defaultValue) {
    if (document.HasMember(memberName.c_str()) && document[memberName.c_str()].IsBool()) {
        return document[memberName.c_str()].GetBool();
    }
    return defaultValue;
}

int Input::readIntFromJSON(const rapidjson::Document& document, const std::string& memberName, int defaultValue) {
    if (document.HasMember(memberName.c_str()) && document[memberName.c_str()].IsInt()) {
        return document[memberName.c_str()].GetInt();
    }
    return defaultValue;
}

double Input::readDoubleFromJSON(const rapidjson::Value& value, const std::string& memberName, double defaultValue) {
    if (value.HasMember(memberName.c_str()) && value[memberName.c_str()].IsNumber()) {
        return value[memberName.c_str()].GetDouble();
    }
    return defaultValue;
}

float Input::readFloatFromJSON(const rapidjson::Value& value, const std::string& memberName, float defaultValue) {
    if (value.HasMember(memberName.c_str()) && value[memberName.c_str()].IsNumber()) {
        return value[memberName.c_str()].GetFloat();
    }
    return defaultValue;
}


std::string Input::readStringFromJSON(const rapidjson::Document& document, const std::string& memberName, const std::string& defaultValue) {
    if (document.HasMember(memberName.c_str()) && document[memberName.c_str()].IsString()) {
        return document[memberName.c_str()].GetString();
    }
    return defaultValue;
}

std::vector<std::string> Input::readStringArrayFromJSON(const rapidjson::Document& document, const std::string& memberName) {
    std::vector<std::string> result;

    // Check if the member exists and is an array
    if (document.HasMember(memberName.c_str()) && document[memberName.c_str()].IsArray()) {
        const rapidjson::Value& array = document[memberName.c_str()];

        // Iterate through the array
        for (auto& value : array.GetArray()) {
            // Check if the current element is a string
            if (value.IsString()) {
                // Add the string to the result vector
                result.push_back(value.GetString());
            } else {
                // handle non string elements? generalize func?
            }
        }
    }

    // Return the result, which could be an empty vector
    return result;
}
