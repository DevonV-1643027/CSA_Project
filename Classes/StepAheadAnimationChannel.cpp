#include "../Headers/StepAheadAnimationChannel.h"

// Implementation of StepAheadAnimationChannel methods (if needed)

void StepAheadAnimationChannel::update(float deltaTime) {
    // Basic update logic for StepAheadAnimationChannel
}

void StepAheadAnimationChannel::render(const glm::mat4& view, const glm::mat4& projection) {
    // Basic render logic for StepAheadAnimationChannel
}

bool StepAheadAnimationChannel::importObject(const std::string& filePath) {
    // Check if the file has a .obj extension
    if (filePath.size() < 4 || filePath.substr(filePath.size() - 4) != ".obj") {
        std::cerr << "File is not an OBJ file: " << filePath << std::endl;
        return false;
    }
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;
    std::vector<glm::vec2> temp_uvs;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (prefix == "vt") {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        }
        else if (prefix == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (prefix == "f") {
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            char slash; // to handle the '/' characters in the face definitions
            for (int i = 0; i < 3; i++) {
                iss >> vertexIndex[i] >> slash >> uvIndex[i] >> slash >> normalIndex[i];
                vertexIndices.push_back(vertexIndex[i]);
                uvIndices.push_back(uvIndex[i]);
                normalIndices.push_back(normalIndex[i]);
            }
        }
    }

    vertices = temp_vertices;
    normals = temp_normals;
    uvs = temp_uvs;

    return true;
}
