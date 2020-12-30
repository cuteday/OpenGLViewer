#include "function.hpp"
#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

/**	OpenGL context is combined with window in GLFW. 
 * 	This function creates a GLFWwindow and make it as the current context.
 *  @param windowless set to true if you want to do offscreen rendering on a framebuffer
 * 	@return GLFWwindow
 */
GLFWwindow* initGlfwContext(int width, int height, bool windowless){
	if(windowless){
#ifdef __APPLE__
		glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_FALSE);
#endif
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	}
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGLViewer", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
		assert(false);
	}
	glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
		assert(false);
    }
	return window;
}

void saveImage(uchar* data, const char* output_path, unsigned int width, unsigned int height, bool flipUV){
	
	cv::Mat img(height, width, CV_8UC(3));
	for (int i = 0; i < height; i++){
		uchar *p = img.ptr<uchar>(flipUV ? height - 1 - i : i);	// can also use cv::flip 
		for (int j = 0; j < width; j++){
			int k1 = (i * width + j) * 3, k2 = j * 3;
			// RGB in OpenGL color buffer but BGR in OpenCV Mat
			p[k2] = data[k1 + 2];
			p[k2 + 1] = data[k1 + 1];
			p[k2 + 2] = data[k1];
		}
	}
	std::cout << "saving image to" << output_path <<"...\n";
	cv::imwrite(output_path, img);
}

//	Save current framebuffer's RGB color attachment to image.
void screenshot(const char* output_path){
	GLint viewPort[4] = {0};
	glGetIntegerv(GL_VIEWPORT, viewPort);
	unsigned char *pixels = new unsigned char[viewPort[3] * viewPort[2] * 3];
	glReadPixels(viewPort[0], viewPort[1], viewPort[2], viewPort[3], GL_RGB, GL_UNSIGNED_BYTE, pixels);
	saveImage(pixels, output_path, viewPort[2], viewPort[3]);
	delete[] pixels;
}

/** @brief This is a wrapper of OpenCV::imread()
 *  @param FlipUV Set this to true when load some kind of textures.
 * 	@return A consecutive memory with row major RGB image data, returns NULL if fails to read.
 * 		The memory needs to be free manually, using freeImage() or delete[]. 
 */
unsigned char* loadImage(std::string filename, int* width, int* height, int* channels, bool flipUV){
	cv::Mat img = cv::imread(filename);
	assert(img.isContinuous());
	cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
	if(flipUV) cv::flip(img, img, 1);
	if(img.data == NULL) return NULL;

	*width = img.cols;
	*height = img.rows;
	*channels = img.channels();

	size_t size = img.elemSize() * img.total();
	uchar *data = new uchar[size];
	memcpy(data, img.data, size);
	return data;
}


GLenum glCheckError_(const char *file, int line){

    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR){
        std::string error;
        switch (errorCode){
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}

void generateFramebuffer(
					unsigned int width, 
					unsigned int height, 
					unsigned int *framebuffer, 
					unsigned int *textureColorbuffer, 
					unsigned int *RBO,
					bool HDR){
	GLenum colorInternalFormat = HDR ? GL_RGBA16F : GL_RGBA;
	GLenum colorType = HDR ? GL_FLOAT : GL_UNSIGNED_BYTE;

	// setup framebuffer and texture attachment
	glGenFramebuffers(1, framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer);
	// color attachment
	glGenTextures(1, textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, *textureColorbuffer);	// dont need active/ set texunit since only 1 texture
	glTexImage2D(GL_TEXTURE_2D, 0, colorInternalFormat, width, height, 0, GL_RGBA, colorType, NULL);
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

void generateColorTexture(unsigned int *texture, unsigned int width, unsigned int height, bool HDR){
	GLenum colorInternelFormat = HDR ? GL_RGBA16F : GL_RGBA;
	GLenum colorType = HDR ? GL_FLOAT : GL_UNSIGNED_BYTE;

	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);	// dont need active/ set texunit since only 1 texture
	glTexImage2D(GL_TEXTURE_2D, 0, colorInternelFormat, width, height, 0, GL_RGBA, colorType, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// filter kernel border padding 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void generateMSFramebuffer(
	unsigned int width,
	unsigned int height,
	unsigned int *framebuffer,
	unsigned int *textureColorBuffer,
	unsigned int *RBO,
	unsigned int samples,
	bool HDR
){
	GLenum textureColorFormat = HDR ? GL_RGBA16F : GL_RGB;

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
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int quadVAO, quadVBO;

void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
