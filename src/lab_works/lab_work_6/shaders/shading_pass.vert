#version 450

layout(location = 0) in vec2 aVertexPosition;

out vec2 texCoords;

void main()
{
    texCoords = aVertexPosition * 0.5 + 0.5; 
    gl_Position = vec4(aVertexPosition, 0.0, 1.0);
}
