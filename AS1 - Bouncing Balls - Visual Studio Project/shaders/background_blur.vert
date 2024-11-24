#version 330 core

layout(location = 0) in vec2 position; // Position attribute
layout(location = 1) in vec2 texCoords; // Texture coordinates attribute

out vec2 TexCoord; // Pass texture coordinates to fragment shader

void main() {
    gl_Position = vec4(position, 0.0, 1.0); // Set the position
    TexCoord = texCoords; // Pass the texture coordinates to the fragment shader
}