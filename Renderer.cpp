#include "Renderer.hpp"
#include "Camera.hpp"

#define SCENE_UNIT 100

Renderer::Renderer() {
    game_title = "";
    width = 640;
    height = 360;
    clear_colors = std::make_tuple(255, 255, 255);
    renderer_sdl = nullptr;
}

Renderer::~Renderer() {
    // Clean up textures
    for (auto& pair : textures) {
        SDL_DestroyTexture(pair.second);
    }
    textures.clear();
}

void Renderer::LoadLibraries(){
    // error check
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        exit(0);
    }
    if (IMG_Init(IMG_INIT_PNG) == 0){
        std::cerr << "SDL image couldn't initialize SDL_Error: " << SDL_GetError() << std::endl;
        exit(0);
    }
    if (TTF_Init() < 0){
        std::cerr << "SDL ttf couldn't initialize SDL_Error: " << SDL_GetError() << std::endl;
        exit(0);
    }
    if (AudioHelper::Mix_OpenAudio498(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        // 44100 frequency, default bit depth, stereo sound, chunk size
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        exit(0);
    }

}

void Renderer::Initialize() {
	// create window
	SDL_Window* window = Helper::SDL_CreateWindow498(
		game_title.c_str(),			// window title
		SDL_WINDOWPOS_UNDEFINED,	// initial x position
		SDL_WINDOWPOS_UNDEFINED, 	// initial y position
		width, 						// width, in pixels
		height, 					// height, in pixels
		SDL_WINDOW_SHOWN 			// flags
	);
	// null return = error
	if (window == NULL) {
		std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return;
	}

	// Create renderer for window, first driver available, no flags
	renderer_sdl = Helper::SDL_CreateRenderer498(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (renderer_sdl == NULL) {
		std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
	}
}

void Renderer::StartFrame() {
    // Initialize renderer color
    SDL_SetRenderDrawColor(renderer_sdl, std::get<0>(clear_colors), std::get<1>(clear_colors), std::get<2>(clear_colors), 255);
    // Always clear framebuffer at beginning of frame
    SDL_RenderClear(renderer_sdl);
    //std::cout << "buffer cleared" << std::endl;
}

void Renderer::EndFrame() {
    // Update screen
    Helper::SDL_RenderPresent498(renderer_sdl);
}

void Renderer::Cleanup() {
    //Mix_Quit();
    TTF_Quit();
    SDL_Quit();
    IMG_Quit();
}

void Renderer::ProcessEvents() {
    SDL_Event next_event;
    while (Helper::SDL_PollEvent498(&next_event)) {
        // if next_event is now filled with data about a new event
        if (next_event.type == SDL_QUIT) {
            SDL_SetRenderDrawColor(renderer_sdl, std::get<0>(clear_colors), std::get<1>(clear_colors), std::get<2>(clear_colors), 255);
            SDL_RenderClear(renderer_sdl);
            Helper::SDL_RenderPresent498(renderer_sdl);
            exit(0); // quit
        }
        else if (next_event.type == SDL_SCANCODE_RIGHT) {
            // move player?
        }
        // event types/corresponding actions
    }
}

SDL_Texture* Renderer::LoadImage(const std::string& imageName) {
    std::string imagePath = "resources/images/" + imageName + ".png";
    // if file path doesn't exist, exit
    if (!std::filesystem::exists(imagePath)){
        std::cout << "error: missing image " << imageName;
        exit(0);
    }
    SDL_Texture* texture = IMG_LoadTexture(renderer_sdl, imagePath.c_str());
    // if no texture was able to load, exit
    if (!texture) {
        std::cerr << "error: image failed to load " << imageName << ". SDL Error: " << IMG_GetError();
        exit(0);
    }
    return texture;
}

SDL_Texture* Renderer::LoadText(const std::string& text) {
    // Create surface from the text
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    if (!textSurface) {
        std::cerr << "Unable to create text surface. SDL_ttf Error: " << TTF_GetError() << std::endl;
        return nullptr;
    }

    // Create texture from surface
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer_sdl, textSurface);
    if (!textTexture) {
        std::cerr << "Unable to create texture from surface. SDL Error: " << SDL_GetError() << std::endl;
    }

    // Clean up the surface
    SDL_FreeSurface(textSurface);

    return textTexture;
}
SDL_Texture* Renderer::getImagePointer(const std::string& imageName){
    // Load and cache texture if not already loaded
    if (textures.find(imageName) == textures.end()) {
        textures[imageName] = LoadImage(imageName);
    }
    SDL_Texture* texture = textures[imageName];
    return texture;
}

void Renderer::RenderImage(const std::string& imageName) {
    SDL_Texture* texture = getImagePointer(imageName);
    if (texture) {
        // Render the texture
        SDL_Rect dstRect = {0, 0, width, height};
        SDL_RenderCopy(renderer_sdl, texture, NULL, &dstRect);
        //std::cout << "rendered image " << imageName << std::endl;
    }
}

