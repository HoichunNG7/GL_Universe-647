#include <glad/glad.h>
#include <glfw3.h>
#include <iostream>
#include <random>
#include <math.h>

#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define PI 3.1415927

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void character_random_move();

// screen size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera initial settings
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float lastX = 400, lastY = 300;
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;
bool firstMouse = true;

// character random move settings
float delta = 0.005;
float currentX = -7.0;
float currentZ = -7.0;
float limitCoord = 8.0;

std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_real_distribution<float> distr1(-PI/2, PI/2);
std::uniform_real_distribution<float> distr2(PI/2, 3*PI/2);
std::uniform_real_distribution<float> distr3(0, PI);
std::uniform_real_distribution<float> distr4(PI, 2*PI);
float angle = distr1(eng);


// process time
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

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

int main(int argc, char** argv)
{
    // initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Universe-647", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // initializing GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST); // enabling Z-buffer

    // create vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
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
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // exception handling
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // create fragment shader
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec2 TexCoord;\n"
        "uniform sampler2D ourTexture;\n"
        "void main()\n"
        "{\n"
        "    FragColor = texture(ourTexture, TexCoord);\n"
        "}\0";
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // create program and link shaders
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success); // exception handling
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // generate texture
    unsigned int texture_soil, texture_crops, texture_tomoko;
    glGenTextures(1, &texture_soil);
    glBindTexture(GL_TEXTURE_2D, texture_soil);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("soil.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &texture_crops);
    glBindTexture(GL_TEXTURE_2D, texture_crops);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("cornfield.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &texture_tomoko);
    glBindTexture(GL_TEXTURE_2D, texture_tomoko);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("tomoko.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // configure ground
    float vertices[] = {
             20.0f,   20.0f, 0.0f,   1.0f, 1.0f,
             20.0f,  -20.0f, 0.0f,   1.0f, 0.0f,
            -20.0f,  -20.0f, 0.0f,   0.0f, 0.0f,
            -20.0f,   20.0f, 0.0f,   0.0f, 1.0f
    };
    unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
    };
    
    unsigned int EBO_soil, VBO_soil, VAO_soil;
    glGenVertexArrays(1, &VAO_soil);
    glGenBuffers(1, &VBO_soil);
    glGenBuffers(1, &EBO_soil);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO_soil);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_soil);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_soil);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // configure others
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
        glm::vec3(10.0f,  1.0f, 10.0f)
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int VBO_char, VAO_char;
    glGenVertexArrays(1, &VAO_char);
    glGenBuffers(1, &VBO_char);

    glBindVertexArray(VAO_char);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_char);
    glBufferData(GL_ARRAY_BUFFER, sizeof(character_vertices), character_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // update camera
        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

        // draw ground
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindTexture(GL_TEXTURE_2D, texture_soil);
        glBindVertexArray(VAO_soil);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // draw others
        projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glBindTexture(GL_TEXTURE_2D, texture_crops);
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // draw character
        projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        character_random_move(); // calculate current position
        glBindTexture(GL_TEXTURE_2D, texture_tomoko);
        glBindVertexArray(VAO_char);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(currentX, 1.6f, currentZ));
        model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
        modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO_soil);
    glDeleteBuffers(1, &VBO_soil);
    glDeleteBuffers(1, &EBO_soil);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}

void character_random_move() {
    float newX = currentX + delta * cos(angle);
    float newZ = currentZ + delta * sin(angle);

    if (newX < -8)
    {     
        angle = distr1(eng);
    }
    else if (newX > 8)
    {
        angle = distr2(eng);
    }
    else if (newZ < -8)
    {
        angle = distr3(eng);
    }
    else if (newZ > 8)
    {
        angle = distr4(eng);
    }
    else
    {
        currentX = newX;
        currentZ = newZ;
        return;
    }

    currentX = currentX + delta * cos(angle);
    currentZ = currentZ + delta * sin(angle);

    return;
}