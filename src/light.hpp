#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"

class Light{
public:
	glm::vec3 ambient, diffuse, specular;
	float constant, linear, quadratic;

	void set(Shader *shader, std::string name){
		shader->setVec3(name + "ambient", ambient);
		shader->setVec3(name + "diffuse", diffuse);
		shader->setVec3(name + "specular", specular);
		shader->setFloat(name + "constant", constant);
		shader->setFloat(name + "linear", linear);
		shader->setFloat(name + "quadratic", quadratic);
	}
};

class PointLight : public Light{
public:
	PointLight(
		glm::vec3 position = glm::vec3(0.0f),
		glm::vec3 ambient = glm::vec3(600),
		glm::vec3 diffuse = glm::vec3(600),
		glm::vec3 specular = glm::vec3(500),
		float constant = 1.0f, 
		float linear = 0.09f,
		float quadratic = 0.032f
	):position(position){
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;
	}

	glm::vec3 position;

	void set(Shader *shader, std::string name){
		shader->setVec3(name + "position", position);
		((Light*)this)->set(shader, name);
	}
};

class DirLight: public Light{
public:
	DirLight(
		glm::vec3 direction = glm::vec3(0.0f),
		glm::vec3 ambient = glm::vec3(0.15f),
		glm::vec3 diffuse = glm::vec3(0.8f),
		glm::vec3 specular = glm::vec3(1.0f)
	):direction(direction){
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->constant = 0;
		this->linear = 0;
		this->quadratic = 0;
	}

	glm::vec3 direction;
	void set(Shader *shader, std::string name){
		shader->setVec3(name + "direction", direction);
		((Light*)this)->set(shader, name);
	}
};
