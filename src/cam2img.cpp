#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "importer.hpp"
#include "shader.hpp"
#include "function.hpp"
#include "scene.hpp"

char *path_model;
char *path_camera;

glm::vec3 cam_pos, front, up;
glm::vec3 world_up(0.0, 1.0, 0.0);
float pitch, yaw, roll;

Scene *scene;
Shader *rgbShader;

unsigned int resultFbo, rgbTexture, dsRbo;
unsigned int width = 640, height = 480;

glm::mat4 model, view, projection;

void setLighting(Shader &shader);

int main(int argc, char* argv[]){
	GLFWwindow* window = initGlfwContext(width, height, true);
	
	glViewport(0, 0, width, height);

	path_model = argv[1];
	path_camera = argv[1];

	scene = Importer::Import3DFront(path_model);
	rgbShader = new Shader(PATH_SHADER_VERTEX, PATH_SHADER_FRAG);
	generateFramebuffer(width, height, &resultFbo, &rgbTexture, &dsRbo);
	glBindFramebuffer(GL_FRAMEBUFFER, resultFbo);

	glClearColor(1.0, 0.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	cam_pos = glm::vec3(1.0);
	front = glm::vec3(0.0, 1.0, 1.0);
	up = world_up;

	model = glm::mat4(1.0);
	view = glm::lookAt(front, cam_pos, up);
	projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.1f, 100.0f);

	rgbShader->use();
	rgbShader->setVec3("viewPos", cam_pos);
	rgbShader->setMat4("model", model);
	rgbShader->setMat4("view", view);
	rgbShader->setMat4("projection", projection);
	
	setLighting(*rgbShader);
	scene->Draw(rgbShader);
	screenshot("./scrshot.png");

	return 0;
}

void setLighting(Shader& shader){
	vector<PointLight*> lights{
		new PointLight(glm::vec3(10.0, 30.0, 10.0)),
	};
	DirLight dirLight(glm::vec3(0.5, -1.0, 0.5));

	shader.use();
	for (int i = 0; i < lights.size(); i++)
		lights[i]->set(&shader, "pointLights[" + std::to_string(i) + "].");
	dirLight.set(&shader, "dirLight.");
}
