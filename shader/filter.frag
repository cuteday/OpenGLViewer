#version 410 core
const float gamma = 2.2;
const float exposure = 0.50;

in vec2 TexCoords;

uniform bool enableHDR;
uniform bool enableGamma;
uniform bool enableBloom;

uniform sampler2D screenTexture;
uniform sampler2D bloomColor;

out vec4 FragColor;

void main(){
	FragColor = texture(screenTexture, TexCoords);
	if(enableBloom){
		// simply add -> blend
		FragColor.rgb += texture(bloomColor, TexCoords).rgb;	
	}
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