#version 330 core
out vec4 FragColor;

in vec2 fragTexCoords;

uniform sampler2D grass_blade;

void main()
{
    FragColor = texture(grass_blade, fragTexCoords);
}