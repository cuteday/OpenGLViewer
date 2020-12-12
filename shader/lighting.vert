#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aColor;

out VS_OUT{
	vec2 TexCoords;
	vec3 Normal;
	vec3 Normal0;
	vec3 FragPos;
	vec4 Color;
} vs_out;

// layout (std140) uniform Matrices{
// 	mat4 projection;
// 	mat4 view;
// } mat;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vs_out.TexCoords = aTexCoords;   
	vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
	vs_out.Normal0 = vec3(view * vec4(vs_out.Normal, 1.0));
	vs_out.Color = aColor;
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}