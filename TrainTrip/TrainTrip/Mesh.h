#ifndef MESH_H
#define MESH_H


#include <glm.hpp>
#include <string>
#include <vector>

using namespace std;

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
