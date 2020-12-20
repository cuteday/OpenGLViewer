#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern unsigned int quadVAO, quadVBO;
void renderQuad();

void generateMSFramebuffer(unsigned int width, unsigned int height, unsigned int *framebuffer, unsigned int *textureColorBuffer, unsigned int *RBO, unsigned int samples = 4, bool HDR = true);

void generateColorTexture(unsigned int *texture, unsigned int width, unsigned int height, bool HDR = true);

void generateFramebuffer(unsigned int width, unsigned int height, unsigned int *framebuffer, unsigned int *textureColorbuffer, unsigned int *RBO, bool HDR = true);