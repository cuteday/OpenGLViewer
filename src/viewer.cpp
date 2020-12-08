#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "filter.hpp"

#include <iostream>

#define FILTER_ENABLED 0

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);
void setLighting(Shader shader);

// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

const char *PATH_SHADER_VERTEX = "/Users/cappu/Public/Projects/OpenGLViewer/shader/lighting.vert";
const char *PATH_SHADER_FRAG = "/Users/cappu/Public/Projects/OpenGLViewer/shader/lighting.frag";
const char *PATH_SHADER_SCREEN_VERTEX = "/Users/cappu/Public/Projects/OpenGLViewer/shader/filter.vert";
const char *PATH_SHADER_SCREEN_FRAG = "/Users/cappu/Public/Projects/OpenGLViewer/shader/filter.frag";
const char *PATH_SHADER_SCREEN_BLUR = "/Users/cappu/Public/Projects/OpenGLViewer/shader/kernel.frag";

const char *PATH_MODEL_NANOSUIT = "/Users/cappu/Public/Projects/OpenGLViewer/model/nanosuit/nanosuit.obj";
const char *PATH_MODEL_REPLICA = "/Users/cappu/Public/Projects/OpenGLViewer/model/Replica/apartment_0/mesh.ply";
const char *PATH_MODEL_SCENENET = "/Users/cappu/Public/Projects/OpenGLViewer/model/SceneNetData/1Office/66office_scene.obj";

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

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

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGLViewer", NULL, NULL);
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

	// hide mouse and restrict cursor to this window...
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	// glEnable(GL_CULL_FACE); // face culling

	Model ourModel(PATH_MODEL_NANOSUIT);
	Shader shader(PATH_SHADER_VERTEX, PATH_SHADER_FRAG);
	Shader screenShader(PATH_SHADER_SCREEN_VERTEX, PATH_SHADER_SCREEN_BLUR);
	Filter filter(SCR_WIDTH, SCR_HEIGHT);

	shader.use();
	setLighting(shader);

	// draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

#if(FILTER_ENABLED)
		filter.toTexture();			// switch framebuffer to screen texture... 
#endif
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

        shader.use();

		// view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);	
        // render the loaded model
		shader.setVec3("viewPos", camera.Position);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);
        ourModel.Draw(shader);
#if(FILTER_ENABLED)
		filter.Draw(screenShader);
#endif
		glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(UPWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWNWARD, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
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
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void setLighting(Shader shader){
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.0, 15.0, 0.0),
	};
	
	shader.setVec3("pointLights[0].position", pointLightPositions[0]);
	shader.setVec3("pointLights[0].ambient", glm::vec3(0.80f));
	shader.setVec3("pointLights[0].diffuse", 0.9f, 0.9f, 0.9f);
	shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[0].constant", 1.0f);
	shader.setFloat("pointLights[0].linear", 0.09);
	shader.setFloat("pointLights[0].quadratic", 0.032);
	
	// since we don't have texture now...
	shader.setFloat("material.shininess", 5.0);
	shader.setVec3("material.diffuse", glm::vec3(1.0));
	shader.setVec3("material.specular", glm::vec3(0.0));
}