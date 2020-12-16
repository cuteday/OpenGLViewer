#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.hpp"

class Filter{
public: 

	Filter(unsigned int _width, unsigned int _height, bool _gamma):
		width(_width), height(_height){
#ifdef __APPLE__	// retina
		width *= 2, height *= 2;
#endif
		setup();
	}

	void Draw(Shader *screenShader){
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

		screenShader->use();
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void toScreenTexture(){
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	}

	static void generateFramebuffer(
					unsigned int width, 
					unsigned int height, 
					unsigned int *framebuffer, 
					unsigned int *textureColorbuffer, 
					unsigned int *RBO){
		// setup framebuffer and texture attachment
		glGenFramebuffers(1, framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer);
		// color attachment
		glGenTextures(1, textureColorbuffer);
		glBindTexture(GL_TEXTURE_2D, *textureColorbuffer);	// dont need active/ set texunit since only 1 texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// filter kernel border padding 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *textureColorbuffer, 0);
		// stencil and depth attachment
		glGenRenderbuffers(1, RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, *RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *RBO);
	    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        	std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	unsigned int getFrameBuffer() { return framebuffer; }

private:
	unsigned int width, height;
	unsigned int quadVAO, quadVBO;
	unsigned int framebuffer, textureColorbuffer, RBO;

	void setup(){
		float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			1.0f, -1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f,  0.0f, 1.0f,
			1.0f, -1.0f,  1.0f, 0.0f,
			1.0f,  1.0f,  1.0f, 1.0f
		};

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		generateFramebuffer(width, height, &framebuffer, &textureColorbuffer, &RBO);
	}
};