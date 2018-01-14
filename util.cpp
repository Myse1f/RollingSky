//
// Created by kira on 18-1-6.
//

#include "util.h"

// ����power_of_two�����ж�һ�������ǲ���2����������
int power_of_two(int n) {
    if (n <= 0)
        return 0;
    return (n & (n - 1)) == 0;
}

void loadTexture(const char *file_name, GLuint &texture) {
    GLint width, height, total_bytes;
    GLubyte *pixels = nullptr;

    // ���ļ������ʧ�ܣ�����
    FILE *pFile = fopen(file_name, "rb");
    if (pFile == nullptr)
        return;

    // ��ȡ�ļ���ͼ��Ŀ�Ⱥ͸߶�
    fseek(pFile, 0x0012, SEEK_SET);
    fread(&width, 4, 1, pFile);
    fread(&height, 4, 1, pFile);
    fseek(pFile, BMP_Header_Length, SEEK_SET);

    // ����ÿ��������ռ�ֽ����������ݴ����ݼ����������ֽ���
    {
        GLint line_bytes = width * 3;
        while (line_bytes % 4 != 0)
            ++line_bytes;
        total_bytes = line_bytes * height;
    }

    // �����������ֽ��������ڴ�
    pixels = (GLubyte *) malloc((size_t) total_bytes);
    if (pixels == nullptr) {
        fclose(pFile);
        return;
    }

    // ��ȡ��������
    if (fread(pixels, (size_t) total_bytes, 1, pFile) <= 0) {
        free(pixels);
        fclose(pFile);
        return;
    }

    // �Ծ;ɰ汾�ļ��ݣ����ͼ��Ŀ�Ⱥ͸߶Ȳ��ǵ������η�������Ҫ��������
    // ��ͼ���߳�����OpenGL�涨�����ֵ��Ҳ����
    {
        GLint max;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
        if (!power_of_two(width)
            || !power_of_two(height)
            || width > max
            || height > max) {
            const GLint new_width = 256;
            const GLint new_height = 256; // �涨���ź��µĴ�СΪ�߳���������
            GLint new_line_bytes, new_total_bytes;
            GLubyte *new_pixels = nullptr;

            // ����ÿ����Ҫ���ֽ��������ֽ���
            new_line_bytes = new_width * 3;
            while (new_line_bytes % 4 != 0)
                ++new_line_bytes;
            new_total_bytes = new_line_bytes * new_height;

            // �����ڴ�
            new_pixels = (GLubyte *) malloc((size_t) new_total_bytes);
            if (new_pixels == nullptr) {
                free(pixels);
                fclose(pFile);
                return;
            }

            // ������������
            gluScaleImage(GL_RGB,
                          width, height, GL_UNSIGNED_BYTE, pixels,
                          new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

            // �ͷ�ԭ�����������ݣ���pixelsָ���µ��������ݣ�����������width��height
            free(pixels);
            pixels = new_pixels;
            width = new_width;
            height = new_height;
        }
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);

    free(pixels);
    fclose(pFile);
}

std::string& trim(std::string &s)
{
    if (s.empty())
    {
        return s;
    }

    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}
