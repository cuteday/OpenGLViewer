#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aColor;
// layout (location = 4) in vec3 aTangent;
// layout (location = 5) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec4 Color;
// out vec3 Tangent;
// out vec3 Bitangent;

// layout (std140) uniform Matrices{
// 	mat4 projection;
// 	mat4 view;
// };

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;   
	Normal = mat3(transpose(inverse(model))) * aNormal;
	Color = aColor;
	FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}