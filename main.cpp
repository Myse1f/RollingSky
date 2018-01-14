#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "GLee/GLee.h"
#include <GL/freeglut.h>
#include "util.h"
#include "ObjReader.h"
#include "SkyBox.h"
#include "Ball.h"
#include "firework.h"
#include "snow.h"
#include "Maths/Maths.h"
#include "Box.h"
#include "const.h"
#ifndef GL_Lib
#ifdef _DEBUG
#define GL_Lib(name) name "d.lib"
#else
#define GL_Lib(name) name ".lib"
#endif
#endif
#pragma comment(lib, GL_Lib("freeglut_static"))
#pragma comment(lib, "legacy_stdio_definitions.lib")

#define SNOWMANPOS (-15500.0)

GLdouble look[3] = {0.0, 70.0, 0.0};
//Camera & light positions
VECTOR3D cameraPosition(0.0f, 330.0f, 300.0f);
VECTOR3D lightPosition(200.0f, 300.0f, 100.0f);


//Size of shadow map
const int shadowMapSize = 512;
//Textures
GLuint shadowMapTexture;

//Matrices
MATRIX4X4 lightProjectionMatrix, lightViewMatrix;
MATRIX4X4 cameraProjectionMatrix, cameraViewMatrix;

GLdouble rotAngle[3] = {0, 0, 0};
int buttonDown, oldx, oldy;
ObjReader *objReader, *objReader1, *objReader2;
SkyBox skyBox;
Ball ball(0, BALL_INIT_HEIGHT, -BALL_RADIUS);
Box *barrierBoxes[BARRIER_NUM];
int currentBarrier = 0, currentEmpty = 0, currentJump = 0;
firework *fw;
snow *sw;
double angle = 0;
GLuint Box::texture[3];
GLuint gameOverTexture;
bool gameOver=false;
int speed = 15;

GLfloat light_amb[] = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat light_diff[] = {0.7f, 0.7f, 0.7f, 1.0f};
GLfloat light_spec[] = {0.9f, 0.9f, 0.9f, 1.0f};


void initialize() {
    fw = new firework(-154 * 100 - 500);
    sw = new snow(0, WIDTH, HEIGHT, HEIGHT);
    skyBox.Initialize(5000);
    skyBox.LoadTextures("images//lostatseanight_top.tga", "images//lostatseanight_bottom.tga",
                        "images//lostatseanight_front.tga", "images//lostatseanight_back.tga",
                        "images//lostatseanight_left.tga", "images//lostatseanight_right.tga");
    objReader1 = new ObjReader();
    objReader2 = new ObjReader();
    objReader = new ObjReader();
    objReader->loadFromFile("rollingsky_map.obj", "rollingsky_map.mtl");
    objReader1->loadFromFile("snowcookiejar.obj", "snowcookiejar.mtl");
    objReader2->loadFromFile("snowcookjarlid.obj", "snowcookjarlid.mtl");

    loadTexture("texture/1.bmp", Box::texture[0]);   // 障碍物的纹理
    loadTexture("texture/2.bmp", Box::texture[1]);
    loadTexture("texture/3.bmp", Box::texture[2]);
    for (int i = 0; i < BARRIER_NUM; i++) {
        barrierBoxes[i] = new Box(BARRIER_LENGTH, BARRIER_HEIGHT, BARRIER_WIDTH,
                                  barriers[i][0] * BRICK_LENGTH, 0, -barriers[i][1] * BRICK_WIDTH,
                                  Box::texture[rand() % 3]);
    }

    loadTexture("images/gameOver.bmp", gameOverTexture);


    glLightfv(GL_LIGHT0, GL_POSITION, VECTOR4D(lightPosition)); //指定第0号光源的位置
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb); //GL_AMBIENT表示各种光线照射到该材质上，
    //经过很多次反射后最终遗留在环境中的光线强度（颜色）
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff); //漫反射后
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_spec);//镜面反射后
    glEnable(GL_LIGHTING); //在后面的渲染中使用光照

    //Check for necessary extensions
    if (!GLEE_ARB_depth_texture || !GLEE_ARB_shadow) {
        printf("I require ARB_depth_texture and ARB_shadow extensionsn\n");
        return;
    }

    //Load identity modelview
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //Shading states
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    //Depth states
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);

    //We use glScale when drawing the scene
    glEnable(GL_NORMALIZE);

    //Create the shadow map texture
    glGenTextures(1, &shadowMapTexture);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapSize, shadowMapSize, 0,
                 GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    //
    glPushMatrix();

    glLoadIdentity();
    gluPerspective(90, (GLdouble) WIDTH / HEIGHT, 0.01, 1000000);
    glGetFloatv(GL_MODELVIEW_MATRIX, cameraProjectionMatrix);

    glLoadIdentity();
    gluPerspective(120.0f, 1.0f, 0.1f, 1000.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, lightProjectionMatrix);

    glPopMatrix();
}


