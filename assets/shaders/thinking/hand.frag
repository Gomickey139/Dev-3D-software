#version 330 core
out vec4 FragColor;

in vec3 localPos;
in vec3 Normal;

uniform float uHitFlash;
uniform float uDeathDark;

vec3 lightDir = normalize(vec3(0.25, 0.25, 1.0));

void main() {
    vec3 baseColor = vec3(0.99, 1.0, 0.25);

    vec3 norm = normalize(Normal);

    float diff = max(dot(norm, lightDir), 0.0);

    float ambient = 0.2;

    float lighting = diff + ambient;

    vec3 subColor = vec3(0.0);

    FragColor = mix(vec4(subColor, 1.0), vec4(baseColor, 1.0), lighting);

    vec4 flashColor = mix(FragColor, vec4(1.0), 0.8);

    FragColor = mix(FragColor, flashColor, uHitFlash);
    FragColor = mix(FragColor, vec4(0.0), uDeathDark);
}