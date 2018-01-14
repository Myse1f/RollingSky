#include "particle.h"



particle::particle()
{
}

particle::~particle()
{
}

particle::particle(float _size_x, float _size_y, float _speed_x, float _speed_y, float _acc_x, float _acc_y, float _life, float _angle, unsigned int _texture, bool _is_forever)
{
    size_x = _size_x;
    size_y = _size_y;
    speed_x = _speed_x;
    speed_y = _speed_y;
    acc_x = _acc_x;
    acc_y = _acc_y;
    life = _life;
    texture = _texture;
    angle = _angle;
    has_tex = true;
    is_forever = _is_forever;
}

particle::particle(float _size_x, float _size_y, float _speed_x, float _speed_y, float _acc_x, float _acc_y, float _life, float _angle, unsigned char * _color, bool _is_forever)
{
    size_x = _size_x;
    size_y = _size_y;
    speed_x = _speed_x;
    speed_y = _speed_y;
    acc_x = _acc_x;
    acc_y = _acc_y;
    life = _life;
    if (_is_forever) _life = 1;
    color[0] = *_color;
    color[1] = *(_color + 1);
    color[2] = *(_color + 2);
    angle = _angle;
    has_tex = false;
    is_forever = _is_forever;
}

void particle::draw(GLuint texture)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    const GLfloat x1 = -0.5, x2 = 0.5;
    const GLfloat y1 = -0.5, y2 = 0.5;
    const GLfloat point[4][2] = { { x1,y1 },{ x2,y1 },{ x2,y2 },{ x1,y2 } };
    int dir[4][2] = { { 0,0 },{ 1,0 },{ 1,1 },{ 0,1 } };
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0);
    for (int i = 0; i < 4; i++) {
        glTexCoord2iv(dir[i]);
        glVertex2fv(point[i]);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void particle::draw()
{
    const GLfloat x1 = -0.5, x2 = 0.5;
    const GLfloat y1 = -0.5, y2 = 0.5;
    const GLfloat point[4][2] = { { x1,y1 },{ x2,y1 },{ x2,y2 },{ x1,y2 } };
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; i++) {
        glVertex2fv(point[i]);
    }
    glEnd();
}

void particle::show() {

    if (life > 0 || is_forever) {
        if (has_tex) {
            glDepthMask(GL_FALSE);
            glPushMatrix();
            glTranslatef(x, y, 0);
            glRotatef(angle, 0, 0, 1);
            glScalef(size_x, size_y, 0);
            draw(texture);
            glPopMatrix();
            y += speed_y;
            x += speed_x;
            speed_y += acc_y;
            speed_x += acc_x;
            if (!is_forever)life -= 0.2f;
            glDepthMask(GL_TRUE);
        }
        else {
            glPushMatrix();
            glColor3f(color[0], color[1], color[2]);
            glTranslatef(x, y, 0);
            glRotatef(angle, 0, 0, 1);
            glScalef(size_x, size_y, 0);
            draw();
            glPopMatrix();
            y += speed_y;
            x += speed_x;
            speed_y += acc_y;
            speed_x += acc_x;
            if (!is_forever)life -= 0.2f;
            glColor3f(0, 0, 0);
        }
    }
}

emitter::emitter(int _speed, float _x1, float _x2, float _y1, float _y2) {
    speed = _speed;
    x1 = _x1;
    x2 = _x2;
    y1 = _y1;
    y2 = _y2;
    p = new particle*[speed];
}

void emitter::update() {
    for (int i = 0; i < speed; i++) {
        p[i]->show();
        if (p[i]->life < 0) {
            delete p[i];
            p[i] = f();
            int place = rand() % speed;
            p[i]->x = 1.0f*place / speed*(x2 - x1) + x1;
            p[i]->y = 1.0f*place / speed*(y2 - y1) + y1;
        }
    }
}

void emitter::emit(particle *(init)())
{
    for (int i = 0; i < speed; i++) {
        f = init;
        p[i] = init();
        int place = rand() % speed;
        p[i]->x = 1.0f*place / speed*(x2 - x1) + x1;
        p[i]->y = 1.0f*place / speed*(y2 - y1) + y1;
    }

}
