#ifndef CARLANE_H
#define CARLANE_H

#include "Entity.h"
#include <vector>

const Uint32 REGENERATION_INTERVAL = 5000;
struct CarLane {
    int laneY;
    int speed;
    Uint32 lastSpawnTime;
    std::vector<Entity> cars;

    CarLane(int y, int speed);
    void regenerateCars(Uint32 currentTime);
};

#endif
