#include "firework.h"
#include "time.h"

particle* init_particle1()
{
    float size = rand() % 50 * 0.08f;
    unsigned char color[] = { 1,0,0 };
    particle* p = new particle(size, size, float(rand() % 10 - 4)/10, float(rand() % 10 - 4)/10,
                               0, -4.9 / 5000, rand() % 2000 * 0.035, 0, color, false);
    return p;
}

particle* init_particle2()
{
    float size = rand() % 50 * 0.08f;;
    unsigned char color[] = { 1,1,0 };
    particle* p = new particle(size, size, float(rand() % 10 - 4)/10, float(rand() % 10 - 4)/10,
                               0, -4.9 / 5000, rand() % 2000 * 0.04, 20, color, false);
    return p;
}

particle* init_particle3()
{
    float size = rand() % 50 * 0.08f;;
    unsigned char color[] = { 1,0,1 };
    particle* p = new particle(size, size, float(rand() % 10 - 4)/10, float(rand() % 10 - 4)/10,
                               0, -4.9 / 5000, rand() % 2000 * 0.05, 15, color, false);
    return p;
}
firework::firework(double _z): z(_z)
{
    srand(unsigned(time(NULL)));
    e1 = new emitter(1500, 0, 0, 100, 100);
    e1->emit(init_particle1);
    e2 = new emitter(1500, -200, -200, 120, 120);
    e2->emit(init_particle2);
    e3 = new emitter(1500, 210, 210, 150, 150);
    e3->emit(init_particle3);
}


firework::~firework()
{
    delete e1;
    delete e2;
    delete e3;
}

void firework::draw()
{
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glColor4f(1.0, 1.0, 1.0, 1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(0, 0, z);
    e1->update();
    e2->update();
    e3->update();
    glPopMatrix();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}


