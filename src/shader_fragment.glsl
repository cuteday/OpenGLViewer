#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in vec4 Color;

struct Material{		
	vec3 diffuse;
	vec3 specular;		// currently we dont have texture with specular albedo!...
	float shininess;
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

#define NR_POINT_LIGHTS 1

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{   
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = vec3(0.0);
    //vec3 result = CalcDirLight(dirLight, norm, viewDir);

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);  

	FragColor = vec4(result, 1.0);
	// FragColor = Color;
    // FragColor = texture(texture_diffuse1, TexCoords);
}

// vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
// {
//     vec3 lightDir = normalize(-light.direction);
//     float diff = max(dot(normal, lightDir), 0.0);
//     vec3 reflectDir = reflect(-lightDir, normal);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//     vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
//     vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
//     vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
//     return (ambient + diffuse + specular);
// }

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
	vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;

	// vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    // vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    // vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular) * attenuation;
}
