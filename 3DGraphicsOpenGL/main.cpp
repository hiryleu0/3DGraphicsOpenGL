#include <glad/glad.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Model.h"
#include "Camera.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void renderQuad();

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

// camera
int camera_mode = 1;
Camera camera1(glm::vec3(0.0f, 15.0f, 50.0f));
Camera camera2(glm::vec3(50.0f, 10.0f, 0.0f));
glm::vec3 cam3_pos0(0.0, 50.0, 4.0);
Camera camera3(cam3_pos0);
Camera *camera = &camera1;

//mouse
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float part_of_day = 0.5;

//shaders
Shader* flatShader;
Shader* gouradShader;
Shader* phongShader;
Shader* shaderGeometryPass;
Shader* shaderLightingPass;
Shader* shader1;
Shader* shader2;

bool deferred = false;

//movement settings
float car_speed = 2.0;
float old_car_speed = 0.0;
float car_radius = 8.0;
float car_angle = 0.0;
bool car_moving = true;
float lights_moved = 0.0; 

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    
    // shaders
    flatShader = new Shader("./Shaders/FlatShader/shader.vs", "./Shaders/FlatShader/shader.fs");
    gouradShader = new Shader("./Shaders/GouradShader/shader.vs", "./Shaders/GouradShader/shader.fs");
    phongShader = new Shader("./Shaders/PhongShader/shader.vs", "./Shaders/PhongShader/shader.fs");
    shaderGeometryPass = new Shader("./Shaders/DeferredShaders/GeometryPassShader/shader.vs", "./Shaders/DeferredShaders/GeometryPassShader/shader.fs");
    shaderLightingPass = new Shader("./Shaders/DeferredShaders/LightingPassShader/shader.vs", "./Shaders/DeferredShaders/LightingPassShader/shader.fs");
    shader1 = shader2 = flatShader;

    // ------------------------------
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedoSpec;
    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    shaderLightingPass->use();
    shaderLightingPass->setInt("gPosition", 0);
    shaderLightingPass->setInt("gNormal", 1);
    shaderLightingPass->setInt("gAlbedoSpec", 2);

    // load models
    // -----------
    Model houses("./Objects/houses/houses.obj");
    Model car("./Objects/car/car.obj");
    Model board("./Objects/board/untitled.obj");
    Model ball("./Objects/ball/ball.obj");
    Model tower("./Objects/tower/tower.obj");


    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view;


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        part_of_day = min((cos(currentFrame/3)+1), 1.0f);
        if (car_moving) car_angle += car_speed * deltaTime;
        glm::vec3 car_translation = car_radius * glm::vec3(-sin(car_angle), 0, -cos(car_angle) + 1);

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (camera_mode == 1) view = camera1.GetViewMatrix();             
        else if (camera_mode == 2) view = camera2.GetViewMatrix(car_translation);
        else
        {
            camera3.Position = car_translation + cam3_pos0;
            view = camera3.GetViewMatrix(car_translation);
        }
        
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 modelT;
        glm::mat4 modelTT;
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        modelT = glm::translate(model, car_translation);
        modelT = glm::rotate(modelT, car_angle, glm::vec3(0, 1, 0));
        modelTT = glm::rotate(modelT, lights_moved, glm::vec3(0, 1, 0));

        shader1->use();
        shader1->setMat4("projection", projection);
        shader1->setMat4("view", view);
        shader1->setMat4("model", model);
        shader2->use();
        shader2->setMat4("modelT", modelT);
        shader2->setMat4("modelTT", modelTT);
        shader2->setVec3("camCoords", camera->Position);
        shader2->setVec3("s", glm::vec3(0));
        shader2->setFloat("part_of_day", part_of_day);

        if (deferred)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        shader1->use();
        board.Draw(*shader1);
        houses.Draw(*shader1);
        ball.Draw(*shader1);
        tower.Draw(*shader1);
        shader1->setMat4("model", modelT);
        car.Draw(*shader1);

        if (deferred)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shaderLightingPass->use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
            renderQuad();
            glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (camera_mode == 1) 
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera1.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera1.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera1.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera1.ProcessKeyboard(RIGHT, deltaTime);

        shader2->setVec3("camCoords", camera->Position);
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        camera_mode = 1;
        camera = &camera1;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        camera_mode = 2;
        camera = &camera2;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        camera_mode = 3;
        camera = &camera3;
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        deferred = false;
        shader1 = flatShader;
        shader2 = flatShader;
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        deferred = false;
        shader1 = gouradShader;
        shader2 = gouradShader;
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        deferred = false;
        shader1 = phongShader;
        shader2 = phongShader;
    }
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
    {
        deferred = true;
        shader1 = shaderGeometryPass;
        shader2 = shaderLightingPass;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && car_moving)
        car_speed += 0.001;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && car_moving)
        car_speed -= 0.001;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        lights_moved += 0.005;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        lights_moved -= 0.005;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        car_moving = false;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        car_moving = true;    
}

// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (camera_mode != 1) return;
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera1.ProcessMouseMovement(xoffset, yoffset);
}

// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera->ProcessMouseScroll(yoffset);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;

float quadVertices[] = {
    // positions        // texture Coords
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};

void renderQuad()
{
    if (quadVAO == 0)
    {
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}