#version 330 core
// gl_FragCoord
out vec4 FragColor;

#define NR_POINT_LIGHTS 3
#define NR_MAX_TEXTURES 3

in VS_OUT{
	vec2 TexCoords;
	vec3 Normal;
	vec3 Normal0;
	vec3 FragPos;
	vec4 Color;
} fs_in;

struct Material{		
	sampler2D diffuse[NR_MAX_TEXTURES];
	sampler2D specular[NR_MAX_TEXTURES];		// currently we dont have texture with specular albedo!...
	float shininess;
	int nDiffuse;
	int nSpecular;
};

struct DirLight{
	vec3 direction;
	// Ia, Id and Is
	vec3 ambient;	
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};



uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

bool blinn_phong = true;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);

void main()
{   
	vec3 norm = normalize(fs_in.Normal);
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);

	vec3 result = vec3(0.0);
    //vec3 result = CalcDirLight(dirLight, norm, viewDir);

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, viewDir);  

	FragColor = vec4(result, 1.0);
	// FragColor = Color;
    // FragColor = texture(texture_diffuse1, TexCoords);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
    float diff = max(dot(normal, lightDir), 0.0), spec = 0.0;
	if(blinn_phong){
		vec3 halfDir = normalize(viewDir + reflectDir);
		spec = pow(max(dot(halfDir, normal), 0.0), material.shininess);
	}
	else {
		spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	}

    float distance = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

	vec3 texDiffuse = vec3(0.0), texSpecular = vec3(0.0);
	if(material.nDiffuse > 0){
		for(int i = 0; i < material.nDiffuse; i++)
			texDiffuse += vec3(texture(material.diffuse[0], fs_in.TexCoords));
	}
	else texDiffuse = vec3(1.0);
	if(material.nSpecular > 0){
		for(int i = 0; i < material.nSpecular; i++)
			texSpecular += vec3(texture(material.specular[0], fs_in.TexCoords));
	}
	else texSpecular = vec3(0.0);

	vec3 ambient = light.ambient * texDiffuse;
    vec3 diffuse = light.diffuse * diff * texDiffuse;
    vec3 specular = light.specular * spec * texSpecular;
    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
	vec3 reflectDir = reflect(-lightDir, normal);	
    float diff = max(dot(normal, lightDir), 0.0), spec = 0.0;
	if(blinn_phong){
		vec3 halfDir = normalize(viewDir + reflectDir);
		spec = pow(max(dot(halfDir, normal), 0.0), material.shininess);
	}
	else {
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	}

	vec3 texDiffuse = vec3(0.0), texSpecular = vec3(0.0);
	if(material.nDiffuse > 0){
		for(int i = 0; i < 2; i++)
			texDiffuse += vec3(texture(material.diffuse[0], fs_in.TexCoords));
	}
	else texDiffuse = vec3(1.0);
	if(material.nSpecular > 0){
		for(int i = 0; i < 2; i++)
			texSpecular += vec3(texture(material.specular[0], fs_in.TexCoords));
	}
	else texSpecular = vec3(0.0);
	
	vec3 ambient = light.ambient * texDiffuse;
    vec3 diffuse = light.diffuse * diff * texDiffuse;
    vec3 specular = light.specular * spec * texSpecular;
    return (ambient + diffuse + specular);
}
