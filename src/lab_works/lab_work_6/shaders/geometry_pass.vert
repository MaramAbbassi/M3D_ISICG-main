#version 450

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexNormal;
layout(location = 2) in vec2 aVertexTexCoords;
layout(location = 3) in vec3 aVertexTangent;
layout(location = 4) in vec3 aVertexBitagent;

uniform mat4 uMVPMatrix; // Projection * View * Model
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;
uniform mat4 uViewMatrix;

out vec3 normal;
out vec3 fragPos;
out vec2 texCoords;
out vec3 lightPos;
out mat3 TBN;

uniform vec3 LightPos;

void main()
{
    texCoords = aVertexTexCoords;

    normal = mat3(uNormalMatrix) * aVertexNormal;

    vec3 T = normalize(vec3(uModelMatrix * vec4(aVertexTangent, 0.0f)));
    vec3 B = normalize(vec3(uModelMatrix * vec4(aVertexBitagent, 0.0f)));
    vec3 N = normalize(vec3(uModelMatrix * vec4(aVertexNormal, 0.0f)));

    T = normalize(T - dot(T, N) * N); 
    B = cross(N, T);

    mat3 TBN = mat3(T, B, N);
    mat3 inv_TBN = transpose(TBN); 

    lightPos = vec3(uViewMatrix * vec4(LightPos, 1.0f)) * inv_TBN;
    fragPos = vec3(uViewMatrix * uModelMatrix * vec4(aVertexPosition, 1.0f)) * inv_TBN;

    gl_Position = uMVPMatrix * vec4(aVertexPosition, 1.0f);
}
