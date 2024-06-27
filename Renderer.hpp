#ifndef Renderer_hpp
#define Renderer_hpp
#define SDL_MAIN_HANDLED
#include <unordered_map>
#include <string>
#include <iostream>
#include <cmath>
#include "glm/glm.hpp"
#include "Directions.hpp"

#include "External/Helper.h"
#include "External/AudioHelper.h"
#ifdef __APPLE__
    #include "SDL2/SDL.h"
    #include "SDL2/SDL_events.h"
    #include "SDL2_image/SDL_image.h"
    #include "SDL2_ttf/SDL_ttf.h"
    #include "SDL2_mixer/SDL_mixer.h"
#else
    #include "SDL.h"
    #include "SDL_events.h"
    #include "SDL_image.h"
    #include "SDL_ttf.h"
    #include "SDL_mixer.h"
#endif

#include "Actor.hpp"
class Camera;
class Renderer
{
private:
    bool shouldMovementFlip = false;
	std::unordered_map<std::string, SDL_Texture*> textures;
	std::string game_title = "";
	int width = 640;
	int height = 360;
	std::tuple<int, int, int> clear_colors = std::make_tuple(255, 255, 255);
	SDL_Renderer* renderer_sdl;
    TTF_Font* font;
    SDL_Color textColor = {255, 255, 255, 255}; // white
public:
    Renderer();
    ~Renderer();
    void LoadLibraries();
	void Initialize();
	void StartFrame();
	void EndFrame();
	void Cleanup();
    void ProcessEvents();
    SDL_Texture* LoadImage(const std::string& imageName);
    SDL_Texture* LoadText(const std::string& text);
    SDL_Texture* getImagePointer(const std::string& imageName);
	void RenderImage(const std::string& imageName);
	void RenderText(const std::string& text, int x, int y);
	void RenderActor(Actor& actor, glm::vec2 cameraPosition, double zoomFactor);
	void RenderHUD(const std::string& hp_image, int health, int score);
	void setGameTitle(const std::string& title_in);
	void setWidth(int width_in);
	void setHeight(int height_in);
    void setMovementFlip(bool shouldMovementFlip);
	void setClearColors(int r, int g, int b);
    void setFont(TTF_Font* font_in);
    int getHeight();
    int getWidth();
    SDL_Renderer* getRendererSDL();
    void RenderCollider(Actor &actor, glm::vec2 cameraPosition, const Collider &collider);
};
#endif
