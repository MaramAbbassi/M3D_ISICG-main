#version 450
layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aColor;
/*uniform mat4 uModelMatrix; 
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;*/
uniform mat4 uMVPMatrix;
out vec3 vColor;

void main() {
	gl_Position =uMVPMatrix * vec4(aVertexPosition, 1.0);
	vColor = aColor;
}
