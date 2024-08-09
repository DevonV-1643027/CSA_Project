#include "../Headers/StickFigure.h"

StickFigure::StickFigure() {
    // Define the skeleton structure with joint names, positions, rotations, and scales
    skeleton.push_back(createJoint("Head", glm::vec3(0.0f, 0.6f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.7f, 0.5f, 1.5f)));
    skeleton.push_back(createJoint("Torso", glm::vec3(0.0f, 1.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.5f)));
    skeleton.push_back(createJoint("Left Arm", glm::vec3(-0.18f, 0.15f, 0.0f), glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(0.25f)));
    skeleton.push_back(createJoint("Right Arm", glm::vec3(0.18f, 0.15f, 0.0f), glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(0.25f)));
    skeleton.push_back(createJoint("Left Leg", glm::vec3(-0.1f, -0.55f, 0.0f), glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(0.25f, 0.7f, 0.25f)));
    skeleton.push_back(createJoint("Right Leg", glm::vec3(0.1f, -0.55f, 0.0f), glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(0.25f, 0.7f, 0.25f)));

    // Adding hands as child joints of the arms
    skeleton.push_back(createJoint("Left Hand", glm::vec3(0.0f, 0.8f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.7f, 0.5f)));
    skeleton.push_back(createJoint("Right Hand", glm::vec3(0.0f, 0.8f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.7f, 0.5f)));

    // Define parent-child relationships
    skeleton[1].childrenIndices.push_back(0); // Torso -> Head
    skeleton[1].childrenIndices.push_back(2); // Torso -> Left Arm
    skeleton[1].childrenIndices.push_back(3); // Torso -> Right Arm
    skeleton[1].childrenIndices.push_back(4); // Torso -> Left Leg
    skeleton[1].childrenIndices.push_back(5); // Torso -> Right Leg
    skeleton[2].childrenIndices.push_back(6); // Left Arm -> Left Hand
    skeleton[3].childrenIndices.push_back(7); // Right Arm -> Right Hand

    // Shaders
    shader = new Shader("../Shaders/stick_figure.vs", "../Shaders/stick_figure.fs");
    
    // Setup
    setupCylinder();
    setupSphere();
}

glm::mat4 StickFigure::getJointModelMatrix(const Joint& joint) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), joint.position);
    model *= glm::toMat4(joint.rotation);
    model = glm::scale(model, joint.scale);
    return model;
}

void StickFigure::render(const glm::mat4& view, const glm::mat4& projection) {
    if (!shader) return;

    std::function<void(int, glm::mat4)> renderJoint = [&](int jointIndex, glm::mat4 parentMatrix) {
        const auto& joint = skeleton[jointIndex];
        glm::mat4 modelMatrix = parentMatrix * getJointModelMatrix(joint);

        if (joint.name == "Head") {
            drawSphere(modelMatrix, view, projection); // Draw head or hand
        }
        else {
            drawCylinder(modelMatrix, view, projection); // Draw limbs
        }

        for (int childIndex : joint.childrenIndices) {
            renderJoint(childIndex, modelMatrix);
        }
        };

    renderJoint(1, glm::mat4(1.0f)); // Start with the torso as the root (index 1)
}


void StickFigure::drawCylinder(const glm::mat4& modelMatrix, const glm::mat4& view, const glm::mat4& projection) {
    // Set the model, view, and projection matrices in the shader
    shader->use();
    shader->setMat4("model", modelMatrix);

    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    // Define light and view positions
    glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
    glm::vec3 viewPos = glm::vec3(0.0f, 2.0f, 5.0f);

    shader->setVec3("lightPos", lightPos);
    shader->setVec3("viewPos", viewPos);

    // Bind VAO for the cylinder
    glBindVertexArray(cylinderVAO);

    // Draw the cylinder
    glDrawElements(GL_TRIANGLES, cylinderIndexCount, GL_UNSIGNED_INT, 0);

    // Unbind the VAO
    glBindVertexArray(0);
}


void StickFigure::drawSphere(const glm::mat4& modelMatrix, const glm::mat4& view, const glm::mat4& projection) {
    // Set the model, view, and projection matrices in the shader
    shader->use();
    shader->setMat4("model", modelMatrix);

    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    // Define light and view positions
    glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
    glm::vec3 viewPos = glm::vec3(0.0f, 2.0f, 5.0f);

    shader->setVec3("lightPos", lightPos);
    shader->setVec3("viewPos", viewPos);

    // Bind VAO for the sphere
    glBindVertexArray(sphereVAO);

    // Draw the sphere
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);

    // Unbind the VAO
    glBindVertexArray(0);
}



