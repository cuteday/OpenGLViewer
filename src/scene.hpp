#pragma once

#include "model.hpp"
#include "mesh.hpp"
#include "light.hpp"

class Scene{
public:
	
	void Draw(Shader* shader){
		for (int i = 0;i < models.size(); i++){
			models[i].Draw(*shader);
		}
	}

	vector<Model> models;
};