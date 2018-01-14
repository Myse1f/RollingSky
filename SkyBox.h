#pragma once

class SkyBox {
public:
    SkyBox();

    ~SkyBox();

    void Initialize(float size);

    bool LoadTextures(const char *top, const char *bottom, const char *front, const char *back, const char *left, const char *right);

    void Render(float cameraX, float cameraY, float cameraZ);

    void Release();

    enum {
        SKY_TOP,
        SKY_BOTTOM,
        SKY_FRONT,
        SKY_BACK,
        SKY_LEFT,
        SKY_RIGHT
    };

protected:
    unsigned int m_textures[6];        // 6 texture objects
    float m_size;
};

