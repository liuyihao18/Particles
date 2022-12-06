#version 330 core
out vec4 FragColor;

uniform sampler2D texture1;

in vec3 ourColor;
in vec2 TexCoord;

void main(void)
{
    FragColor = mix(texture(texture1, TexCoord), vec4(ourColor, 1.0), 0.0);
}
