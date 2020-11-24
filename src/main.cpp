#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <random>
#include <math.h>

#include "stb_image.h"
#include "parameter_config.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "ply_model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void character_random_move();
void light_source_move();
bool check_collision(float ax, float az, float aSize, float bx, float bz, float bSize);
void create_shader(unsigned int& shader, const int shader_type, const char** source);
void generate_texture(unsigned int& texture_id, const char* image_filename);
void configure_object_with_ebo(unsigned int& VAO_obj, int coord_size, const float* vertex_coords, unsigned int* face_list, int v_size, int f_size);

std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_real_distribution<float> distr1(-PI/2, PI/2);
std::uniform_real_distribution<float> distr2(PI/2, 3*PI/2);
std::uniform_real_distribution<float> distr3(0, PI);
std::uniform_real_distribution<float> distr4(PI, 2*PI);
float angle = distr1(eng);

int main(int argc, char** argv)
{
    // load ply models
    auto plyBunny = new PlyModel();
    plyBunny->get_ply_model("models/bun_zipper_res4.ply");
    plyBunny->add_normal_vectors();
    // plyBunny->print_all_lists(); // test
    // plyBunny->print_bounding_box(); // test

    auto plyDragon = new PlyModel();
    plyDragon->get_ply_model("models/dragon_vrip_res4.ply");
    plyDragon->add_normal_vectors();

    auto plyHappy = new PlyModel();
    plyHappy->get_ply_model("models/happy_vrip_res4.ply");
    plyHappy->add_normal_vectors();

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
    unsigned int vertexShader, reducedVertexShader, illumVertexShader;
    create_shader(vertexShader, GL_VERTEX_SHADER, &vertexShaderSource);
    create_shader(reducedVertexShader, GL_VERTEX_SHADER, &reducedVertexShaderSource);
    create_shader(illumVertexShader, GL_VERTEX_SHADER, &illumVertexShaderSource);

    // create fragment shader
    unsigned int fragmentShader, illumModelFragmentShader, lightFragmentShader;
    create_shader(fragmentShader, GL_FRAGMENT_SHADER, &fragmentShaderSource);
    create_shader(illumModelFragmentShader, GL_FRAGMENT_SHADER, &illumModelFragmentShaderSource);
    create_shader(lightFragmentShader, GL_FRAGMENT_SHADER, &lightFragmentShaderSource);

    // create program and link shaders
    unsigned int shaderProgram, illumProgram, illumObjectProgram;

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    illumProgram = glCreateProgram();
    glAttachShader(illumProgram, reducedVertexShader);
    glAttachShader(illumProgram, lightFragmentShader);
    glLinkProgram(illumProgram);

    illumObjectProgram = glCreateProgram();
    glAttachShader(illumObjectProgram, illumVertexShader);
    glAttachShader(illumObjectProgram, illumModelFragmentShader);
    glLinkProgram(illumObjectProgram);

    int  success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success); // exception handling
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }
    glGetProgramiv(illumProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(illumProgram, 512, NULL, infoLog);
        std::cout << "ERROR::ILLUM::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(reducedVertexShader);
    glDeleteShader(lightFragmentShader);
    glDeleteShader(illumModelFragmentShader);

    // generate texture
    unsigned int texture_soil, texture_crops, texture_tomoko;
    unsigned int texture_bearing[4];

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);

    generate_texture(texture_soil, "img/soil.jpg");
    generate_texture(texture_crops, "img/cornfield.jpg");
    generate_texture(texture_tomoko, "img/tomoko.jpg");
    for (int i = 0; i < 4; i++)
    {
        generate_texture(texture_bearing[i], bearing_filenames[i]);
    }

    // configure ground
    unsigned int VAO_soil;
    configure_object_with_ebo(VAO_soil, 5, vertices, indices, sizeof(vertices), sizeof(indices));

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

    unsigned int VAO_brn;
    configure_object_with_ebo(VAO_brn, 5, brn_vertices, indices, sizeof(brn_vertices), sizeof(indices));

    // configure ply models
    unsigned int VAO_bunny, VAO_dragon, VAO_happy;
    int v_num = plyBunny->get_vertex_num();
    int f_num = plyBunny->get_face_num();
    int v_size = sizeof(float) * 6 * v_num;
    int f_size = sizeof(unsigned int) * 3 * f_num;
    configure_object_with_ebo(VAO_bunny, 6, plyBunny->get_model_vertices(), plyBunny->get_model_faces(), v_size, f_size);

    v_size = sizeof(float) * 6 * plyDragon->get_vertex_num();
    f_size = sizeof(unsigned int) * 3 * plyDragon->get_face_num();
    configure_object_with_ebo(VAO_dragon, 6, plyDragon->get_model_vertices(), plyDragon->get_model_faces(), v_size, f_size);

    v_size = sizeof(float) * 6 * plyHappy->get_vertex_num();
    f_size = sizeof(unsigned int) * 3 * plyHappy->get_face_num();
    configure_object_with_ebo(VAO_happy, 6, plyHappy->get_model_vertices(), plyHappy->get_model_faces(), v_size, f_size);

    // configure light source
    unsigned int VBO_light, VAO_light;
    glGenVertexArrays(1, &VAO_light);
    glGenBuffers(1, &VBO_light);

    glBindVertexArray(VAO_light);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_light);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // constant settings
    glUseProgram(illumObjectProgram);
    int objColorLoc = glGetUniformLocation(illumObjectProgram, "objectColor");
    int lightColorLoc = glGetUniformLocation(illumObjectProgram, "lightColor");
    int lightPosLoc;
    glUniform3f(objColorLoc, 1.0f, 0.5f, 0.31f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

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
            colorLocation = glGetUniformLocation(shaderProgram, "ourColor");
            glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);

            glBindTexture(GL_TEXTURE_2D, texture_bearing[i]);
            glBindVertexArray(VAO_brn);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        // draw others
        glBindTexture(GL_TEXTURE_2D, texture_crops);
        glBindVertexArray(VAO);
        bool judgeCollision;
        for (unsigned int i = 0; i < 9; i++)
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

        // Now we switch to 'Illumination Sector'
        glUseProgram(illumProgram);

        // draw light source
        light_source_move();
        projLoc = glGetUniformLocation(illumProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        viewLoc = glGetUniformLocation(illumProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPosition);
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        modelLoc = glGetUniformLocation(illumProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO_light);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // draw models
        glUseProgram(illumObjectProgram);

        lightPosLoc = glGetUniformLocation(illumObjectProgram, "lightPos");
        glUniform3f(lightPosLoc, lightPosition[0], lightPosition[1], lightPosition[2]);
        projLoc = glGetUniformLocation(illumObjectProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        viewLoc = glGetUniformLocation(illumObjectProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        modelLoc = glGetUniformLocation(illumObjectProgram, "model");
               
        int viewPosLoc = glGetUniformLocation(illumObjectProgram, "viewPos");
        glUniform3f(viewPosLoc, cameraPos[0], cameraPos[1], cameraPos[2]);

        model = glm::mat4(1.0f);
        model = glm::translate(model, bunnyPosition);
        model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO_bunny);
        f_num = plyBunny->get_face_num();
        glDrawElements(GL_TRIANGLES, 3 * f_num, GL_UNSIGNED_INT, 0);

        model = glm::mat4(1.0f);
        model = glm::translate(model, dragonPosition);
        model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO_dragon);
        f_num = plyDragon->get_face_num();
        glDrawElements(GL_TRIANGLES, 3 * f_num, GL_UNSIGNED_INT, 0);

        model = glm::mat4(1.0f);
        model = glm::translate(model, happyPosition);
        model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO_happy);
        f_num = plyHappy->get_face_num();
        glDrawElements(GL_TRIANGLES, 3 * f_num, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);
    glDeleteProgram(illumProgram);

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

void light_source_move()
{
    lightPosition[1] += delta / 5;
    if (lightPosition[1] >= 12)
    {
        lightPosition[1] = 0;
    }

    return;
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

void generate_texture(unsigned int& texture_id, const char* image_filename)
{
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(image_filename, &width, &height, &nrChannels, 0);
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

    return;
}

void configure_object_with_ebo(unsigned int& VAO_obj, int coord_size, const float* vertex_coords, unsigned int* face_list, int v_size, int f_size)
{
    unsigned int VBO_obj, EBO_obj;
    glGenVertexArrays(1, &VAO_obj);
    glGenBuffers(1, &VBO_obj);
    glGenBuffers(1, &EBO_obj);

    glBindVertexArray(VAO_obj);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_obj);
    glBufferData(GL_ARRAY_BUFFER, v_size, vertex_coords, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_obj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, f_size, face_list, GL_STATIC_DRAW);

    if (coord_size == 5)
    {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    else if (coord_size == 3)
    {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
    else if (coord_size == 6)
    {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    else
    {
        std::cout << "Error: Unexpected dimention of coordinates which may cause ambiguity." << std::endl;
    }
    
    return;
}
