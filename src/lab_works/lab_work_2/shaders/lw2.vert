#version 450
layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec3 aColor;
out vec3 vColor;
uniform float uTranslationX; 
void main() {
	gl_Position = vec4(aVertexPosition.x + uTranslationX, aVertexPosition.y, 0.0, 1.0);
	vColor = aColor;
}
