#include "Entity.h"
#include <SDL.h>

void Entity::move() {
    rect.x += vx;
    rect.y += vy;
}

bool Entity::checkCollision(const SDL_Rect& other) const {
    return SDL_HasIntersection(&rect, &other);
}
