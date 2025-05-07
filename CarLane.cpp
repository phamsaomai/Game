#include "CarLane.h"
#include <cstdlib>
#include <ctime>

CarLane::CarLane(int y, int speed) : laneY(y), speed(speed), lastSpawnTime(0) {}

void CarLane::regenerateCars(Uint32 currentTime) {
    if (currentTime - lastSpawnTime >= REGENERATION_INTERVAL) {
        int x1 = 640 + rand() % 100;
        Entity car;
        car.rect = { x1, laneY, 80, 40 };
        car.vx = -speed;
        cars.push_back(car);
        lastSpawnTime = currentTime;
    }
}