void displayBarriers(bool shadow) {
    glPushMatrix();
    glTranslatef(BARRIER_INIT_X, BARRIER_INIT_Y, BARRIER_INIT_Z);
    for (auto barrierBox : barrierBoxes) {
        barrierBox->draw(shadow);
    }
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (gameOver) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, WIDTH, 0, HEIGHT, 0, 10000);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glColor4f(1.0, 1.0, 1.0, 1.0f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, gameOverTexture);
        glBegin(GL_QUADS);
        glTexCoord2d(0, 0);
        glVertex3f(0, 0, -1000);
        glTexCoord2d(1, 0);
        glVertex3f(WIDTH, 0, -1000);
        glTexCoord2d(1, 1);
        glVertex3f(WIDTH, HEIGHT, -1000);
        glTexCoord2d(0, 1);
        glVertex3f(0, HEIGHT, -1000);
        glEnd();
        glutSwapBuffers();
        return;
    }

    skyBox.Render(cameraPosition.x, cameraPosition.y, cameraPosition.z);
    if (ball.z < -200.0 && ball.z > -7500.0) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, WIDTH, 0, HEIGHT, 0, 10000);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        sw->show();
    } else if (ball.z < -8000.0) {
        fw->draw();
    }

    //First pass - from light's point of view
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(lightProjectionMatrix);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(lightPosition.x, lightPosition.y, lightPosition.z, ball.x, ball.y, ball.z, 0, 1, 0);
    glGetFloatv(GL_MODELVIEW_MATRIX, lightViewMatrix);

    //Use viewport the same size as the shadow map
    glViewport(0, 0, shadowMapSize, shadowMapSize);

    //Draw back faces into the shadow map
    glCullFace(GL_FRONT);

    //Disable color writes, and use flat shading for speed
    glShadeModel(GL_SMOOTH);
    glColorMask(0, 0, 0, 0);

    //Draw the scene
    ball.draw(false);
    objReader->DrawObj(false);
    displayBarriers(false);

    //Read the depth buffer into the shadow map texture
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, shadowMapSize, shadowMapSize);

    //restore states
    glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);
    glColorMask(1, 1, 1, 1);

    //2nd pass - Draw from camera's point of view
    glClear(GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(cameraProjectionMatrix);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z, look[0], look[1], look[2], 0, 1, 0);
    glGetFloatv(GL_MODELVIEW_MATRIX, cameraViewMatrix);

    glViewport(0, 0, WIDTH, HEIGHT);

    glLightfv(GL_LIGHT0, GL_POSITION, VECTOR4D(lightPosition)); //指定第0号光源的位置
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb); //GL_AMBIENT表示各种光线照射到该材质上，
    //经过很多次反射后最终遗留在环境中的光线强度（颜色）
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff); //漫反射后
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_spec);//镜面反射后
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    //draw scene
    ball.draw(false);
    objReader->DrawObj(false);
    displayBarriers(false);


    //3rd pass draw shadow
    glLightfv(GL_LIGHT0, GL_POSITION, VECTOR4D(lightPosition));
    glLightfv(GL_LIGHT0, GL_AMBIENT, white * 0.2f);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white * 0.2f);
    glLightfv(GL_LIGHT0, GL_SPECULAR, black);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    static MATRIX4X4 biasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
                                0.0f, 0.5f, 0.0f, 0.0f,
                                0.0f, 0.0f, 0.5f, 0.0f,
                                0.5f, 0.5f, 0.5f, 1.0f);    //bias from [-1, 1] to [0, 1]
    MATRIX4X4 textureMatrix = biasMatrix * lightProjectionMatrix * lightViewMatrix;
    //Set up texture coordinate generation.
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_S, GL_EYE_PLANE, textureMatrix.GetRow(0));
    glEnable(GL_TEXTURE_GEN_S);

    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_T, GL_EYE_PLANE, textureMatrix.GetRow(1));
    glEnable(GL_TEXTURE_GEN_T);

    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_R, GL_EYE_PLANE, textureMatrix.GetRow(2));
    glEnable(GL_TEXTURE_GEN_R);

    glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_Q, GL_EYE_PLANE, textureMatrix.GetRow(3));
    glEnable(GL_TEXTURE_GEN_Q);

    //Bind & enable shadow map texture
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    glEnable(GL_TEXTURE_2D);

    //Enable shadow comparison
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);

    //Shadow comparison should be true (is in shadow) if r>texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_GREATER);

    //Shadow comparison should generate an INTENSITY result
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);

    //Set alpha test to discard false comparisons
    glAlphaFunc(GL_GEQUAL, 0.99f);
    glEnable(GL_ALPHA_TEST);

    ball.update();
    displayBarriers(true);
    objReader->DrawObj(true);

    //Disable textures and texgen
    glDisable(GL_TEXTURE_2D);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);

    //Restore other states
    glDisable(GL_LIGHTING);
    glDisable(GL_ALPHA_TEST);

    glPushMatrix();
    glTranslatef(-450, 50, SNOWMANPOS);
    glScalef(1000, 1000, 1000);
    glRotatef(angle, 0, 1, 0);

    objReader1->DrawObj(false);
    objReader2->DrawObj(false);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(450, 50, SNOWMANPOS);
    glScalef(1000, 1000, 1000);
    glRotatef(-angle, 0, 1, 0);
    objReader1->DrawObj(false);
    objReader2->DrawObj(false);
    glPopMatrix();

    glFinish();
    glutSwapBuffers();
    glutPostRedisplay();
}

