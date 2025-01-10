#version 450
in vec3 vColor; 
out vec4 fragColor;
//uniform float uBrightness;
void main() {
	 fragColor = vec4(vColor, 1.0);
}
