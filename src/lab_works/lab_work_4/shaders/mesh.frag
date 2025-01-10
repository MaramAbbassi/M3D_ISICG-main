#version 450

layout(location = 0) out vec4 fragColor;

in vec3 fragPos;
in vec3 normal;
in vec3 lightPos;

uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;

uniform vec3 CameraPosition;
uniform vec3 LightColor;
uniform bool Blinn_Phong;
uniform bool Gauss_light;

void main()
{
    //normaliser
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(CameraPosition - fragPos);

    // backface rendering
    if (!gl_FrontFacing) norm = -norm;

    // Diffuse 
    float diffIntensity = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffIntensity * diffuseColor;

    // Specular 
    vec3 specular = vec3(0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    if (Blinn_Phong)
    {
        // Blinn-Phong 
        float specIntensity = pow(max(dot(norm, halfwayDir), 0.0), shininess);
        specular = specIntensity * specularColor;
    }
    else
    {
        // Phong specular
        vec3 reflectDir = reflect(-lightDir, norm);
        float specIntensity = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        specular = specIntensity * specularColor;
    }

    if (Gauss_light)
    {
        // Gaussien 
        float angleMoit = acos(dot(halfwayDir, norm));
        float exponent = -pow(angleMoit / shininess, 2.0);
        float gaussianTerm = exp(exponent);
        specular = gaussianTerm * specularColor;
    }

    // Combine results
    vec3 finalColor = (ambientColor + diffuse + specular) * LightColor;

    // Output the fragment color
    fragColor = vec4(finalColor, 1.0);
}
