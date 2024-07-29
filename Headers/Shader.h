#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <string>

class Shader {
public:
    GLuint ID;
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void use();
    void checkCompileErrors(GLuint shader, std::string type);
};

#endif // SHADER_H
