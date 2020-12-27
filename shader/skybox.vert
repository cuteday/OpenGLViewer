#version 410 core

layout (location = 0) in vec3 aPos;
uniform mat4 projection;
uniform mat4 view;

// layout (std140) uniform Matrices{
// 	mat4 projection;
// 	mat4 view;
// } mat;

out vec3 TexCoords;

void main(){
	gl_Position = (projection * view * vec4(aPos, 1.0)).xyww;
	TexCoords = aPos;
}