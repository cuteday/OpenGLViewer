#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>

extern unsigned int quadVAO, quadVBO;
void renderQuad();

void generateMSFramebuffer(unsigned int width, unsigned int height, unsigned int *framebuffer, unsigned int *textureColorBuffer, unsigned int *RBO, unsigned int samples = 4, bool HDR = true);

void generateColorTexture(unsigned int *texture, unsigned int width, unsigned int height, bool HDR = true);

void generateFramebuffer(unsigned int width, unsigned int height, unsigned int *framebuffer, unsigned int *textureColorbuffer, unsigned int *RBO, bool HDR = true);

GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void screenshot(const char *output_path);

void saveImage(unsigned char *data, const char *output_path, unsigned int width, unsigned int height, bool flipUV = true);

unsigned char *loadImage(std::string filename, int *width, int *height, int *channels, bool flipUV = false);
#define freeImage(data) 	delete[] data