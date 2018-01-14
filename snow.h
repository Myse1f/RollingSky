#pragma once
#include "particle.h"

class snow
{
public:
    snow(float _x1, float _x2, float _y1, float _y3);
    ~snow();
    void show();

private:
    float x1, x2, y1, y2;
    emitter *e;

};