void Renderer::RenderActor(Actor& actor, glm::vec2 cameraPosition, double zoomFactor){
    Direction currDirection = South;
    //don't render actor who has no image
    if (!actor.getViewImage()) return;
    //if no movemet flip don't get direction
    if (shouldMovementFlip){
        currDirection = actor.getXDirection();
    }
    SDL_Texture* texture = actor.getViewImage();
    // could store in actor class to decrease num of function calls
    int textureWidth, textureHeight;
    SDL_QueryTexture(texture, nullptr, nullptr, &textureWidth, &textureHeight);

    SDL_Rect dstRect;
    SDL_Point* pivotPoint = actor.getPivotSDLPoint();

    double adjustedPosX = ((actor.getPosition().x - cameraPosition.x) * SCENE_UNIT) - pivotPoint->x;
    double adjustedPosY = ((actor.getPosition().y - cameraPosition.y) * SCENE_UNIT) - pivotPoint->y;
    double animationX = adjustedPosX + actor.getAnimationOffset().x;
    double animationY = adjustedPosY + actor.getAnimationOffset().y;
    dstRect.x = static_cast<int>(std::round(animationX + (width * 0.5) / zoomFactor));
    dstRect.y = static_cast<int>(std::round(animationY + (height * 0.5) / zoomFactor));

    dstRect.w = textureWidth * std::abs(actor.getTransformScale().x);
    dstRect.h = textureHeight * std::abs(actor.getTransformScale().y);
    
    SDL_RendererFlip currFlip = actor.getFlip();
    if (currDirection == West){
        if (currFlip == SDL_FLIP_NONE){
            currFlip = SDL_FLIP_HORIZONTAL;
        }
        else {
            currFlip = SDL_FLIP_NONE;
        }
    }
    
    Helper::SDL_RenderCopyEx498(actor.getActorID(), actor.getActorName(), renderer_sdl, actor.getViewImage(), nullptr, &dstRect, actor.getTransformRotationDegrees(), pivotPoint, currFlip);
    
}

void Renderer::RenderCollider(Actor &actor, glm::vec2 cameraPosition, const Collider &collider){
    // Set the renderer's draw color to red
    SDL_SetRenderDrawColor(renderer_sdl, 255, 0, 0, SDL_ALPHA_OPAQUE); // RGBA

    // Calculate the screen position of the collider
    SDL_Rect colliderRect;
    colliderRect.x = static_cast<int>((collider.left - cameraPosition.x) * SCENE_UNIT + (width / 2));
    colliderRect.y = static_cast<int>((collider.top - cameraPosition.y) * SCENE_UNIT + (height / 2));
    colliderRect.w = static_cast<int>(collider.width * SCENE_UNIT);
    colliderRect.h = static_cast<int>(collider.height * SCENE_UNIT);

    // Draw the collider rectangle
    SDL_RenderDrawRect(renderer_sdl, &colliderRect);

    SDL_SetRenderDrawColor(renderer_sdl, std::get<0>(clear_colors), std::get<1>(clear_colors), std::get<2>(clear_colors), 255);
}

void Renderer::RenderText(const std::string& text, int x, int y){
    if (textures.find(text) == textures.end()) {
        textures[text] = LoadText(text);
    }
    SDL_Texture* texture = textures[text];
    if (texture) {
        // Render the texture
        int textWidth = 0;
        int textHeight = 0;
        SDL_QueryTexture(texture, NULL, NULL, &textWidth, &textHeight);
        SDL_Rect renderQuad = {x, y, textWidth, textHeight};

        // Render the texture to the renderer
        SDL_RenderCopy(renderer_sdl, texture, NULL, &renderQuad);
        //std::cout << "rendered text " << text << std::endl;
    }
}

void Renderer::RenderHUD(const std::string& hp_image, int health, int score){
    SDL_RenderSetScale(renderer_sdl, 1.0, 1.0);
    SDL_Texture* hpTexture = getImagePointer(hp_image);
    // Query the health icon texture to get its width and height
    int imgWidth, imgHeight;
    SDL_QueryTexture(hpTexture, nullptr, nullptr, &imgWidth, &imgHeight);
    
    // Render health icons based on the player's health
    for (int i = 0; i < health; ++i) {
        int x = 5 + i * (imgWidth + 5);
        int y = 25;
        SDL_Rect dstRect = {x, y, imgWidth, imgHeight};
        SDL_RenderCopy(renderer_sdl, hpTexture, NULL, &dstRect);
        //std::cout << "rendered img " << hp_image << std::endl;
    }
    
    std::string scoreText = "score : " + std::to_string(score);
    RenderText(scoreText, 5, 5);

    
}

void Renderer::setGameTitle(const std::string& title_in) {
	game_title = title_in;
}

void Renderer::setWidth(int width_in) {
	width = width_in;
}
void Renderer::setHeight(int height_in) {
	height = height_in;
}
void Renderer::setMovementFlip(bool shouldMovementFlip_in) {
    shouldMovementFlip = shouldMovementFlip_in;
}
void Renderer::setClearColors(int r, int g, int b) {
	clear_colors = std::make_tuple(r, g, b);
}

void Renderer::setFont(TTF_Font* font_in){
    font = font_in;
}

SDL_Renderer* Renderer::getRendererSDL(){
    return renderer_sdl;
}

int Renderer::getHeight(){
    return height;
}

int Renderer::getWidth(){
    return width;
}

