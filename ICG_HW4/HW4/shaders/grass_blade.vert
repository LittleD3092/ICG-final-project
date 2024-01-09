#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in float bladeLength;

out vec3 WorldPos;
out vec2 TexCoords;
out float vbladeLength;

uniform mat4 model;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
    WorldPos = vec3(model * vec4(aPos, 1.0));
    vbladeLength = bladeLength;
}
