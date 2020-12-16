#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.hpp"
// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"
  
#include <vector>
#include <string>
using namespace std;

vector<std::string> skyboxFaces{
    "right.jpg",
    "left.jpg",
    "top.jpg",
    "bottom.jpg",
    "front.jpg",
    "back.jpg"
};

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

class Skybox{
public:
	Skybox(vector<string> faces, string path){
		setup();
		texture = loadCubeMap(faces, path);
	}

	static unsigned int loadCubeMap(vector<string> faces, string path){
		int width, height, nrChannels;
		unsigned int _texture;
		//stbi_set_flip_vertically_on_load(false); // the cubemap texture need top-down image, different from other textures!
		glGenTextures(1, &_texture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);
		for (int i = 0; i < faces.size();i++){
			// std::cout << "Loading skybox from: " + path + faces[i] << std::endl;
			unsigned char *data = stbi_load((path + faces[i]).c_str(), &width, &height, &nrChannels, 0);
			if (data){
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
							0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        	}
			else{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			}
			stbi_image_free(data);
		}
		//stbi_set_flip_vertically_on_load(true);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return _texture;
	}

	void Draw(Shader *skyboxShader){
		glDepthFunc(GL_LEQUAL);		// depth buffer intialized as 1.0f!
		skyboxShader->use();
		glBindVertexArray(VAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthFunc(GL_LEQUAL);
	}

private:
	unsigned int texture;
	unsigned VAO, VBO;

	void setup(){
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float) , (void *)0);
		glEnableVertexAttribArray(0);
	}
};