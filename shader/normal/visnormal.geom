#version 410 core
/*
 *	generate vertex normal, for 3 vertices in a triangle primitive
 *	can also draw face normal in the center of this triangle mesh
 */

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
	vec3 normal;		// this is vectex normal
}vs_in[];

const float len = 0.5;

void DrawLine(int index){
	gl_Position = gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = gl_in[index].gl_Position + vec4(vs_in[index].normal, 0.0) * len;
	EmitVertex();
	EndPrimitive();
}

void main(){
	for(int i=0; i<3; i++)
		DrawLine(i);
}