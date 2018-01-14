#include "ObjReader.h"
#include "util.h"

using namespace std;


ObjReader::ObjReader()
{
}

void ObjReader::loadFromFile(string objfile, string mtlfile)
{
    loadMtlFile(mtlfile);
    loadObjFile(objfile);
    transToVAO();
}

void ObjReader::DrawObj(bool shadow)
{
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );

    for(int i=0; i<m_mtls.size();i++)
    {
        if (shadow) {
            glEnable(GL_TEXTURE_2D);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        }
        else if(m_mtls[i]->hasTexture)
        {

            glEnable(GL_TEXTURE_2D);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
            glBindTexture(GL_TEXTURE_2D,m_mtls[i]->texture);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            //glColor3f(1.0f,1.0f,1.0f);
        }
        else
        {
            glDisable(GL_TEXTURE_2D);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glColor3fv(m_mtls[i]->Kd);
        }

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_mtls[i]->Ka);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_mtls[i]->Kd);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_mtls[i]->Ks);
        //glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_mtls[i]->Ns);
        //glColor3fv(m_mtls[i]->Kd);


        glVertexPointer(3,GL_DOUBLE,0,m_mtls[i]->triVertexs);
        glNormalPointer(GL_DOUBLE,0,m_mtls[i]->triNormals);
        glTexCoordPointer(3,GL_DOUBLE,0,m_mtls[i]->triTextures);
        glDrawArrays(GL_TRIANGLES,0,m_mtls[i]->triNum*3);

        glVertexPointer(3,GL_DOUBLE,0,m_mtls[i]->quadVertexs);
        glNormalPointer(GL_DOUBLE,0,m_mtls[i]->quadNormals);
        glTexCoordPointer(3,GL_DOUBLE,0,m_mtls[i]->quadTextures);
        glDrawArrays(GL_QUADS,0,m_mtls[i]->quadNum*4);

        glVertexPointer(3, GL_DOUBLE, 0, m_mtls[i]->polyVertexs);
        glNormalPointer(GL_DOUBLE, 0, m_mtls[i]->polyNormals);
        glTexCoordPointer(3, GL_DOUBLE, 0, m_mtls[i]->polyTextures);
        GLuint *indices = new GLuint[m_mtls[i]->polyNum * 8];
        for (int j = 0; j < m_mtls[i]->polyNum; j++) {
            indices[j * 8 + 0] = j * 6 + 0;
            indices[j * 8 + 1] = j * 6 + 1;
            indices[j * 8 + 2] = j * 6 + 2;
            indices[j * 8 + 3] = j * 6 + 3;
            indices[j * 8 + 4] = j * 6 + 3;
            indices[j * 8 + 5] = j * 6 + 4;
            indices[j * 8 + 6] = j * 6 + 5;
            indices[j * 8 + 7] = j * 6 + 0;
        }
        glDrawElements(GL_QUADS, m_mtls[i]->polyNum * 8, GL_UNSIGNED_INT, indices);
        delete [] indices;
    }

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    //glDisable(GL_TEXTURE_2D);
}

void ObjReader::loadObjFile(std::string filename)
{
    ifstream ifs(filename);
    string s;

    if (!ifs.is_open()) {
        cout << "Error: obj file can not be opened!" << endl;
        exit(-2);
    }

    int id = -1;	//current material id
    while (getline(ifs, s)) {
        if (s[0] == '#')
            continue;

        istringstream in(s);
        string head;
        in >> head;

        if (head == "v") {
            Point *tmp = new Point();
            in >> tmp->x >> tmp->y >> tmp->z;
            vertexs.push_back(*tmp);
        }
        else if (head == "vt") {
            Point *tmp = new Point();
            in >> tmp->x >> tmp->y;
            tmp->z = 0.0;
            textures.push_back(*tmp);
        }
        else if (head == "vn") {
            Point *tmp = new Point();
            in >> tmp->x >> tmp->y >> tmp->z;
            normals.push_back(*tmp);
        }
        else if (head == "usemtl") {
            string name;
            in >> name;
            for (int i = 0; i < m_mtls.size(); i++) {
                if (name == m_mtls[i]->mtlName) {
                    id = i;
                    break;
                }
            }
        }
        else if (head == "f") {
            int num = 0;
            trim(s);
            for (int k = s.size() - 1; k >= 0; k--) {
                if (s[k] == ' ')
                    num++;
                if (s[k] == '/')
                    s[k] = ' ';
            }
            istringstream in(s);
            int data;
            string tmp;
            in >> tmp;
            while (in >> data) {
                if (num == 4) //quad
                    m_mtls[id]->quadFaces.push_back(data);
                else if (num == 3){ //tri
                    m_mtls[id]->triFaces.push_back(data);
                }
                else if (num == 6) {
                    m_mtls[id]->polyFaces.push_back(data);
                }
            }
        }
    }

}

