#pragma once

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class ShaderD {
public:
    GLuint ID;
    ShaderD(const char* vertexPath, const char* fragmentPath);
    void use();
    bool isCompiled() const;

private:
    bool compiled;
    void checkCompileErrors(GLuint shader, std::string type);
};