void motion(int x, int y) {
    if (buttonDown == 1) {              //左键按下
        rotAngle[1] += (x - oldx) * 0.2;//绕y轴旋转角度变化
        rotAngle[0] += (y - oldy) * 0.2;//绕x轴旋转角度变化

        if (fabs(rotAngle[0]) >= 360)
            rotAngle[0] = 0;
        if (fabs(rotAngle[1] >= 360))
            rotAngle[1] = 0;
    } else if (buttonDown == 2) {          //右键按下
        rotAngle[2] -= (x - oldx) * 0.2;//绕z轴旋转
        if (fabs(rotAngle[2]) >= 360)
            rotAngle[2] = 0;
    }
    oldx = x;
    oldy = y;
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        buttonDown = 1;
        oldx = x;
        oldy = y;
    } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        buttonDown = 2;
        oldx = x;
        oldy = y;
    } else
        buttonDown = 0;
}

inline void boundCheck() {
    if (ball.x < -MAP_X_BOUND || ball.x > MAP_X_BOUND) {
        puts("out of bound!");
        gameOver = true;
        speed = 0;
//        exit(0);
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        //        case 'w':
        //        case 'W':
        //            cameraPosition[2] -= 0.1;
        //            look[2] -= 0.1;
        //            ball.z -= 0.1;
        //            break;
        //        case 's':
        //        case 'S':
        //            cameraPosition[2] += 0.1;
        //            look[2] += 0.1;
        //            ball.z += 0.1;
        //            break;
        case 'a':
        case 'A':
            ball.x -= 20;
            boundCheck();
            break;
        case 'd':
        case 'D':
            ball.x += 20;
            boundCheck();
            break;
        case ' ':
        case 'J':
        case 'j':
            ball.isJump = true;
            break;
        default:
            break;
    }
}

void globalTimer(int _) {
    // do sth
	if (ball.z < -15500)
		gameOver = true;
    ball.z -= speed;
    cameraPosition.z -= speed;
    lightPosition.z -= speed;
    look[2] -= speed;
    angle += 1.0;
    glutPostRedisplay();
    glutTimerFunc(50, globalTimer, 1);
}

