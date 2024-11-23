#include "ShaderLoader.hpp"

std::string ShaderLoader::loadFile(const std::string& path) {
    std::ifstream file(path);
    std::stringstream buffer;
    if (file.is_open()) {
        buffer << file.rdbuf();
        file.close();
    } else {
        std::cerr << "Error reading file: " << path << std::endl;
    }
    return buffer.str();
}

GLuint ShaderLoader::compileShader(const std::string& shaderCode, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    const char* shaderSource = shaderCode.c_str();
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

GLuint ShaderLoader::createShaderProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    return shaderProgram;
}

GLuint ShaderLoader::loadShader(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode = loadFile(vertexPath);
    std::string fragmentCode = loadFile(fragmentPath);

    GLuint vertexShader = compileShader(vertexCode, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentCode, GL_FRAGMENT_SHADER);

    return createShaderProgram(vertexShader, fragmentShader);
}
