//
// Created by kira on 17-10-31.
//


#ifndef _UTIL_H
#define _UTIL_H

#include <GL/glut.h>
#include <cstdio>
#include <string>


#define BMP_Header_Length 54  //图像数据在内存块中的偏移量

void loadTexture(const char *file_name, GLuint &texture);

int power_of_two(int n);

std::string& trim(std::string &s);

#endif //_UTIL_H

