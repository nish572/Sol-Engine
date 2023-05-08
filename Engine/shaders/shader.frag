#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture1;
uniform vec4 color;

void main() {
    vec4 sampled = texture(texture1, TexCoord);
    if (sampled.a < 0.1) {
        discard;
    }
    FragColor = color * sampled;
}
