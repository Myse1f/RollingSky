#pragma once

#include <GL/glut.h>

class particle {
    bool is_forever;//����
    bool has_tex;//�������ɫ
    float x, y;//λ��
    float size_x;//��С
    float size_y;
    unsigned int texture;//����
    float speed_x;//�ٶ�
    float speed_y;
    float acc_x;//���ٶ�
    float acc_y;
    float life;//����
    float angle;//�Ƕ�
    unsigned char color[3];//��ɫ
    friend class emitter;
    void draw(GLuint texture);//�������ӣ�����
    void draw();//�������ӣ���ɫ��
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
    float x1, y1, x2, y2; //������λ��
    int speed;//��������
    particle **p;//��������
    particle* (*f)(); //��ʼ�����ӵĺ���ָ��
public:
    void emit(particle* (init)());
    void update();
    emitter(int _speed, float _x1, float _x2, float _y1, float _y2);
};


