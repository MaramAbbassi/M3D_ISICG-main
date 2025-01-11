#version 450

layout(location = 0) out vec4 fragColor;

in vec3 fragPos;
in vec3 normal;
in vec3 lightPos;
in vec2 textureCoords;
in mat3 TBN;

uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;

uniform vec3 CameraPosition;
uniform vec3 LightColor;
uniform vec3 LightPos;

uniform bool Blinn_Phong;
uniform bool Gauss_light;
uniform bool NormalMaps;

uniform bool uHasDiffuseMap;
layout(binding = 1) uniform sampler2D uDiffuseMap;

uniform bool uHasAmbientMap;
layout(binding = 2) uniform sampler2D uAmbientMap;

uniform bool uHasSpecularMap;
layout(binding = 3) uniform sampler2D uSpecularMap;

uniform bool uHasShininessMap;
layout(binding = 4) uniform sampler2D uShininess;

uniform bool uHasNormalMap;
layout(binding = 5) uniform sampler2D uNormalMap;

void main()
{
    if (uHasDiffuseMap && texture(uDiffuseMap, textureCoords).w < 0.5f)
        discard;

    // Normaliser
    vec3 norm = normalize(normal);

    // Apply normal map 
    if (uHasNormalMap && NormalMaps)
    {
        vec3 mappedNormal = texture(uNormalMap, textureCoords).rgb;
        mappedNormal = normalize(mappedNormal * 2.0 - 1.0);
        norm = normalize(TBN * mappedNormal);
    }

    //  backface rendering
    if (!gl_FrontFacing) norm = -norm;

    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(CameraPosition - fragPos);

    
    // Ambient Component
    vec3 ambient = ambientColor;
    if (uHasAmbientMap)
    {
        ambient = texture(uAmbientMap, textureCoords).rgb;
    }

    // Diffuse Component
    vec3 diff = diffuseColor;
    if (uHasDiffuseMap)
    {
        diff = texture(uDiffuseMap, textureCoords).rgb;
    }
    float diffIntensity = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffIntensity * diff;

    // Shininess 
    float Shininess = shininess;
    if (uHasShininessMap)
    {
        Shininess = texture(uShininess, textureCoords).x;
    }

    // Specular Component
    vec3 spec = specularColor;
    if (uHasSpecularMap)
    {
        spec = texture(uSpecularMap, textureCoords).xxx;
    }

    vec3 specular = vec3(0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    if (Blinn_Phong)
    {
        // Blinn-Phong Specular
        float specIntensity = pow(max(dot(norm, halfwayDir), 0.0), Shininess);
        specular = specIntensity * spec;
    }
    else
    {
        // Phong Specular
        vec3 reflectDir = reflect(-lightDir, norm);
        float specIntensity = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
        specular = specIntensity * spec;
    }

    // Gaussian 
    if (Gauss_light)
    {
        float angleNormalHalf = acos(dot(halfwayDir, norm));
        float exponent = -pow(angleNormalHalf / Shininess, 2.0);
        float gaussianTerm = exp(exponent);
        specular = gaussianTerm * spec;
    }

    vec3 finalColor = (ambient + diffuse + specular) * LightColor;
    fragColor = vec4(finalColor, 1.0);

    // alpha 
   /* float alpha = uHasDiffuseMap ? texture(uDiffuseMap, textureCoords).a : 1.0;
    fragColor = vec4(finalColor, alpha);*/
}
