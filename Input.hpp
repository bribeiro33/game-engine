//
//  Input.hpp
//  game_engine
//
//  Created by Barbara Ribeiro on 1/31/24.
//

#ifndef Input_hpp
#define Input_hpp

#include <stdio.h>
#include <cstdlib>
#include <filesystem>
#include <unordered_map>
#include <string>
#include "Actor.hpp"
#include "rapidjson/document.h"
#include "Scene.hpp"
#include "Renderer.hpp"
#include "Utility.hpp"

class Engine;
class Camera;
class IntroHandler;
class Input{
public:
    static void ReadJsonFile(const std::string& path, rapidjson::Document& out_document);
    static void checkResourcesDirectory();
    static void readGameFile(Renderer& window, IntroHandler& intro, Engine& engine, const rapidjson::Document& document);
    static void readRenderFile(Renderer& renderer, Camera& camera);
    static std::string getInitialScene(const rapidjson::Document& document);
    static std::string getIntroSong(rapidjson::Document& document);
    static std::string getMainSong(rapidjson::Document& document);
    static std::string getHPImg(rapidjson::Document& document);
    static void readScene(Scene& scene, Engine& engine, Renderer& renderer);
    static bool checkTemplates();
    static std::string obtain_word_after_phrase(const std::string& input, const std::string& phrase);
    static void findScene(std::string scenePath);
    static std::string extractSceneName(const std::string& path);
private:
    static bool readBoolFromJSON(const rapidjson::Document& document, const std::string& memberName, bool defaultValue);
    static int readIntFromJSON(const rapidjson::Document& document, const std::string& memberName, int defaultValue);
    static double readDoubleFromJSON(const rapidjson::Value& value, const std::string& memberName, double defaultValue);
    static float readFloatFromJSON(const rapidjson::Value& value, const std::string& memberName, float defaultValue);
    static std::string readStringFromJSON(const rapidjson::Document& document, const std::string& memberName, const std::string& defaultValue);
    static std::vector<std::string> readStringArrayFromJSON(const rapidjson::Document& document, const std::string& memberName);
    static std::string loadInAudio(const rapidjson::Document& document, const std::string audio_property);
};


#endif /* Input_hpp */
