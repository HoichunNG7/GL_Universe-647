#ifndef PARAMETER_CONFIG_H
#define PARAMETER_CONFIG_H

#include "glm/glm.hpp"
#define PI 3.1415927

// screen size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// character random move settings
float delta = 0.005;
float currentX = -7.0;
float currentZ = -7.0;
float limitCoord = 8.0;

// process time
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

// camera initial settings
glm::vec3 cameraPos = glm::vec3(11.0f, 1.0f, 11.0f);
glm::vec3 cameraFront = glm::vec3(1.0f, 0.0f, 1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float lastX = 400, lastY = 300;
float yaw = 45.0f;
float pitch = 0.0f;
float fov = 45.0f;
bool firstMouse = true;

// object vertices
const float cube_vertices[] = {
    -1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
     1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
    -1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
     1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,

    -1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
    -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
    -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,  1.0f, 1.0f,

     1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
     1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
     1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
     1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
     1.0f,  1.0f,  1.0f,  0.0f, 1.0f,

    -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
     1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,  1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,

    -1.0f,  1.0f, -1.0f,  1.0f, 0.5f,
     1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
     1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
     1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,  0.0f, 0.5f,
    -1.0f,  1.0f, -1.0f,  1.0f, 0.5f
};

const float character_vertices[] = {
    -1.0f, -2.0f, -1.0f,  1.0f, 0.0f,
     1.0f, -2.0f, -1.0f,  0.0f, 0.0f,
     1.0f,  2.0f, -1.0f,  0.0f, 1.0f,
     1.0f,  2.0f, -1.0f,  0.0f, 1.0f,
    -1.0f,  2.0f, -1.0f,  1.0f, 1.0f,
    -1.0f, -2.0f, -1.0f,  1.0f, 0.0f,

    -1.0f, -2.0f,  1.0f,  0.0f, 0.0f,
     1.0f, -2.0f,  1.0f,  1.0f, 0.0f,
     1.0f,  2.0f,  1.0f,  1.0f, 1.0f,
     1.0f,  2.0f,  1.0f,  1.0f, 1.0f,
    -1.0f,  2.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -2.0f,  1.0f,  0.0f, 0.0f,

    -1.0f,  2.0f,  1.0f,  1.0f, 1.0f,
    -1.0f,  2.0f, -1.0f,  0.0f, 1.0f,
    -1.0f, -2.0f, -1.0f,  0.0f, 0.0f,
    -1.0f, -2.0f, -1.0f,  0.0f, 0.0f,
    -1.0f, -2.0f,  1.0f,  1.0f, 0.0f,
    -1.0f,  2.0f,  1.0f,  1.0f, 1.0f,

     1.0f,  2.0f,  1.0f,  0.0f, 1.0f,
     1.0f,  2.0f, -1.0f,  1.0f, 1.0f,
     1.0f, -2.0f, -1.0f,  1.0f, 0.0f,
     1.0f, -2.0f, -1.0f,  1.0f, 0.0f,
     1.0f, -2.0f,  1.0f,  0.0f, 0.0f,
     1.0f,  2.0f,  1.0f,  0.0f, 1.0f,

    -1.0f, -2.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -2.0f, -1.0f,  0.0f, 1.0f,
     1.0f, -2.0f,  1.0f,  1.0f, 0.0f,
     1.0f, -2.0f,  1.0f,  1.0f, 0.0f,
    -1.0f, -2.0f,  1.0f,  1.0f, 1.0f,
    -1.0f, -2.0f, -1.0f,  0.0f, 0.0f,

    -1.0f,  2.0f, -1.0f,  1.0f, 0.5f,
     1.0f,  2.0f, -1.0f,  1.0f, 1.0f,
     1.0f,  2.0f,  1.0f,  0.0f, 1.0f,
     1.0f,  2.0f,  1.0f,  0.0f, 1.0f,
    -1.0f,  2.0f,  1.0f,  0.0f, 0.5f,
    -1.0f,  2.0f, -1.0f,  1.0f, 0.5f
};

const float brn_vertices[] = {
    4.0f,   4.0f, 0.0f,   0.0f, 1.0f,
    4.0f,  -4.0f, 0.0f,   0.0f, 0.0f,
    -4.0f,  -4.0f, 0.0f,   1.0f, 0.0f,
    -4.0f,   4.0f, 0.0f,   1.0f, 1.0f
};// bearings

const float vertices[] = {
    20.0f,   20.0f, 0.0f,   1.0f, 1.0f,
    20.0f,  -20.0f, 0.0f,   1.0f, 0.0f,
    -20.0f,  -20.0f, 0.0f,   0.0f, 0.0f,
    -20.0f,   20.0f, 0.0f,   0.0f, 1.0f
};// ground

// object positions
glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  1.0f,  0.0f),
    glm::vec3(5.0f,  1.0f, 0.0f),
    glm::vec3(5.0f, 1.0f, 5.0f),
    glm::vec3(0.0f, 1.0f, 5.0f),
    glm::vec3(-5.0f, 1.0f, 5.0f),
    glm::vec3(-5.0f,  1.0f, 0.0f),
    glm::vec3(-5.0f, 1.0f, -5.0f),
    glm::vec3(0.0f,  1.0f, -5.0f),
    glm::vec3(5.0f,  1.0f, -5.0f),
};

