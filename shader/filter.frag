#version 330 core

in vec2 TexCoords;
uniform sampler2D screenTexture;
const float gamma = 2.2;
out vec4 FragColor;

void main(){
	FragColor = texture(screenTexture, TexCoords);
	FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}