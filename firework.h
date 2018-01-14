#pragma once

#include "particle.h"
#include "util.h"

class firework
{
public:
    firework(double _z);
    ~firework();
    void draw();
private:
    emitter *e1, *e2, *e3;
    double size;
    double z;
};

