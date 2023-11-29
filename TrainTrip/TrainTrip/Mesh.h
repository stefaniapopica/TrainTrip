#ifndef MESH_H
#define MESH_H


#include <glm.hpp>
#include <string>
#include <vector>

using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh
{
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

   

private:
    unsigned int VBO, EBO;
    void setupMesh();
};

#endif
