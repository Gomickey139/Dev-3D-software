#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec3 uColor;
uniform float uAlpha;
uniform sampler2D uTexture;
uniform bool uUseTexture;

void main() {
    if(uUseTexture) {
        vec4 texColor = texture(uTexture, TexCoord);
        FragColor = texColor * vec4(uColor, uAlpha);
    } else {

        FragColor = vec4(uColor, uAlpha);
    }
}