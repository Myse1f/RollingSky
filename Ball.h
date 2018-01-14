//
// Created by kira on 18-1-1.
//

#ifndef ROLLING_SKY_BALL_H
#define ROLLING_SKY_BALL_H


#include <GL/freeglut.h>
#include "Maths/Maths.h"

#define BALL_RADIUS 40
#define BALL_INIT_HEIGHT 83.26

class Ball {
    GLdouble maxHeight;
public:
    int direction;   // 方向
    bool isJump;
    bool isFall;
    GLdouble x, y, z;

    // 构造函数
    Ball(GLdouble x, GLdouble y, GLdouble z) :
            maxHeight(y + BALL_RADIUS * 2), direction(1),
            x(x), y(y), z(z), isJump(false), isFall(false) {
    }

    // 更新和绘制方法
    void update();

    void draw(bool shadow);

};

extern VECTOR3D cameraPosition;
extern GLdouble look[3];
extern bool gameOver;
extern int speed;

#endif //ROLLING_SKY_BALL_H
