#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

in vec3 WorldPos[];

uniform mat4 projection;
uniform mat4 view;
float grassHeight = 1.0; // Adjust the height as needed

void main() {
    vec3 pos = WorldPos[0]; // World space position from vertex shader

    // Define the bottom and top points of the grass blade in world space
    vec3 bottomLeft = pos;
    vec3 topLeft = pos + vec3(0.0, grassHeight, 0.0);

    // Define the width of the grass blade as some fraction of the height
    float width = grassHeight * 0.1;
    vec3 bottomRight = bottomLeft + vec3(width, 0.0, 0.0);
    vec3 topRight = topLeft + vec3(width, 0.0, 0.0);

    // Transform the points by the view and projection matrices
    vec4 bl = projection * view * vec4(bottomLeft, 1.0);
    vec4 tl = projection * view * vec4(topLeft, 1.0);
    vec4 br = projection * view * vec4(bottomRight, 1.0);
    vec4 tr = projection * view * vec4(topRight, 1.0);

    // Emit the first triangle
    gl_Position = bl;
    EmitVertex();
    gl_Position = tl;
    EmitVertex();
    gl_Position = br;
    EmitVertex();

    // Emit the second triangle
    gl_Position = tr;
    EmitVertex();
    gl_Position = tl;
    EmitVertex();
    gl_Position = br;
    EmitVertex();

    EndPrimitive();
}