Joint StickFigure::createJoint(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
    return { name, position, rotation, scale };
}

void StickFigure::setupCylinder() {
    const int segments = 20;
    const float height = 1.0f;
    const float radius = 0.1f;
    std::vector<float> cylinderVertices;
    std::vector<unsigned int> cylinderIndices;

    // Define cylinder vertex and indices data
    for (int i = 0; i <= segments; ++i) {
        float angle = (float)i / segments * 2.0f * M_PI;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Bottom circle vertices
        cylinderVertices.push_back(x);
        cylinderVertices.push_back(-height / 2.0f);
        cylinderVertices.push_back(z);
        cylinderVertices.push_back(x);
        cylinderVertices.push_back(0.0f);
        cylinderVertices.push_back(z);

        // Top circle vertices
        cylinderVertices.push_back(x);
        cylinderVertices.push_back(height / 2.0f);
        cylinderVertices.push_back(z);
        cylinderVertices.push_back(x);
        cylinderVertices.push_back(0.0f);
        cylinderVertices.push_back(z);

        // Indices
        if (i < segments) {
            int current = i * 2;
            int next = current + 2;

            // Side triangles
            cylinderIndices.push_back(current);
            cylinderIndices.push_back(next);
            cylinderIndices.push_back(current + 1);

            cylinderIndices.push_back(next);
            cylinderIndices.push_back(next + 1);
            cylinderIndices.push_back(current + 1);
        }
    }
    cylinderIndexCount = cylinderIndices.size();

    // Generate and bind the VAO and VBO
    glGenVertexArrays(1, &cylinderVAO);
    glGenBuffers(1, &cylinderVBO);
    GLuint cylinderEBO;
    glGenBuffers(1, &cylinderEBO);

    glBindVertexArray(cylinderVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);
    glBufferData(GL_ARRAY_BUFFER, cylinderVertices.size() * sizeof(float), cylinderVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylinderIndices.size() * sizeof(unsigned int), cylinderIndices.data(), GL_STATIC_DRAW);

    // Define the vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Normal attribute
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO
}

void StickFigure::setupSphere() {
    const int latitudeSegments = 18;
    const int longitudeSegments = 36;
    const float radius = 0.5f;

    std::vector<float> sphereVertices;
    std::vector<unsigned int> sphereIndices;

    for (int lat = 0; lat <= latitudeSegments; ++lat) {
        float theta = lat * M_PI / latitudeSegments;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int lon = 0; lon <= longitudeSegments; ++lon) {
            float phi = lon * 2.0f * M_PI / longitudeSegments;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;

            // Positions
            sphereVertices.push_back(radius * x);
            sphereVertices.push_back(radius * y);
            sphereVertices.push_back(radius * z);

            // Normals
            sphereVertices.push_back(x);
            sphereVertices.push_back(y);
            sphereVertices.push_back(z);
        }
    }

    for (int lat = 0; lat < latitudeSegments; ++lat) {
        for (int lon = 0; lon < longitudeSegments; ++lon) {
            int first = (lat * (longitudeSegments + 1)) + lon;
            int second = first + longitudeSegments + 1;

            sphereIndices.push_back(first);
            sphereIndices.push_back(second);
            sphereIndices.push_back(first + 1);

            sphereIndices.push_back(second);
            sphereIndices.push_back(second + 1);
            sphereIndices.push_back(first + 1);
        }
    }

    sphereIndexCount = sphereIndices.size();

    // Generate and bind the VAO and VBO
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    GLuint sphereEBO;
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), sphereIndices.data(), GL_STATIC_DRAW);

    // Define the vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Normal attribute
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO
}

int StickFigure::getParentIndex(int jointIndex) const {
    for (size_t i = 0; i < skeleton.size(); ++i) {
        if (std::find(skeleton[i].childrenIndices.begin(), skeleton[i].childrenIndices.end(), jointIndex) != skeleton[i].childrenIndices.end()) {
            return i;
        }
    }
    return -1; // Return -1 if no parent is found (for root)
}


