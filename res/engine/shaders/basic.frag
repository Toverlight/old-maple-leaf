#version 460 core
uniform sampler2D uTexture;
in vec3 ourColor;
in vec2 TexCoord;
out vec4 FragColor;
void main() {
    FragColor = texture(uTexture, TexCoord) * vec4(ourColor, 1.0);
}