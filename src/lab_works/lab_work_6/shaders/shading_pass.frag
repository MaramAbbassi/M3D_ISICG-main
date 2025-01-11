#version 450

layout( location = 0 ) out vec4 fragColor;
layout( binding = 1 ) uniform sampler2D uPosition;
layout( binding = 2 ) uniform sampler2D uNormalMap;
layout( binding = 3 ) uniform sampler2D uAmbientMap;
layout( binding = 4 ) uniform sampler2D uDiffuseMap;
layout( binding = 5 ) uniform sampler2D uSpecularMap;

uniform vec3 LightPOS;

void main()
{
    ivec2 coord = ivec2(gl_FragCoord.xy);

    vec3 Position = texelFetch(uPosition, coord, 0).xyz;
    vec3 Normal = texelFetch(uNormalMap, coord, 0).xyz;
    vec3 Ambient = texelFetch(uAmbientMap, coord, 0).xyz;
    vec3 Diffuse = texelFetch(uDiffuseMap, coord, 0).xyz;
    vec4 Specular = texelFetch(uSpecularMap, coord, 0);

    vec3 lightDir = normalize(LightPOS - Position);
    vec3 viewDir = normalize(-Position); // View direction

    Normal = normalize(Normal);

    if (dot(lightDir, Normal) < 0)
        Normal = -Normal;

    float diffCoeff = max(dot(Normal, lightDir), 0.0);
    vec3 finalDiffuse = Diffuse * diffCoeff;

    float specCoeff = pow(max(dot(Normal, normalize(lightDir + viewDir)), 0.0), Specular.w);
    vec3 finalSpecular = Specular.xyz * specCoeff;

    fragColor = vec4(Ambient + finalDiffuse + finalSpecular, 1.0);
}
