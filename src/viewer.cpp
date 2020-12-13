#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "filter.hpp"
#include "skybox.hpp"
#include "msaa.hpp"
#include "light.hpp"

#include <iostream>
#include <string>

const bool ENABLE_FILTER = 0;
const bool ENABLE_SKYBOX = 1;
const bool ENABLE_MSAA = 1;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);
void setLighting(Shader shader);

// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

const char *PATH_SHADER_VERTEX = "/Users/cappu/Public/Projects/OpenGLViewer/shader/lighting.vert";
const char *PATH_SHADER_FRAG = "/Users/cappu/Public/Projects/OpenGLViewer/shader/lighting.frag";
const char *PATH_SHADER_NORMAL_FRAG = "/Users/cappu/Public/Projects/OpenGLViewer/shader/normal.frag";
const char *PATH_SHADER_SCREEN_VERTEX = "/Users/cappu/Public/Projects/OpenGLViewer/shader/filter.vert";
const char *PATH_SHADER_SCREEN_FRAG = "/Users/cappu/Public/Projects/OpenGLViewer/shader/kernel.frag";
const char *PATH_SHADER_SKYBOX_VERTEX = "/Users/cappu/Public/Projects/OpenGLViewer/shader/skybox.vert";
const char *PATH_SHADER_SKYBOX_FRAG = "/Users/cappu/Public/Projects/OpenGLViewer/shader/skybox.frag";

const char *PATH_TEXTURE_SKYBOX = "/Users/cappu/Public/Projects/OpenGLViewer/texture/skybox/";

const char *PATH_MODEL_NANOSUIT = "/Users/cappu/Public/Projects/OpenGLViewer/model/nanosuit/nanosuit.obj";
const char *PATH_MODEL_ROBOT = "/Users/cappu/Public/Projects/OpenGLViewer/model/halloween-little-witch/source/03/03.obj";
const char *PATH_MODEL_SCENENET = "/Users/cappu/Public/Projects/OpenGLViewer/model/SceneNetData/1Office/66office_scene.obj";
const char *PATH_MODEL_REPLICA = "/Users/cappu/Public/Projects/OpenGLViewer/model/Replica/apartment_0/mesh.ply";

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Filter *filter;
Skybox *skybox;
Shader *skyboxShader, *screenShader;
MultiSample *multiSample;

int main(){

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGLViewer", NULL, NULL);
    if (window == NULL){
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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    stbi_set_flip_vertically_on_load(false);

    glEnable(GL_DEPTH_TEST);
	// glEnable(GL_STENCIL_TEST);
	// glEnable(GL_CULL_FACE);	// face culling
	// glCullFace(GL_BACK);		// this is the default

	Model ourModel(PATH_MODEL_SCENENET);
	Shader shader(PATH_SHADER_VERTEX, PATH_SHADER_FRAG);
	
	if(ENABLE_FILTER){
		filter = new Filter(SCR_WIDTH, SCR_HEIGHT);
		screenShader = new Shader(PATH_SHADER_SCREEN_VERTEX, PATH_SHADER_SCREEN_FRAG);
	}
	if(ENABLE_SKYBOX){
		skybox = new Skybox(skyboxFaces, PATH_TEXTURE_SKYBOX);
		skyboxShader = new Shader(PATH_SHADER_SKYBOX_VERTEX, PATH_SHADER_SKYBOX_FRAG);
	}
	if(ENABLE_MSAA){
		glEnable(GL_MULTISAMPLE);
		multiSample = new MultiSample(SCR_WIDTH, SCR_HEIGHT, 4);
	}

	shader.use();
	setLighting(shader);

	// draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window)){
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

		if(ENABLE_MSAA)
			multiSample->offScreen();			// switch to multi-sampling offscreen fbo
		else if (ENABLE_FILTER)
			filter->toScreenTexture();			// switch framebuffer to screen texture... 


		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f));	

		shader.use();
		shader.setVec3("viewPos", camera.Position);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);
        ourModel.Draw(shader);

		if(ENABLE_SKYBOX){
			skyboxShader->use();
			skyboxShader->setMat4("view", glm::mat4(glm::mat3(view)));
			skyboxShader->setMat4("projection", projection);
			skybox->Draw(skyboxShader);
		}

		if(ENABLE_MSAA)		
			multiSample->Draw(ENABLE_FILTER ? filter->getFrameBuffer() : 0);
		if (ENABLE_FILTER)
			filter->Draw(screenShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window){
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    // make sure the viewport matches the new window dimensions
	// the width and height is actuallt 2*(w, h) (as we specified to glVp) on retina displays
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if (firstMouse){
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    camera.ProcessMouseScroll(yoffset);
}

void setLighting(Shader shader){
	vector<PointLight*> lights{
		new PointLight(glm::vec3(0.0, 6.0, 0.0)),
		new PointLight(glm::vec3(5.0, 0.0, 0.0)),
		new PointLight(glm::vec3(0.0, 0.0, 5.0)),
	};

	for (int i = 0; i < lights.size(); i++)
		lights[i]->set(&shader, "pointLights[" + std::to_string(i) + "].");
}