#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform sampler2D grass;

void main()
{
    vec2 trimmedTexCoords = vec2(TexCoords.x, TexCoords.z);
    FragColor = texture(grass, trimmedTexCoords);
}