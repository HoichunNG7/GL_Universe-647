#include <glad/glad.h>
#include <glfw3.h>
#include <iostream>
#include <random>
#include <math.h>

#include "stb_image.h"
#include "parameter_config.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void character_random_move();
bool check_collision(float ax, float az, float aSize, float bx, float bz, float bSize);
void create_shader(unsigned int& shader, const int shader_type, const char** source);

std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_real_distribution<float> distr1(-PI/2, PI/2);
std::uniform_real_distribution<float> distr2(PI/2, 3*PI/2);
std::uniform_real_distribution<float> distr3(0, PI);
std::uniform_real_distribution<float> distr4(PI, 2*PI);
float angle = distr1(eng);

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
    unsigned int vertexShader, reducedVertexShader;
    create_shader(vertexShader, GL_VERTEX_SHADER, &vertexShaderSource);
    create_shader(reducedVertexShader, GL_VERTEX_SHADER, &reducedVertexShaderSource);

    // create fragment shader
    unsigned int fragmentShader, illumModelFragmentShader, lightFragmentShader;
    create_shader(fragmentShader, GL_FRAGMENT_SHADER, &fragmentShaderSource);
    create_shader(illumModelFragmentShader, GL_FRAGMENT_SHADER, &illumModelFragmentShaderSource);
    create_shader(lightFragmentShader, GL_FRAGMENT_SHADER, &lightFragmentShaderSource);

    // create program and link shaders
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // glAttachShader(shaderProgram, lightFragmentShader);
    glLinkProgram(shaderProgram);

    int  success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success); // exception handling
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    // glDeleteShader(lightFragmentShader);

    // generate texture
    unsigned int texture_soil, texture_crops, texture_tomoko;
    unsigned int texture_bearing[4];

    glGenTextures(1, &texture_soil);
    glBindTexture(GL_TEXTURE_2D, texture_soil);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("img/soil.jpg", &width, &height, &nrChannels, 0);
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
    data = stbi_load("img/cornfield.jpg", &width, &height, &nrChannels, 0);
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
    data = stbi_load("img/tomoko.jpg", &width, &height, &nrChannels, 0);
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

    for (int i = 0; i < 4; i++)
    {
        glGenTextures(1, &(texture_bearing[i]));
        glBindTexture(GL_TEXTURE_2D, texture_bearing[i]);
        data = stbi_load(bearing_filenames[i], &width, &height, &nrChannels, 0);
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
    }

    // configure ground
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

    unsigned int EBO_brn, VBO_brn, VAO_brn;
    glGenVertexArrays(1, &VAO_brn);
    glGenBuffers(1, &VBO_brn);
    glGenBuffers(1, &EBO_brn);
    
    glBindVertexArray(VAO_brn);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_brn);
    glBufferData(GL_ARRAY_BUFFER, sizeof(brn_vertices), brn_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_brn);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
        int colorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4f(colorLocation, 0.5f, 0.5f, 0.5f, 1.0f);

        glBindTexture(GL_TEXTURE_2D, texture_soil);
        glBindVertexArray(VAO_soil);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // draw bearing signs
        model = glm::mat4(1.0f);

        for (int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, brnPositions[i]);
            model = glm::rotate(model, glm::radians((i+1) * 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            viewLoc = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            projLoc = glGetUniformLocation(shaderProgram, "projection");
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            colorLocation = glGetUniformLocation(shaderProgram, "ourColor");
            glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);

            glBindTexture(GL_TEXTURE_2D, texture_bearing[i]);
            glBindVertexArray(VAO_brn);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        // draw others
        projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glBindTexture(GL_TEXTURE_2D, texture_crops);
        glBindVertexArray(VAO);
        bool judgeCollision;
        for (unsigned int i = 0; i < 10; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            colorLocation = glGetUniformLocation(shaderProgram, "ourColor");
            judgeCollision = check_collision(currentX-0.8, currentZ-0.8, 1.6, cubePositions[i][0]-1, cubePositions[i][2]-1, 2);
            if (judgeCollision)
            {
                glUniform4f(colorLocation, 2.0f, 2.0f, 2.0f, 1.0f);
            }
            else
            {
                glUniform4f(colorLocation, 0.7f, 0.7f, 0.7f, 1.0f);
            }

            glDrawArrays(GL_TRIANGLES, 0, 36);
        } // crops

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
        colorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);

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
    glm::vec3 tempPos;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        tempPos = cameraPos + cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        tempPos = cameraPos - cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        tempPos = cameraPos - glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        tempPos = cameraPos + glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (tempPos[1] < 0.005 || tempPos[1] > 20)
        return;
    else if (tempPos[0] < -19.995 || tempPos[0] > 19.995)
    {
        tempPos[0] = 0 - tempPos[0];
    }
    else if (tempPos[2] < -19.995 || tempPos[2] > 19.995)
    {
        tempPos[2] = 0 - tempPos[2];
    }

    cameraPos = tempPos;
    return;
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

void character_random_move() 
{
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

bool check_collision(float ax, float az, float aSize, float bx, float bz, float bSize) 
{
    bool collisionX = ax + aSize >= bx && bx + bSize >= ax;
    bool collisionZ = az + aSize >= bz && bz + bSize >= az;

    return collisionX && collisionZ;
}

void create_shader(unsigned int& shader, const int shader_type, const char** source)
{
    shader = glCreateShader(shader_type);

    glShaderSource(shader, 1, source, NULL);
    glCompileShader(shader);

    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); // exception handling
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (shader_type == GL_VERTEX_SHADER)
        {
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        else
        {
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    }

    return;
}