#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec4 Color;

uniform sampler2D texture_diffuse1;

void main()
{    
	FragColor = Color;
    // FragColor = texture(texture_diffuse1, TexCoords);
}