glm::vec3 brnPositions[] = {
    glm::vec3(20.0f,  10.0f,  0.0f),
    glm::vec3(0.0f,  10.0f, -20.0f),
    glm::vec3(-20.0f, 10.0f, 0.0f),
    glm::vec3(0.0f, 10.0f, 20.0f)
};

glm::vec3 lightPosition = glm::vec3(21.0f, 6.0f, 21.0f);

glm::vec3 bunnyPosition = glm::vec3(16.0f, 0.0f, 16.0f);

glm::vec3 dragonPosition = glm::vec3(16.0f, 0.0f, 12.0f);

glm::vec3 happyPosition = glm::vec3(12.0f, 0.0f, 16.0f);

// object indices
unsigned int indices[] = {
0, 1, 3,
1, 2, 3
};

// lighting strength & object colors
glm::vec3 modelColor = glm::vec3(1.0f, 0.5f, 0.31f);
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

// shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"out vec3 ourColor;\n"
"out vec2 TexCoord;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"   TexCoord = aTexCoord;\n"
"}\0";

const char* reducedVertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\0";

const char* illumVertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"out vec3 FragPos;\n"
"out vec3 Normal;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"    FragPos = vec3(model * vec4(aPos, 1.0));\n"
"    Normal = aNormal;\n"
"   gl_Position = projection * view * vec4(FragPos, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform vec4 ourColor;"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"    FragColor = texture(ourTexture, TexCoord) * ourColor;\n"
"}\0";

const char* illumModelFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 Normal;\n"
"in vec3 FragPos;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 viewPos;\n"
"uniform vec3 objectColor;\n"
"uniform vec3 lightColor;\n"
"void main()\n"
"{\n"
"    float ambientStrength = 0.1;\n"
"    vec3 ambient = ambientStrength * lightColor;\n"
"\n"
"    vec3 lightDir = normalize(lightPos - FragPos);\n"
"    float diff = max(dot(Normal, lightDir), 0.0);\n"
"    vec3 diffuse = diff * lightColor;\n"
"\n"
"    float specularStrength = 0.5;\n"
"    vec3 viewDir = normalize(viewPos - FragPos);\n"
"    vec3 reflectDir = reflect(-lightDir, Normal);\n"
"    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
"    vec3 specular = specularStrength * spec * lightColor;\n"
"\n"
"    vec3 result = (ambient + diffuse + specular) * objectColor;\n"
"    FragColor = vec4(result, 1.0);\n"
"}\0";

const char* lightFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0);\n"
"}\0";

// filenames
const char* bearing_filenames[] = { "img/W.jpg", "img/S.jpg", "img/E.jpg", "img/N.jpg" };

#endif