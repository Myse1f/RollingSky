#pragma once

#include <GL/glut.h>

class particle {
    bool is_forever;//永生
    bool has_tex;//纹理或颜色
    float x, y;//位置
    float size_x;//大小
    float size_y;
    unsigned int texture;//纹理
    float speed_x;//速度
    float speed_y;
    float acc_x;//加速度
    float acc_y;
    float life;//生命
    float angle;//角度
    unsigned char color[3];//颜色
    friend class emitter;
    void draw(GLuint texture);//绘制粒子（纹理）
    void draw();//绘制粒子（颜色）
    void show();
public:
    particle();
    particle(float _size_x, float _size_y,\
		float _speed_x, float _speed_y,\
		float _acc_x, float _acc_y,\
		float _life, float _angle, unsigned int _texture, bool _is_forever);
    ~particle();
    particle(float _size_x, float _size_y,\
		float _speed_x, float _speed_y,\
		float _acc_x, float _acc_y,\
		float _life, float _angle, unsigned char* _color, bool _is_forever);
};

class emitter {
    float x1, y1, x2, y2; //发射器位置
    int speed;//发射速率
    particle **p;//发射粒子
    particle* (*f)(); //初始化粒子的函数指针
public:
    void emit(particle* (init)());
    void update();
    emitter(int _speed, float _x1, float _x2, float _y1, float _y2);
};


