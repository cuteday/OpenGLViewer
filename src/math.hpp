#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Math{

glm::mat4 to_glm_mat4(const aiMatrix4x4& assimp_matrix){
	glm::mat4 mat(0.0f);
	mat[0][0] = assimp_matrix.a1; mat[0][1] = assimp_matrix.a2; mat[0][2] = assimp_matrix.a3; mat[0][3] = assimp_matrix.a4;
	mat[1][0] = assimp_matrix.b1; mat[1][1] = assimp_matrix.b2; mat[1][2] = assimp_matrix.b3; mat[1][3] = assimp_matrix.b4;
	mat[2][0] = assimp_matrix.c1; mat[2][1] = assimp_matrix.c2; mat[2][2] = assimp_matrix.c3; mat[2][3] = assimp_matrix.c4;
	mat[3][0] = assimp_matrix.d1; mat[3][1] = assimp_matrix.d2; mat[3][2] = assimp_matrix.d3; mat[3][3] = assimp_matrix.d4;
	return mat;
}

}