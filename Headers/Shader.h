#pragma once

#include <string>

class Shader {
public:
    GLuint ID;
    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
    bool isCompiled() const;

private:
    bool compiled;
    void checkCompileErrors(GLuint shader, std::string type);
};