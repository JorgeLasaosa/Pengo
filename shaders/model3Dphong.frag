#version 330 core
out vec4 color;
  
in vec3 ourColor;
in vec3 FragPos;
in vec3 Normal;  
  
uniform vec3 lightPos; 
uniform vec3 lightColor;

void main()
{
    // Ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * ourColor;
    color = vec4(result, 1.0f);
} 