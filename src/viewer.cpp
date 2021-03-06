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
#include "scene.hpp"
#include "importer.hpp"

#include <iostream>
#include <string>

bool ENABLE_FILTER = 1;
bool ENABLE_SKYBOX = 1;
bool ENABLE_MSAA = 1;
bool ENABLE_GAMMA = 1;
bool ENABLE_HDR = 1;
bool ENABLE_BLOOM = 0;
bool ENABLE_NORMALVIS = 0;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);
void setLighting(Shader shader);

// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

const char *PATH_TEXTURE_SKYBOX = "/Users/cappu/Public/Projects/OpenGLViewer/texture/skybox/";

const char *PATH_MODEL_NOEL = "/Users/cappu/Public/Projects/OpenGLViewer/model/3D_fanart_Noel_From_Sora_no_Method.blend";
const char *PATH_MODEL_LITTLEWITCH = "/Users/cappu/Public/Projects/OpenGLViewer/model/halloween-little-witch/source/03/03.obj";
const char *PATH_MODEL_SCENENET = "/Users/cappu/Public/Projects/OpenGLViewer/model/SceneNetData/1Office/66office_scene.obj";
const char *PATH_MODEL_3DFRONT = "/Users/cappu/Public/Projects/OpenGLViewer/model/3dfront/00004f89-9aa5-43c2-ae3c-129586be8aaa";

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
Shader *skyboxShader, *screenShader, *gaussianShader;
MultiSample *multiSample;

int main(){

	GLFWwindow *window = initGlfwContext(SCR_WIDTH, SCR_HEIGHT, false);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
	// hide mouse and restrict cursor to this window...
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Scene *front3d = Importer::Import3DFront(PATH_MODEL_3DFRONT);

    glEnable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE);	// face culling
	// glCullFace(GL_BACK);		// this is the default

	//Model ourModel(PATH_MODEL_LITTLEWITCH);
	Shader shader(PATH_SHADER_VERTEX, PATH_SHADER_FRAG);
	Shader normalShader(PATH_SHADER_VISNORMAL_VERT, PATH_SHADER_VISNORMAL_FRAG, PATH_SHADER_VISNORMAL_GEOM);
	if (ENABLE_GAMMA || ENABLE_HDR || ENABLE_BLOOM){
		// we need to do gamma correction and tone mapping in the last screen frame buffer.
		// also, if HDR enabled, the HDR render buffer, along with all consequent frame buffers,
		// must equip with floating color buffer GL_RGBA_16/32F.
		ENABLE_FILTER = 1;
	}
	if(ENABLE_FILTER){
		filter = new Filter(SCR_WIDTH, SCR_HEIGHT, ENABLE_GAMMA, ENABLE_HDR, ENABLE_BLOOM);
		screenShader = new Shader(PATH_SHADER_SCREEN_VERTEX, PATH_SHADER_SCREEN_FRAG);
	}
	if(ENABLE_SKYBOX){
		skybox = new Skybox(skyboxFaces, PATH_TEXTURE_SKYBOX);
		skyboxShader = new Shader(PATH_SHADER_SKYBOX_VERTEX, PATH_SHADER_SKYBOX_FRAG);
	}
	if(ENABLE_MSAA){
		glEnable(GL_MULTISAMPLE);
		multiSample = new MultiSample(SCR_WIDTH, SCR_HEIGHT, 4, ENABLE_HDR);
	}

	shader.use();
	setLighting(shader);

    while (!glfwWindowShouldClose(window)){
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

		if(ENABLE_MSAA)
			multiSample->offScreen();			// switch to multi-sampling offscreen fbo
		else if (ENABLE_FILTER)
			filter->toScreenTexture();			// switch framebuffer to screen texture... 

		// glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 view = camera.GetCameraMatrix();
		glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));	

		// display pos and rotate on window title (for now, will translate to UI or text later)
		std::stringstream ss;
		ss << "GLViewer [Pos (" 
			<< camera.Position[0] << " "
			<< camera.Position[1] << " "
			<< camera.Position[2] << ") "
			<< "Rotate ("
			<< glm::radians(camera.Roll) << " "
			<< glm::radians(camera.Yaw) << " "
			<< glm::radians(camera.Pitch) << " "
		   	<< ")]";
		glfwSetWindowTitle(window, ss.str().c_str());

		shader.use();
		shader.setVec3("viewPos", camera.Position);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);
        //ourModel.Draw(shader);
		front3d->Draw(&shader);

		if(ENABLE_NORMALVIS){
			normalShader.use();
			normalShader.setMat4("projection", projection);
			normalShader.setMat4("view", view);
        	normalShader.setMat4("model", model);
			front3d->Draw(&normalShader);
		}
		if(ENABLE_SKYBOX){
			skyboxShader->use();
			skyboxShader->setMat4("view", glm::mat4(glm::mat3(view)));
			skyboxShader->setMat4("projection", projection);
			skybox->Draw(skyboxShader);
		}
		if(ENABLE_MSAA)		
			multiSample->Draw(ENABLE_FILTER ? filter->getFramebuffer() : 0);
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
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(ROLLLT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(ROLLRT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		screenshot("../texture/scrshot.png");
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    // make sure the viewport matches the new window dimensions
	// the width and height is actuallt 2*(w, h) (as we specified to glViewport) on retina displays
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
		new PointLight(glm::vec3(10.0, 30.0, 10.0)),
	};
	DirLight dirLight(glm::vec3(0.5, -1.0, 0.5));

	for (int i = 0; i < lights.size(); i++)
		lights[i]->set(&shader, "pointLights[" + std::to_string(i) + "].");
	dirLight.set(&shader, "dirLight.");
}

