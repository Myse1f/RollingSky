#include "Box.h"

Box::Box(GLfloat l, GLfloat h, GLfloat w, GLfloat cx, GLfloat cy, GLfloat cz, GLuint texture) :
        length(l), height(h), width(w), x(cx), y(cy), z(cz), thisTexture(texture) {
}

void Box::draw(bool shadow) {
    glEnable(GL_TEXTURE_2D);
    if (!shadow) {
        glBindTexture(GL_TEXTURE_2D, thisTexture);        //使用贴图纹理
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
    }
    else {
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }

    GLfloat mat_ambient[4] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat mat_diffuse[4] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat mat_specular[4] = { 1.0, 1.0, 1.0, 1.0 };

    glShadeModel(GL_SMOOTH);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 64.0);

    glPushMatrix();        //压入变换矩阵
    glTranslatef(x, y, z);
    glBegin(GL_QUADS);  //启用四边形带绘制模式绘制

    // 绘制前面，先确定纹理坐标，然后是确定点的位置
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-length/ 2, -height/ 2, width/ 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(length/ 2, -height/ 2, width/ 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(length/ 2, height/ 2, width/ 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-length/ 2, height/ 2, width/ 2);

    // 绘制后面
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-length/ 2, -height/ 2, -width/ 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-length/ 2, height/ 2, -width/ 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(length/ 2, height/ 2, -width/ 2);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(length/ 2, -height/ 2, -width/ 2);

    // 上面
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-length/ 2, height/ 2, -width/ 2);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-length/ 2, height/ 2, width/ 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(length/ 2, height/ 2, width/ 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(length/ 2, height/ 2, -width/ 2);

    //底面
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-length/ 2, -height/ 2, -width/ 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(length/ 2, -height/ 2, -width/ 2);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(length/ 2, -height/ 2, width/ 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-length/ 2, -height/ 2, width/ 2);

    // 右面
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(length/ 2, -height/ 2, -width/ 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(length/ 2, height/ 2, -width/ 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(length/ 2, height/ 2, width/ 2);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(length/ 2, -height/ 2, width/ 2);

    // 左面
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-length/ 2, -height/ 2, -width/ 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-length/ 2, -height/ 2, width/ 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-length/ 2, height/ 2, width/ 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-length/ 2, height/ 2, -width/ 2);

    glEnd();

    glPopMatrix(); //弹出变换矩阵
    if(!shadow)
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    //glDisable(GL_TEXTURE_2D);
}