void ObjReader::loadMtlFile(std::string filename)
{
    ifstream ifs(filename);
    string s;

    if (!ifs.is_open()) {
        cout << "Error: obj file can not be opened!" << endl;
        exit(-2);
    }

    int index = -1; //current material index
    while (getline(ifs, s)) {
        if (s[0] == '#')
            continue;

        istringstream in(s);
        string head;
        in >> head;

        if (head == "newmtl") {
            Material *pMtl = new Material();
            in >> pMtl->mtlName;
            m_mtls.push_back(pMtl);
            index = m_mtls.size() - 1;
        }
        else if (head == "Ns") {
            in >> m_mtls[index]->Ns;
        }
        else if (head == "Ka") {
            in >> m_mtls[index]->Ka[0];
            in >> m_mtls[index]->Ka[1];
            in >> m_mtls[index]->Ka[2];
        }
        else if (head == "Kd") {
            in >> m_mtls[index]->Kd[0];
            in >> m_mtls[index]->Kd[1];
            in >> m_mtls[index]->Kd[2];
        }
        else if (head == "Ks") {
            in >> m_mtls[index]->Ks[0];
            in >> m_mtls[index]->Ks[1];
            in >> m_mtls[index]->Ks[2];
        }
        else if (head == "map_Kd") {
            string name;
            in >> name;
            string tPath = "texture/" + name;
            m_mtls[index]->hasTexture = true;
            loadTexture(tPath.c_str(), m_mtls[index]->texture);
        }
    }
}

