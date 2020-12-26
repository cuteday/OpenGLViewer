#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "function.hpp"

class MultiSample{
public:
	MultiSample(unsigned int _width, unsigned int _height, unsigned int _samples = 4, bool HDR = true):
			width(_width), height(_height), samples(_samples){
#ifdef __APPLE__
		width *= 2, height *= 2;
#endif
		generateMSFramebuffer(width, height, &framebuffer, &msTextureColor, &msRBO, samples, HDR);
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
		// this copies specified attachments of READ_FRAMEBUFFER to WRITE_FRAMEBUFFER
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

private:
	unsigned int width, height, samples;
	unsigned int framebuffer, msTextureColor, msRBO;
};