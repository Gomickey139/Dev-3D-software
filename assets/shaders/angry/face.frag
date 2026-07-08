#version 330 core
out vec4 FragColor;

in vec3 localPos;
in vec3 Normal;

uniform float uHitFlash;
uniform float uDeathDark;

vec3 lightDir = normalize(vec3(0.25, 0.25, 1.0));

void main() {
    // 基本色
    vec3 baseColor = vec3(1.0, 0.25, 0.25);

    // ローカル座標の中心(0,0,0)からの距離を測る
    float dist = length(localPos);

    // 距離に応じて色を変える処理
    if(dist < 0.971) {
        baseColor = vec3(0.0);
    }

    vec3 norm = normalize(Normal);

    float layer = 1 - max(dot(norm, vec3(0.0, 0.0, -1)), 0.0);

    float diff = max(dot(norm, lightDir), 0.0);

    float ambient = 0.2;

    float lighting = diff + ambient;

    vec3 subColor = vec3(0.0);

    FragColor = mix(vec4(subColor, 1.0), vec4(baseColor, 1.0), lighting);
}