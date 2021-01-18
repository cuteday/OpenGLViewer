#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "function.hpp"

class Gaussian{
public:
	Gaussian(unsigned int _width, unsigned int _height):
		width(_width), height(_height){
		setup();
		shader = new Shader(PATH_SHADER_SCREEN_VERTEX, PATH_SHADER_GAUSSIAN);
	}

	void Draw(unsigned int initialTexture){
		const int nPass = 10;
		shader->use();
		for (int i = 0; i < nPass; i++){
			bool horizontal = i % 2;
			glBindFramebuffer(GL_FRAMEBUFFER, FBO[horizontal]);
			shader->setBool("horizontal", horizontal);
			glBindTexture(GL_TEXTURE_2D, i > 0 ? colorbuffer[!horizontal] : initialTexture);
			renderQuad();
		}
	}

	unsigned int getColorbuffer() { return colorbuffer[1]; }

private:
	unsigned int FBO[2], colorbuffer[2];
	unsigned int width, height;
	Shader *shader;
	unsigned int RBO[2];

	void setup(){
		glGenFramebuffers(2, FBO);
		glGenTextures(2, colorbuffer);
		for (int i = 0; i < 2; i++){
			glBindFramebuffer(GL_FRAMEBUFFER, FBO[i]);
			glBindTexture(GL_TEXTURE_2D, colorbuffer[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// filter kernel border padding 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorbuffer[i], 0);
		    
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            	std::cout << "ERROR::GAUSSIAN::Framebuffer not complete" << std::endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};

class Filter{
public: 

	Filter(unsigned int _width, 
			unsigned int _height, 
			bool _gamma = true, 
			bool _HDR = false,
			bool _bloom = false):
		width(_width), height(_height), gamma(_gamma), HDR(_HDR), bloom(_bloom){
#ifdef __APPLE__	// retina
		width *= 2, height *= 2;
#endif
		setup();
	}

	void Draw(Shader *screenShader, unsigned int tgtbuffer = 0){
		if(bloom){
			gaussian->Draw(bloomColorbuffer);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, tgtbuffer);
		glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);

		screenShader->use();
		screenShader->setBool("enableGamma", gamma);
		screenShader->setBool("enableHDR", HDR);
		screenShader->setBool("enableBloom", bloom);
		screenShader->setInt("screenTexture", 0);
		screenShader->setInt("bloomColor", 1);

		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, gaussian->getColorbuffer());
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer );
		if(bloom){
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gaussian->getColorbuffer());
		}
		renderQuad();
		glActiveTexture(GL_TEXTURE0);
	}

	void toScreenTexture(){
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	}

	unsigned int getFramebuffer() { return framebuffer; }

private:
	bool gamma, HDR, bloom;
	unsigned int width, height;
	unsigned int quadVAO, quadVBO;
	unsigned int framebuffer, textureColorbuffer, RBO;
	unsigned int bloomColorbuffer;
	Gaussian *gaussian;

	void setup(){
		unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		generateFramebuffer(width, height, &framebuffer, &textureColorbuffer, &RBO, true);
		if(bloom){	// we bind bloom color texture as 2nd color texture attachment
			gaussian = new Gaussian(width, height);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			generateColorTexture(&bloomColorbuffer, width, height, true);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bloomColorbuffer, 0);
			glDrawBuffers(2, attachments);
		}
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};

