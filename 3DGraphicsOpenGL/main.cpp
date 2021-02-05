#include <glad/glad.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Shader.h"
#include "Model.h"
#include "Camera.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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

//shaders
Shader* ourShader;

//movement settings
float car_speed = 2.0;
float car_radius = 8.0;
float car_angle = 0.0;
int last_time;

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
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    ourShader = new Shader("./Shaders/shader1/shader.vs", "./Shaders/shader1/shader.fs");

    // load models
    // -----------
    Model houses("./Objects/houses/houses.obj");
    Model car("./Objects/car/car.obj");
    Model board("./Objects/board/untitled.obj");
    Model ball("./Objects/ball/ball.obj");
    Model tower("./Objects/tower/tower.obj");

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        car_angle += car_speed * deltaTime;
        processInput(window);

        // render
        // ------
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader->use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 80.0f);
        glm::mat4 view;
        glm::vec3 car_translation = car_radius * glm::vec3(-sin(car_angle), 0, -cos(car_angle) + 1);
        int modelLoc4 = glGetUniformLocation(ourShader->ID, "camCoords");
        if (camera_mode == 1)
        {
            view = camera1.GetViewMatrix();             
        }
        else if (camera_mode == 2)
        {
            view = camera2.GetViewMatrix(car_translation);
        }
        else
        {
            camera3.Position = car_translation + cam3_pos0;
            view = camera3.GetViewMatrix(car_translation);
        }
        glUniform3f(modelLoc4, camera->Position.x, camera->Position.y, camera->Position.z);


        int modelLoc = glGetUniformLocation(ourShader->ID, "projection");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(projection));

        int modelLoc2 = glGetUniformLocation(ourShader->ID, "view");
        glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(view));

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader->setMat4("model", model);
        int modelLoc3 = glGetUniformLocation(ourShader->ID, "model");
        glUniformMatrix4fv(modelLoc3, 1, GL_FALSE, glm::value_ptr(model));
        ball.Draw(*ourShader);
        houses.Draw(*ourShader);
        board.Draw(*ourShader);
        tower.Draw(*ourShader);

        
        last_time = glfwGetTime();
        model = glm::translate(model, car_translation);
        model = glm::rotate(model, car_angle, glm::vec3(0, 1, 0));
        glUniformMatrix4fv(modelLoc3, 1, GL_FALSE, glm::value_ptr(model));
        car.Draw(*ourShader);



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
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

        int modelLoc4 = glGetUniformLocation(ourShader->ID, "camCoords");
        glUniform3f(modelLoc4, camera->Position.x, camera->Position.y, camera->Position.z);

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
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        car_speed += 0.001;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        car_speed -= 0.001;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera->ProcessMouseScroll(yoffset);
}