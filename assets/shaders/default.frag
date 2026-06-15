#version 330 core
out vec4 FragColor;

in vec3 Normal;

void main() {
    vec3 norm = normalize(Normal);
    vec3 color = (norm * 0.5 + 0.5) * 1.2;
    FragColor = vec4(color, 1.0);
}