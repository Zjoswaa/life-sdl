#include "cell.h"

Cell::Cell() {
    isAlive = false;
    isAliveNext = false;
    rect = {0, 0, CELL_WIDTH, CELL_HEIGHT};
}

Cell::Cell(const float x, const float y) {
    isAlive = false;
    isAliveNext = false;
    rect = {x, y, CELL_WIDTH, CELL_HEIGHT};
}

Cell::Cell(float x, float y, bool isAlive) {
    this->isAlive = isAlive;
    isAliveNext = false;
    rect = {x, y, CELL_WIDTH, CELL_HEIGHT};
}

void Cell::flip() {
    isAlive = !isAlive;
}
