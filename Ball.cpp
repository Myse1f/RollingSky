//
// Created by kira on 18-1-1.
//

#include "Ball.h"

void Ball::draw(bool shadow) {
    // 显示小球
    glPushMatrix();

    GLfloat mat_ambient[4] = {0.95, 0.2, 0.05, 1.0};
    GLfloat mat_diffuse[4] = {0.95, 0.2, 0.05, 1.0};
    GLfloat mat_specular[4] = {1.0, 1.0, 1.0, 1.0};

    glShadeModel(GL_SMOOTH);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 64.0);

    glTranslated(x, y, z);
    if (!shadow)
        glDisable(GL_TEXTURE_2D);

    glutSolidSphere(BALL_RADIUS, 64, 64);

    if (!shadow)
        glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

void Ball::update() {
    if (isFall) {
        double delta = BALL_RADIUS / 50.0;
        y -= delta;
        cameraPosition.y -= delta;
        look[1] -= delta;
        if (y < -BALL_RADIUS) {
            gameOver = true;
            speed = 0;
        }
    } else if (isJump) {
        if (y > maxHeight) {
            y = maxHeight;
            direction = -1;
        } else if (y < BALL_INIT_HEIGHT) {
            y = BALL_INIT_HEIGHT;
            direction = 1;
            isJump = false;
        }
    }
    // 显示小球
    draw(true);
}
