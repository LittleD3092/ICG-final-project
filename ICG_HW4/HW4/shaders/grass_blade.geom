#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

in vec3 WorldPos[];
in vec2 TexCoords[];
in float vbladeLength[];

out vec2 fragTexCoords;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 windshift;

void main() {
    float grassHeight = vbladeLength[0];

    vec3 pos = WorldPos[0]; // World space position from vertex shader

    // Define the bottom and top points of the grass blade in world space
    vec3 bottomLeft = pos;
    vec3 topLeft = pos + vec3(0.0, grassHeight, 0.0) + windshift;

    // Define the width of the grass blade as some fraction of the height
    float width = grassHeight * 0.1;
    vec3 bottomRight = bottomLeft + vec3(width, 0.0, 0.0);
    vec3 topRight = topLeft + vec3(width, 0.0, 0.0) + windshift;

    // Transform the points by the view and projection matrices
    vec4 bl = projection * view * vec4(bottomLeft, 1.0);
    vec4 tl = projection * view * vec4(topLeft, 1.0);
    vec4 br = projection * view * vec4(bottomRight, 1.0);
    vec4 tr = projection * view * vec4(topRight, 1.0);

    // Calculate the texture coordinates for each vertex
    vec2 texCoordsBottomLeft = TexCoords[0];
    vec2 texCoordsTopLeft = TexCoords[0] + vec2(0.0, 0.1);
    vec2 texCoordsBottomRight = TexCoords[0] + vec2(0.1, 0.0);
    vec2 texCoordsTopRight = TexCoords[0] + vec2(0.1, 0.1);

    // Emit the first triangle
    gl_Position = bl;
    fragTexCoords = texCoordsBottomLeft;
    EmitVertex();
    gl_Position = tl;
    fragTexCoords = texCoordsTopLeft;
    EmitVertex();
    gl_Position = br;
    fragTexCoords = texCoordsBottomRight;
    EmitVertex();

    // // Emit the second triangle
    // gl_Position = tr;
    // fragTexCoords = texCoordsTopRight;
    // EmitVertex();
    // gl_Position = tl;
    // fragTexCoords = texCoordsTopLeft;
    // EmitVertex();
    // gl_Position = br;
    // fragTexCoords = texCoordsBottomRight;
    // EmitVertex();

    EndPrimitive();
}
