#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 localPos;
out vec3 Normal;

uniform mat4 model;      // どこに置くか、どう回転・拡大するか
uniform mat4 view;       // カメラをどこに置くか
uniform mat4 projection; // 遠近感（パース）のレンズ設定

void main() {
    localPos = aPos;

    FragPos = vec3(model * vec4(aPos, 1.0));

    Normal = mat3(model) * aNormal;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}