#include<iostream>
#include <glad/glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Model.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int camera_mode = 1;
int shading_mode = 1;

float car_offset = 0.0;

glm::vec3 cameraPos = glm::vec3(0.0f, -30.0, 5.0);
glm::vec3 cameraTarget = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

glm::vec3 up = glm::vec3(0.0, 0.0, 0.1);

glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

glm::mat4 view;
glm::mat4 projection;
glm::mat4 model_s;
glm::mat4 model_c;

int main(int argc, char** argv)
{
	GLFWwindow* window;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(800, 600, "Project no 4", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create a window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}
    glEnable(GL_DEPTH_TEST);

	Shader shader = Shader(".\\Shaders\\shader1\\shader.vs", ".\\Shaders\\shader1\\shader.fs");
    shader.use(); 
    
    Model scene_model = Model(".\\Objects\\scene\\scene.obj");
    Model car_model = Model(".\\Objects\\car\\car.obj");

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        process_input(window);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        view = glm::lookAt(cameraPos, glm::vec3(0, 0, 0), cameraUp);
        projection = glm::perspective(glm::radians(100.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f);

        int modelLoc = glGetUniformLocation(shader.ID, "view");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(view));

        int modeLoc1 = glGetUniformLocation(shader.ID, "projection");
        glUniformMatrix4fv(modeLoc1, 1, GL_FALSE, glm::value_ptr(projection));

        int modeLoc2 = glGetUniformLocation(shader.ID, "model");
        glUniformMatrix4fv(modeLoc1, 1, GL_FALSE, glm::value_ptr(model));

        shader.use();
        scene_model.Draw(shader);
        car_model.Draw(shader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
    else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        camera_mode = 1;
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        camera_mode = 2;
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        camera_mode = 3;
    }
    else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        shading_mode = 1;
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        shading_mode = 2;
    }
    else if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        shading_mode = 3;
    }
}