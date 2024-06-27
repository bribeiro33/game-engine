//
//  Utility.hpp
//  game_engine
//
//  Created by Barbara Ribeiro on 2/2/24.
//

#ifndef Utility_hpp
#define Utility_hpp
#include <functional>
#include <iostream>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <utility>
#include "glm/glm.hpp"
#ifdef __APPLE__
    #include "SDL2/SDL.h"
    #include "SDL2/SDL_events.h"
    #include "SDL2_ttf/SDL_ttf.h"
#else
    #include "SDL.h"
    #include "SDL_events.h"
    #include "SDL_ttf.h"
#endif

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2> &pair) const {
        auto hash1 = std::hash<T1>{}(pair.first);
        auto hash2 = std::hash<T2>{}(pair.second);
        return hash1 ^ hash2;
    }
};

//bool operator==(const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) {
//    return lhs.first == rhs.first && lhs.second == rhs.second;
//}

struct glm_hash {
    std::size_t operator()(const glm::vec2& vec) const {
        auto hash1 = std::hash<int>{}(vec.x);
        auto hash2 = std::hash<int>{}(vec.y);
        return hash1 ^ (hash2 << 1); // Shift hash2 to ensure a unique combination
    }
};

static uint64_t create_composite_key(int x, int y) {
    // cast to ensure the ints become exactly 32 bits in size
    uint32_t ux = static_cast<uint32_t> (x);
    uint32_t uy = static_cast<uint32_t> (y);

    // place x into right 32 bits
    uint64_t result = static_cast<uint64_t>(ux);

    // move to left 32 bits
    result = result << 32;

    // place y into right 32 bits
    result = result | static_cast<uint64_t>(uy);
    return result;
}

static TTF_Font* LoadFont(const std::string& fontPath){
    // size 16
    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), 16);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError();
        exit(0);
    }
    return font;
}

#endif /* Utility_hpp */
