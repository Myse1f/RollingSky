//
// Created by kira on 18-1-3.
//

#ifndef ROLLING_SKY_BOX_H
#define ROLLING_SKY_BOX_H

#include <gl/freeglut.h>

class Box {
private:
    GLfloat length, width, height;
    GLuint thisTexture;
public:
    static GLuint texture[3];
    GLfloat x, y, z;

    Box(GLfloat l, GLfloat w, GLfloat h, GLfloat cx, GLfloat cy, GLfloat cz, GLuint texture);

    void draw(bool shadow);
};


#endif //ROLLING_SKY_BOX_H
