#include "ModelLoader.hpp"
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <tiny_obj_loader.h>

ModelLoader::ModelLoader() : VAO(0), VBO(0), EBO(0) {}

ModelLoader::~ModelLoader() {
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    if (VAO) glDeleteVertexArrays(1, &VAO);
}

bool ModelLoader::loadModel(const std::string& filename) {
    std::string modelPath = AssetManager::getAssetPath(filename);
    if (modelPath.empty()) {
        return false;
    }
    
    loadOBJ(modelPath);
    
    // Generate OpenGL buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return true;
}

void ModelLoader::loadOBJ(const std::string& filename) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    // A função LoadObj espera agora um ponteiro para shapes, materiais e atributos
    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str());
    
    if (!success) {
        std::cerr << "Error loading OBJ file: " << err << std::endl;
        return;
    }

    for (size_t s = 0; s < shapes.size(); s++) {
        for (size_t v = 0; v < shapes[s].mesh.indices.size(); v++) {
            tinyobj::index_t idx = shapes[s].mesh.indices[v];

            float px = attrib.vertices[3 * idx.vertex_index + 0];
            float py = attrib.vertices[3 * idx.vertex_index + 1];
            float pz = attrib.vertices[3 * idx.vertex_index + 2];

            float r = 1.0f, g = 1.0f, b = 1.0f;
            if (attrib.colors.size() > 0) {
                r = attrib.colors[3 * idx.vertex_index + 0];
                g = attrib.colors[3 * idx.vertex_index + 1];
                b = attrib.colors[3 * idx.vertex_index + 2];
            }

            vertices.push_back(px);
            vertices.push_back(py);
            vertices.push_back(pz);
            vertices.push_back(r);
            vertices.push_back(g);
            vertices.push_back(b);

            indices.push_back(v);
        }
    }
}

void ModelLoader::draw(GLuint shaderProgram) {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