void ObjReader::transToVAO()
{
    int sum = 0;
    for (int i = 0; i<m_mtls.size(); i++)
    {
        // 四角面和三角面的个数:
        m_mtls[i]->quadNum = m_mtls[i]->quadFaces.size() / 12;
        m_mtls[i]->triNum = m_mtls[i]->triFaces.size() / 9;
        m_mtls[i]->polyNum = m_mtls[i]->polyFaces.size() / 18;

        m_mtls[i]->triVertexs = new GLdouble[m_mtls[i]->triNum * 3 * 3];
        m_mtls[i]->triTextures = new GLdouble[m_mtls[i]->triNum * 3 * 3];
        m_mtls[i]->triNormals = new GLdouble[m_mtls[i]->triNum * 3 * 3];

        m_mtls[i]->quadVertexs = new GLdouble[m_mtls[i]->quadNum * 4 * 3];
        m_mtls[i]->quadTextures = new GLdouble[m_mtls[i]->quadNum * 4 * 3];
        m_mtls[i]->quadNormals = new GLdouble[m_mtls[i]->quadNum * 4 * 3];

        m_mtls[i]->polyVertexs = new GLdouble[m_mtls[i]->polyNum * 6 * 3];
        m_mtls[i]->polyTextures = new GLdouble[m_mtls[i]->polyNum * 6 * 3];
        m_mtls[i]->polyNormals = new GLdouble[m_mtls[i]->polyNum * 6 * 3];

        sum += (m_mtls[i]->triNum + m_mtls[i]->quadNum + m_mtls[i]->polyNum);

        for (int j = 0; j<m_mtls[i]->triNum; j++)  // 第几个三角面
        {

            for (int k = 0; k<3; k++) // 第j个三角面的第k个点
            {
                m_mtls[i]->triVertexs[j * 3 * 3 + k * 3 + 0] = vertexs[m_mtls[i]->triFaces[j * 9 + k * 3 + 0] - 1].x;
                m_mtls[i]->triVertexs[j * 3 * 3 + k * 3 + 1] = vertexs[m_mtls[i]->triFaces[j * 9 + k * 3 + 0] - 1].y;
                m_mtls[i]->triVertexs[j * 3 * 3 + k * 3 + 2] = vertexs[m_mtls[i]->triFaces[j * 9 + k * 3 + 0] - 1].z;

                m_mtls[i]->triTextures[j * 3 * 3 + k * 3 + 0] = textures[m_mtls[i]->triFaces[j * 9 + k * 3 + 1] - 1].x;
                m_mtls[i]->triTextures[j * 3 * 3 + k * 3 + 1] = textures[m_mtls[i]->triFaces[j * 9 + k * 3 + 1] - 1].y;
                m_mtls[i]->triTextures[j * 3 * 3 + k * 3 + 2] = textures[m_mtls[i]->triFaces[j * 9 + k * 3 + 1] - 1].z;

                m_mtls[i]->triNormals[j * 3 * 3 + k * 3 + 0] = normals[m_mtls[i]->triFaces[j * 9 + k * 3 + 2] - 1].x;
                m_mtls[i]->triNormals[j * 3 * 3 + k * 3 + 1] = normals[m_mtls[i]->triFaces[j * 9 + k * 3 + 2] - 1].y;
                m_mtls[i]->triNormals[j * 3 * 3 + k * 3 + 2] = normals[m_mtls[i]->triFaces[j * 9 + k * 3 + 2] - 1].z;

                //cout << "tri : k=" << k << " j=" << j << endl;
            }

        }

        for (int j = 0; j<m_mtls[i]->quadNum; j++)  // 第几个四边形
        {
            for (int k = 0; k<4; k++) // 第j个六边形的第k个点
            {
                m_mtls[i]->quadVertexs[j * 4 * 3 + k * 3 + 0] = vertexs[m_mtls[i]->quadFaces[j * 12 + k * 3 + 0] - 1].x;
                m_mtls[i]->quadVertexs[j * 4 * 3 + k * 3 + 1] = vertexs[m_mtls[i]->quadFaces[j * 12 + k * 3 + 0] - 1].y;
                m_mtls[i]->quadVertexs[j * 4 * 3 + k * 3 + 2] = vertexs[m_mtls[i]->quadFaces[j * 12 + k * 3 + 0] - 1].z;

                m_mtls[i]->quadTextures[j * 4 * 3 + k * 3 + 0] = textures[m_mtls[i]->quadFaces[j * 12 + k * 3 + 1] - 1].x;
                m_mtls[i]->quadTextures[j * 4 * 3 + k * 3 + 1] = textures[m_mtls[i]->quadFaces[j * 12 + k * 3 + 1] - 1].y;
                m_mtls[i]->quadTextures[j * 4 * 3 + k * 3 + 2] = textures[m_mtls[i]->quadFaces[j * 12 + k * 3 + 1] - 1].z;


                m_mtls[i]->quadNormals[j * 4 * 3 + k * 3 + 0] = normals[m_mtls[i]->quadFaces[j * 12 + k * 3 + 2] - 1].x;
                m_mtls[i]->quadNormals[j * 4 * 3 + k * 3 + 1] = normals[m_mtls[i]->quadFaces[j * 12 + k * 3 + 2] - 1].y;
                m_mtls[i]->quadNormals[j * 4 * 3 + k * 3 + 2] = normals[m_mtls[i]->quadFaces[j * 12 + k * 3 + 2] - 1].z;

                //cout << "quad : k=" << k << " j=" << j << endl;
            }
        }

        for (int j = 0; j<m_mtls[i]->polyNum; j++)  // 第几个六边形
        {
            for (int k = 0; k<6; k++) // 第j个六边形的第k个点
            {
                m_mtls[i]->polyVertexs[j * 6 * 3 + k * 3 + 0] = vertexs[m_mtls[i]->polyFaces[j * 18 + k * 3 + 0] - 1].x;
                m_mtls[i]->polyVertexs[j * 6 * 3 + k * 3 + 1] = vertexs[m_mtls[i]->polyFaces[j * 18 + k * 3 + 0] - 1].y;
                m_mtls[i]->polyVertexs[j * 6 * 3 + k * 3 + 2] = vertexs[m_mtls[i]->polyFaces[j * 18 + k * 3 + 0] - 1].z;

                m_mtls[i]->polyTextures[j * 6 * 3 + k * 3 + 0] = textures[m_mtls[i]->polyFaces[j * 18 + k * 3 + 1] - 1].x;
                m_mtls[i]->polyTextures[j * 6 * 3 + k * 3 + 1] = textures[m_mtls[i]->polyFaces[j * 18 + k * 3 + 1] - 1].y;
                m_mtls[i]->polyTextures[j * 6 * 3 + k * 3 + 2] = textures[m_mtls[i]->polyFaces[j * 18 + k * 3 + 1] - 1].z;


                m_mtls[i]->polyNormals[j * 6 * 3 + k * 3 + 0] = normals[m_mtls[i]->polyFaces[j * 18 + k * 3 + 2] - 1].x;
                m_mtls[i]->polyNormals[j * 6 * 3 + k * 3 + 1] = normals[m_mtls[i]->polyFaces[j * 18 + k * 3 + 2] - 1].y;
                m_mtls[i]->polyNormals[j * 6 * 3 + k * 3 + 2] = normals[m_mtls[i]->polyFaces[j * 18 + k * 3 + 2] - 1].z;

                //cout << "poly : k=" << k << " j=" << j << endl;
            }
        }
    }
}


