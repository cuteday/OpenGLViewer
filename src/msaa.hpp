#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.hpp"

class MultiSample{
public:
	MultiSample(unsigned int _width, unsigned int _height, unsigned int _samples = 4, bool HDR = false):
			width(_width), height(_height), samples(_samples){
#ifdef __APPLE__
		width *= 2, height *= 2;
#endif
		generateMSFramebuffer(width, height, &framebuffer, &msTextureColor, &msRBO, samples, HDR);
	}

	static void generateMSFramebuffer(
		unsigned int width,
		unsigned int height,
		unsigned int *framebuffer,
		unsigned int *textureColorBuffer,
		unsigned int *RBO,
		unsigned int samples = 4,
		bool HDR = false
	){
		GLenum textureColorFormat = HDR ? GL_RGB16F : GL_RGB;

		glGenFramebuffers(1, framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer);

		glGenTextures(1, textureColorBuffer);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *textureColorBuffer);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, textureColorFormat, width, height, GL_TRUE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, *textureColorBuffer, 0);

		glGenRenderbuffers(1, RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, *RBO);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *RBO);
	    
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        	std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void offScreen(){
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	}

	void Draw(unsigned int targetFramebuffer = 0){
		/*
		 * set targetFramebuffer to a value other than 0 to draw anti-aliased image to an intermediate buffer.
		 * e.g to draw MSAAed image to screen texture, then do filter.
		 */
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFramebuffer);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

private:
	unsigned int width, height, samples;
	unsigned int framebuffer, msTextureColor, msRBO;
};