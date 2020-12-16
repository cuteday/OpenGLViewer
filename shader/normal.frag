#version 330 core

in vec3 Normal;
in vec3 Normal0;
out vec4 FragColor;

void main(){
	FragColor = vec4(normalize(Normal) * 0.5 + 0.5, 1.0);
}