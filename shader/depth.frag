#version 410 core
out vec4 FragColor;

float near = 0.1;
float far = 100.0;

void main(){
	float z = gl_FragCoord.z;								// [0, 1] non-linear
	float linear = (near * far) / (far - (far - near) * z); 	// [n, f] linear
	// this linear is (V * M * vertex).z (no perspective)
	FragColor = vec4(vec3(linear / (far - near)), 1.0); 	// [0, 1] linear
}