#include "snow.h"
#include "util.h"
#include <time.h>

#define WIDTH 500
#define HEIGHT 650

GLuint texture;

particle *init_particle() {
    float size = rand() % 100 * 0.075f;

    particle *p = new particle(size, size, float(rand() % 10 - 4) / 30, -float(rand() % 10) / 10,
                               1.0 / 10000, -4.9 / 40000, rand() % 100, rand() % 360, texture, false);
    return p;
}

snow::snow(float _x1, float _x2, float _y1, float _y2) :
        x1(_x1), x2(_x2), y1(_y1), y2(_y2) {
    srand(time(NULL));
    e = new emitter(500, _x1, _x2, _y1, _y2);
	loadTexture("texture/snow.bmp", texture);
    e->emit(init_particle);
}


snow::~snow() {
    delete e;
}

void snow::show() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    e->update();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}
