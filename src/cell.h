#ifndef CELL_H
#define CELL_H

#include <SDL3/SDL.h>
#include "constants.h"

class Cell {
public:
    bool isAlive;
    bool isAliveNext;
    SDL_FRect rect{};

    Cell();
    Cell(float x, float y);
    Cell(float x, float y, bool isAlive);

    void flip();
    void updateAliveStatus();
private:

};

#endif
