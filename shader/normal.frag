#version 330 core

in vec3 Tangent;
out vec4 FragColor;

void main(){
	FragColor = vec4(Tangent, 1.0);
}