#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 projection; // Orthographic（平行投影）行列を受け取る
uniform mat4 model;      // 位置とサイズ

void main() {
    gl_Position = projection * model * vec4(aPos.x, aPos.y, 0.0, 1.0);
    TexCoord = aTexCoord;
}