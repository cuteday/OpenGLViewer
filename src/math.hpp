#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
using namespace std;

namespace Math{

glm::mat4 to_glm_mat4(const aiMatrix4x4& assimp_matrix){
	glm::mat4 mat(0.0f);
	mat[0][0] = assimp_matrix.a1; mat[0][1] = assimp_matrix.a2; mat[0][2] = assimp_matrix.a3; mat[0][3] = assimp_matrix.a4;
	mat[1][0] = assimp_matrix.b1; mat[1][1] = assimp_matrix.b2; mat[1][2] = assimp_matrix.b3; mat[1][3] = assimp_matrix.b4;
	mat[2][0] = assimp_matrix.c1; mat[2][1] = assimp_matrix.c2; mat[2][2] = assimp_matrix.c3; mat[2][3] = assimp_matrix.c4;
	mat[3][0] = assimp_matrix.d1; mat[3][1] = assimp_matrix.d2; mat[3][2] = assimp_matrix.d3; mat[3][3] = assimp_matrix.d4;
	return mat;
}

glm::vec3 transform_coordframe(glm::vec3 point, vector<string> axes = {"X", "Y", "Z"}){
	glm::vec3 vec(0.0);
	if(axes.size() != 3){
		cout << "ERROR::transform_coords::specified axis number not 3" << endl;
		assert(false);
	}
	for (int i = 0; i < 3;i++){
		char axis = *axes[i].end();
		if (axis == 'X')
			vec[i] = point[0];
		else if(axis == 'Y')
			vec[i] = point[1];
		else if(axis == 'Z')
			vec[i] = point[2];
		else {
			cout << "ERROR::transform_coords::invalid axis name" << endl;
			assert(false);
		}
		if(axes[i][0] == '-')
			vec[i] *= -1;
	}
	return vec;
}
}