bool AABB(const GLdouble *box2_min, const GLdouble *box2_max) {
    GLdouble min[3] = {ball.x - BALL_RADIUS, ball.y - BALL_RADIUS, ball.z - BALL_RADIUS};
    GLdouble max[3] = {ball.x + BALL_RADIUS, ball.y + BALL_RADIUS, ball.z + BALL_RADIUS};

    if (min[0] > box2_max[0]) return false;
    if (max[0] < box2_min[0]) return false;
    if (min[2] > box2_max[2]) return false;
    if (max[2] < box2_min[2]) return false;
    if (min[1] > box2_max[1]) return false;
    if (max[1] < box2_min[1]) return false;
    return true;
}

void collisionTest(int _) {
    //    glutPostRedisplay();
    static auto zDistance = static_cast<GLfloat>(BALL_RADIUS + BARRIER_WIDTH / 2);
    for (int i = currentBarrier; i < BARRIER_NUM; i++) {
        // 第一步进行aabb检测
        GLdouble box2_min[3] = {barriers[i][0] * BRICK_LENGTH + BARRIER_INIT_X - BARRIER_LENGTH / 2, BRICK_HEIGHT,
                                -barriers[i][1] * BRICK_WIDTH + BARRIER_INIT_Z - BARRIER_WIDTH / 2};
        GLdouble box2_max[3] = {box2_min[0] + BARRIER_LENGTH, BRICK_HEIGHT + BARRIER_HEIGHT,
                                box2_min[2] + BARRIER_WIDTH};
        //        if (i == 0) {
//                    printf("%f %f %f\n", box2_min[0], box2_min[1], box2_min[2]);
//                    printf("%f %f %f\n", box2_max[0], box2_max[1], box2_max[2]);
//                    printf("ball %f %f %f\n", ball.x, ball.y, ball.z);
        //        }
        if (AABB(box2_min, box2_max)) {
            // 需要进一步检测
            if (!ball.isJump) {
                printf("i=%d\n", i);
                printf("%f %f %f\n", box2_min[0], box2_min[1], box2_min[2]);
                printf("%f %f %f\n", box2_max[0], box2_max[1], box2_max[2]);
                printf("ball %f %f %f\n", ball.x, ball.y, ball.z);
                puts("crash!");
                gameOver = true;
                speed = 0;
            } else {
                // 计算与四条边的距离
                const GLdouble radiusSqr = BALL_RADIUS * BALL_RADIUS;
                GLdouble ySqr = (ball.y - BARRIER_INIT_Y) * (ball.y - BARRIER_INIT_Y);
                if ((ball.x - box2_min[0]) * (ball.x - box2_min[0]) + ySqr < radiusSqr ||
                    (ball.x - box2_max[0]) * (ball.x - box2_max[0]) + ySqr < radiusSqr ||
                    (ball.z - box2_min[2]) * (ball.z - box2_min[2]) + ySqr < radiusSqr ||
                    (ball.z - box2_max[2]) * (ball.z - box2_max[2]) + ySqr < radiusSqr) {
                    puts("crash!");
                    exit(0);
                }
            }
        }
        if (ball.z <= box2_min[2]) {
            // 安全度过一个障碍物
            currentBarrier = i + 1;
            printf("currentBarrier %d\n", currentBarrier);
        }
        if (ball.z > -barriers[i + 1][1] * BARRIER_WIDTH + BARRIER_INIT_Z + BARRIER_WIDTH / 2) {
            break;
        }
    }

    glutTimerFunc(50, collisionTest, 1);
}

