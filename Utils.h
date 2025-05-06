#ifndef UTILS_H
#define UTILS_H

#include "Entity.h"
#include <vector>
#include "CarLane.h"

bool checkTreeOverlap(int x, int y, const std::vector<Entity>& trees);
bool checkCarOverlap(int x, int y, const std::vector<CarLane>& carLanes);
void spawnObstacles(int row, std::vector<CarLane>& carLanes, std::vector<Entity>& trees);

#endif
