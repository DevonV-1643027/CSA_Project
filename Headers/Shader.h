#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <string>

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    void use();
    GLuint getID() const;

private:
    GLuint ID;

    std::string loadShaderSource(const std::string& filePath);
    void checkCompileErrors(GLuint shader, const std::string& type);
};

#endif // SHADER_H
