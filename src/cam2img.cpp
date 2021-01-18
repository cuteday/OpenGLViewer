#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "importer.hpp"
#include "shader.hpp"
#include "function.hpp"
#include "scene.hpp"
#include "filter.hpp"
#include "msaa.hpp"

#include <string>
#include <fstream>
#include <vector>
using namespace std;

#define N_MAX_RENDERS 20
#define ENABLE_FILTER 1
#define ENABLE_MSAA 1
 
string path_model;
string path_camera;
string path_output;

float pos_x[N_MAX_RENDERS], pos_y[N_MAX_RENDERS], pos_z[N_MAX_RENDERS];
float eular_x[N_MAX_RENDERS], eular_y[N_MAX_RENDERS], eular_z[N_MAX_RENDERS];

glm::vec3 pos, euler;
glm::vec3 world_up(0.0, 1.0, 0.0);
float pitch, yaw, roll;

Scene *scene;
Shader *rgbShader, *screenShader;
Filter *postProcess;
MultiSample *msaa;

unsigned int resultFbo, rgbTexture, dsRbo;
unsigned int width = 640, height = 480;
float fov = 0.605;

glm::mat4 model, view, projection;

void setLighting(Shader &shader);

int main(int argc, char *argv[])
{

	if (argc < 4)
	{
		cout << "Not enough args! Quitting..." << endl;
		return 0;
	}

	GLFWwindow *window = initGlfwContext(width, height, true);
	glViewport(0, 0, width, height);

	path_model = argv[1];
	path_camera = argv[2];
	path_output = argv[3];
	// int n_render = atoi(argv[4]);

	cout << "Rendering model path: " << path_model << endl;
	cout << "Output to: " << path_output << endl;
	cout << "Camera intrinsics: " << path_camera << endl;

	scene = Importer::Import3DFront(path_model);
	rgbShader = new Shader(PATH_SHADER_VERTEX, PATH_SHADER_FRAG);
	
	if (ENABLE_FILTER){
		postProcess = new Filter(width / 2, height / 2 , true, true, false);
		screenShader = new Shader(PATH_SHADER_SCREEN_VERTEX, PATH_SHADER_SCREEN_FRAG);
	}
	if (ENABLE_MSAA){
		glEnable(GL_MULTISAMPLE);
		msaa = new MultiSample(width / 2, height / 2, 4, true);
	}

	generateFramebuffer(width, height, &resultFbo, &rgbTexture, &dsRbo);
	glBindFramebuffer(GL_FRAMEBUFFER, resultFbo);

	rgbShader->use();
	setLighting(*rgbShader);

	ifstream cams(path_camera);
	for (int i = 0; i < 20; i++){
		cams >> eular_x[i] >> eular_y[i] >> eular_z[i];
		cams >> pos_x[i] >> pos_y[i] >> pos_z[i];
	}
	cams.close();

	for (int n_render = 0; n_render < 20; n_render++){

		if(ENABLE_MSAA)
			msaa->offScreen();
		else if(ENABLE_FILTER)
		postProcess->toScreenTexture();

		pos = glm::vec3(pos_x[n_render], pos_z[n_render], -pos_y[n_render]);
		euler = glm::vec3(eular_x[n_render] - glm::half_pi<float>(), eular_z[n_render], eular_y[n_render]);

		cout << "Current frame:" 
			 << n_render << "[Pos ("
			 << pos[0] << " "
			 << pos[1] << " "
			 << pos[2] << ") Rotate ("
			 << euler[0] << " "
			 << euler[1] << " "
			 << euler[2] << ")]" << endl;

		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		model = glm::mat4(1.0);
		view = glm::transpose(glm::eulerAngleZYX(euler[2], euler[1], euler[0])) *
			   glm::translate(glm::mat4(1.0), -pos);
		projection = glm::perspective(fov, (float)width / height, 0.1f, 100.0f);

		rgbShader->use();
		rgbShader->setVec3("viewPos", pos);
		rgbShader->setMat4("model", model);
		rgbShader->setMat4("view", view);
		rgbShader->setMat4("projection", projection);

		scene->Draw(rgbShader);

		if(ENABLE_MSAA)
			msaa->Draw(ENABLE_FILTER ? postProcess->getFramebuffer() : resultFbo);
		if (ENABLE_FILTER)
			postProcess->Draw(screenShader, resultFbo);

		char output_name[50];
		sprintf(output_name, "%s/rgb_%04d_gl.png", path_output.c_str(), n_render);
		screenshot(output_name);
		cout << "ending loop" << n_render << endl;
	}
    glfwTerminate();
	return 0;
}

void setLighting(Shader &shader)
{
	vector<PointLight *> lights{
		new PointLight(glm::vec3(10.0, 30.0, 10.0)),
	};
	DirLight dirLight(glm::vec3(0.5, -1.0, 0.5));

	shader.use();
	for (int i = 0; i < lights.size(); i++)
		lights[i]->set(&shader, "pointLights[" + std::to_string(i) + "].");
	dirLight.set(&shader, "dirLight.");
}
