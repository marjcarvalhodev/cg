#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <tiny_obj_loader.h>

class ModelLoader {
public:
    ModelLoader();
    ~ModelLoader();

    // Load a model from a file
    bool loadModel(const std::string& filename);

    // Draw the model
    void draw(GLuint shaderProgram);

private:
    GLuint VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Helper function to load OBJ data
    void loadOBJ(const std::string& filename);
};

#endif // MODEL_LOADER_H
