#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aColor;
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBitangent;

out VS_OUT{
	vec2 TexCoords;
	vec3 Normal;
	vec3 FragPos;
	vec4 Color;
	mat3 TBN;
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
	vs_out.Color = aColor;
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0);

	vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
	vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
	vec3 N = normalize(cross(T, B));
	B = cross(N, T); 	// length of b = |T|*|N|*sin0 = 1
	vs_out.TBN = mat3(T, B, N);
}

