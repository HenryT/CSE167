
#version 330 core
struct Material {
	vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	bool on;
};

struct PointLight {
    vec3 position;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	bool on;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
	int exponent;
  
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	bool on;
};

#define NR_POINT_LIGHTS 1

in vec3 FragPos;
in vec3 Normal;

out vec4 color;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    // Properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result = vec3(0.0f, 0.0f, 0.0f);
    
    // Phase 1: Directional lighting
	if (dirLight.on)
		result += CalcDirLight(dirLight, norm, viewDir);

    // Phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
		if (pointLights[i].on)
			result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    

    // Phase 3: Spot light
	if (spotLight.on)
    	result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
    color = vec4(result, 1.0);
}

// Calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

	//Ambient
	vec3 amb = material.ambient * light.ambient;

    // Diffuse shading
    vec3 diff = (material.diffuse * light.diffuse) * max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 spec = (material.specular * light.specular) * dot(material.specular, light.specular) * pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    return (amb + diff + spec);
}


// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
	
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.quadratic * (distance * distance));    

    // Combine results
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = (light.diffuse * material.diffuse) * diff;
    vec3 specular = (light.specular * material.specular) * spec;

    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

// Calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = (pow(dot(normalize(light.direction), lightDir), light.exponent)) / (light.quadratic * (distance * distance));    

    // Combine results
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = (light.diffuse * material.diffuse) * diff;
    vec3 specular = (light.specular * material.specular) * spec;

    diffuse *= attenuation;
    specular *= attenuation;

	if (acos(dot(light.direction, lightDir)) > light.cutOff)
		return ambient;
	
    return (ambient + diffuse + specular);
}

/*
#version 330 core

// This is a sample fragment shader.

out vec4 color;

void main()
{
    color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
*/