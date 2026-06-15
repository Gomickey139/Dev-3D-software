#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

void main() {

    vec3 viewDir = normalize(-FragPos);

    vec3 norm = normalize(Normal);

    float NdotV = max(dot(norm, viewDir), 0.0);

    vec3 color = vec3(1.0);
    vec3 limColor = vec3(1.0, 0.0, 0.0);
    FragColor = vec4(mix(limColor, color, NdotV), 1.0);
}