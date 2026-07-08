#version 330 core
out vec4 FragColor;

uniform float uAlpha;
uniform vec3 uColor;

void main() {
    FragColor = vec4(uColor, uAlpha);
}