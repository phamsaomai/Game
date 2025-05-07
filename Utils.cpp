#include "Utils.h"
#include <cstdlib>
#include <algorithm>

bool checkTreeOverlap(int x, int y, const std::vector<Entity>& trees) {
    for (const auto& tree : trees) {
        if (x < tree.rect.x + tree.rect.w && x + 40 > tree.rect.x &&
            y < tree.rect.y + tree.rect.h && y + 40 > tree.rect.y) {
            return true;
        }
    }
    return false;
}

bool checkCarOverlap(int x, int y, const std::vector<CarLane>& carLanes) {
    for (const auto& lane : carLanes) {
        for (const auto& car : lane.cars) {
            if (x < car.rect.x + car.rect.w && x + 40 > car.rect.x &&
                y < car.rect.y + car.rect.h && y + 40 > car.rect.y) {
                return true;
            }
        }
    }
    return false;
}

void spawnObstacles(int row, std::vector<CarLane>& carLanes, std::vector<Entity>& trees) {
    int y = 480 / 2 - row * 40;

    if (row >= 5) {
        std::vector<int> positions;
        while (positions.size() < 3) {
            int x = 20 + rand() % (640 - 40 - 40);


            if (!checkTreeOverlap(x, y, trees) && !checkCarOverlap(x, y, carLanes)) {
                positions.push_back(x);
            }
        }

        for (int x : positions) {
            Entity tree;
            tree.rect = { x, y, 40, 40 };
            trees.push_back(tree);
        }
    }

    if (row >= 5) {
        if (rand() % 2 == 0) {
            int speed = 2 + rand() % (4 - 2 + 1);
            CarLane newLane(y, speed);

            int x1 = 640 + rand() % 100;
            Entity car;
            car.rect = { x1, y, 80, 40 };
            car.vx = -speed;

            newLane.cars.push_back(car);
            carLanes.push_back(newLane);
        }
    }
}
