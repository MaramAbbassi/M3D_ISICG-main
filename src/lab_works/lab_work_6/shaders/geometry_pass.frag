#version 450

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragAmbient;
layout(location = 3) out vec3 fragDiffuse;
layout(location = 4) out vec3 fragSpecular;

in vec3 fragPos;
in vec3 normal;
in vec3 lightPos;
in vec2 texCoords;
in mat3 TBN; 

uniform vec3 CameraPosition;
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform vec3 LightPos;
uniform float shininess;

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
    fragPosition = fragPos;

    if (uHasDiffuseMap && texture(uDiffuseMap, texCoords).a < 0.5f)
        discard;

    vec3 norm = normalize(normal);

    if (uHasNormalMap)
    {
        norm = texture(uNormalMap, texCoords).rgb;
        norm = normalize(norm * 2.0 - 1.0);
        norm = normalize(TBN * norm); // Apply TBN matrix
    }

    if (dot(lightPos - fragPos, norm) < 0)
        norm = -norm;

    fragNormal = norm;

    vec3 lightDir = normalize(LightPos - fragPos);
    vec3 viewDir = normalize(CameraPosition - fragPos);

    vec3 Var_Diffuse = diffuseColor;
    if (uHasDiffuseMap)
    {
        Var_Diffuse = texture(uDiffuseMap, texCoords).rgb * max(dot(norm, lightDir), 0.0);
    }
    else
    {
        Var_Diffuse = diffuseColor * max(dot(norm, lightDir), 0.0);
    }
    fragDiffuse = Var_Diffuse;

    vec3 Var_Ambient = ambientColor;
    if (uHasAmbientMap)
    {
        Var_Ambient = texture(uAmbientMap, texCoords).rgb;
    }
    fragAmbient = Var_Ambient;

    float Shininess = shininess;
    if (uHasShininessMap)
    {
        Shininess = texture(uShininess, texCoords).x;
    }

    vec3 Var_Specular = specularColor;
    if (uHasSpecularMap)
    {
        Var_Specular = texture(uSpecularMap, texCoords).rgb;
    }

    vec3 Var_Spec = Var_Specular * pow(max(dot(norm, normalize(lightDir + viewDir)), 0.0), Shininess);
    fragSpecular = Var_Spec;
}
