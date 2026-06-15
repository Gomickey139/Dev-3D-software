#version 330 core
out vec4 FragColor;

in vec3 Normal;

vec3 lightDir = normalize(vec3(0.5, 1.0, 1.0));

void main() {

    vec3 baseColor = vec3(0.0);

    vec3 norm = normalize(Normal);

    float diff = max(dot(norm, lightDir), 0.0);

    float ambient = 0.2;

    float lighting = diff + ambient;

    FragColor = vec4(baseColor, 1.0) * lighting;
}