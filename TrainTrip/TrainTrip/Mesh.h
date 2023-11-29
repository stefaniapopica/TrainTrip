#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
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

struct Vertex {

    glm::vec3 Position;

    glm::vec3 Normal;
   
    glm::vec2 TexCoords;
   
    glm::vec3 Tangent;
   
    glm::vec3 Bitangent;
   
    int m_BoneIDs[MAX_BONE_INFLUENCE];
   
    float m_Weights[MAX_BONE_INFLUENCE];
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
