#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ShaderLoader {
public:
    static GLuint loadShader(const std::string& vertexPath, const std::string& fragmentPath);

private:
    static std::string loadFile(const std::string& path);
    static GLuint compileShader(const std::string& shaderCode, GLenum shaderType);
    static GLuint createShaderProgram(GLuint vertexShader, GLuint fragmentShader);
};

#endif // SHADER_LOADER_H
