#version 330 core
out vec4 FragColor;

in vec3 localPos;
in vec3 Normal;

void main() {
    vec3 local = normalize(localPos);
    float mask = smoothstep(0.9994, 0.9998, local.z);
    //mask = step(0.9995, local.z);
    vec3 color1 = vec3(1.0);
    vec3 color2 = vec3(1.0, 0.51, 0.51);
    FragColor = vec4(mix(color1, color2, mask), 1.0);
}