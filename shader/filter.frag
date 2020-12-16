#version 330 core

in vec2 TexCoords;
uniform bool enableHDR;
uniform bool enableGamma;
uniform sampler2D screenTexture;
const float gamma = 2.2;
const float exposure = 1.5;
out vec4 FragColor;

void main(){
	FragColor = texture(screenTexture, TexCoords);
	if(enableHDR){
		// tone mapping
		//FragColor.rgb = FragColor.rgb / (FragColor.rgb + vec3(1.0));	// Reinhard
		FragColor.rgb = vec3(1.0) - exp(-FragColor.rgb * exposure);
	}
	if(enableGamma){
		// gamma correction
		FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
	}

}