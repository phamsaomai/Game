#ifndef BUTTON_H
#define BUTTON_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

struct Button {
    SDL_Rect rect;
    std::string text;
    SDL_Texture* texture;
    bool hovered;
};

SDL_Texture* createTextTexture(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color);

#endif