void emptyTest(int _) {
    //    glutPostRedisplay();
    for (int i = currentEmpty; i < EMPTY_NUM; i++) {
        // x z
        // 球心落在空缺位置
        //        if (i == 8) {
        //            printf("ball %f %f\n", ball.x, ball.z);
        //            printf("pos %d %d %d %d\n", emptyPos[i][0] * BRICK_LENGTH - MAP_X_BOUND,
        //                   emptyPos[i][0] * BRICK_LENGTH + BRICK_LENGTH - MAP_X_BOUND, -emptyPos[i][1] * BRICK_WIDTH,
        //                   -emptyPos[i][1] * BRICK_WIDTH - BRICK_WIDTH);
        //        }
        if (!ball.isJump &&
            ball.x > emptyPos[i][0] * BRICK_LENGTH - MAP_X_BOUND &&
            ball.x < emptyPos[i][0] * BRICK_LENGTH + BRICK_LENGTH - MAP_X_BOUND &&
            ball.z < -emptyPos[i][1] * BRICK_WIDTH &&
            ball.z > -emptyPos[i][1] * BRICK_WIDTH - BRICK_WIDTH) {
            puts("empty!!game over");
            printf("i=%d\n", i);
            printf("ball %f %f\n", ball.x, ball.z);
            printf("pos %d %d %d %d\n", emptyPos[i][0] * BRICK_LENGTH - MAP_X_BOUND,
                   emptyPos[i][0] * BRICK_LENGTH + BRICK_LENGTH - MAP_X_BOUND,
                   -emptyPos[i][1] * BRICK_WIDTH,
                   -emptyPos[i][1] * BRICK_WIDTH - BRICK_WIDTH);
            ball.isFall = true;
            currentEmpty = EMPTY_NUM + 1;
//            gameOver = true;
//            speed = 0;
        }
        if (ball.z <= -emptyPos[i][1] * BRICK_WIDTH - BRICK_WIDTH) {
            //            printf("ball %f %d\n", ball.z, -emptyPos[i][1] * BRICK_WIDTH - BRICK_WIDTH);
            currentEmpty = i + 1;
            printf("currentEmpty %d\n", currentEmpty);
        }
        if (ball.z >= -emptyPos[i + 1][1] * BRICK_WIDTH) {
            break;
        }
    }

    glutTimerFunc(50, emptyTest, 1);
}

void jumpTest(int _) {
    for (int i = currentJump; i < JUMP_NUM; i++) {
        // x z
		if (ball.x > jumpPos[i][0] * BRICK_LENGTH - MAP_X_BOUND &&
			ball.x < jumpPos[i][0] * BRICK_LENGTH + BRICK_LENGTH - MAP_X_BOUND &&
			ball.z < -jumpPos[i][1] * BRICK_WIDTH &&
			ball.z > -jumpPos[i][1] * BRICK_WIDTH - BRICK_WIDTH &&
			ball.y == BALL_INIT_HEIGHT) {
            puts("you jump!!!");
            printf("ball %f %f\n", ball.x, ball.z);
            printf("pos %d %d %d %d\n", jumpPos[i][0] * BRICK_LENGTH - MAP_X_BOUND,
                   jumpPos[i][0] * BRICK_LENGTH + BRICK_LENGTH - MAP_X_BOUND, -jumpPos[i][1] * BRICK_WIDTH,
                   -jumpPos[i][1] * BRICK_WIDTH - BRICK_WIDTH);
            ball.isJump = true;
        }
        if (ball.z <= -jumpPos[i][1] * BRICK_WIDTH - BRICK_WIDTH) {
            //            printf("ball %f %d\n", ball.z, -emptyPos[i][1] * BRICK_WIDTH - BRICK_WIDTH);
            currentJump = i + 1;
            printf("currentJump %d\n", currentJump);
        }
        if (ball.z >= -jumpPos[i + 1][1] * BRICK_WIDTH) {
            break;
        }
    }

    glutTimerFunc(50, jumpTest, 1);
}

void ballJumpTimer(int _) {
    if (ball.isJump) {
//        printf("ball is jumping %f %d\n", ball.y, ball.direction);
        ball.y += ball.direction * BALL_RADIUS / 3.5;
    }

    glutTimerFunc(50, ballJumpTimer, 1);
}

int main(int argc, char *argv[]) {

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInit(&argc, argv);
    glutCreateWindow("Rolling Sky");
    initialize();
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glDepthFunc(GL_LEQUAL);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(50, globalTimer, 1);      // 小球向前移动
    glutTimerFunc(20, collisionTest, 1);    // 检测是否碰撞
    glutTimerFunc(20, emptyTest, 1);        // 检测是否落在空缺位置
    glutTimerFunc(20, jumpTest, 1);         // 检测是否落在跳跃点
    glutTimerFunc(50, ballJumpTimer, 1);    // 检测小球是否在跳跃
    glutMainLoop();

    return 0;
}