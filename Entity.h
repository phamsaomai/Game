#ifndef ENTITY_H
#define ENTITY_H

#include <SDL.h>

struct Entity {
    SDL_Rect rect;
    int vx = 0;
    int vy = 0;

    void move();
    bool checkCollision(const SDL_Rect& other) const;
};

#endif
