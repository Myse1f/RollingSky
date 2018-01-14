#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <GL/freeglut.h>
//#include <GL/gl.h>
struct Point {
    double x;
    double y;
    double z;
};

class Material  
{
public:
    std::string mtlName;

    double Ns;
    float Ka[3];
    float Kd[3];
    float Ks[3];

    bool hasTexture;
    GLuint texture; 

    std::vector<int> triFaces;  
    std::vector<int> quadFaces; 
    std::vector<int> polyFaces; 

    // vertex array object;
    int quadNum;  
    int triNum;   
    int polyNum; 

    GLdouble *triVertexs;
    GLdouble *triNormals;
    GLdouble *triTextures;

    GLdouble *quadVertexs;
    GLdouble *quadNormals;
    GLdouble *quadTextures;

    GLdouble *polyVertexs;
    GLdouble *polyNormals;
    GLdouble *polyTextures;

    Material() :hasTexture(false), Ns(0.0) {}
};

class ObjReader
{
public:
    ObjReader();

    void loadFromFile(std::string objfile, std::string mtlfile);
    void DrawObj(bool shadow);
private:
    
    std::vector<Point> vertexs;
    std::vector<Point> textures;
    std::vector<Point> normals;

    std::vector<Material*> m_mtls;

    void loadObjFile(std::string filename);
    void loadMtlFile(std::string filename);
    void transToVAO();
};
