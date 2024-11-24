#version 330 core

in vec2 TexCoord;

uniform sampler2D texture;
uniform vec2 resolution;
uniform int radius;

void main() {
    vec2 texelSize = 1.0 / resolution;
    vec4 color = vec4(0.0);

    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            color += texture2D(texture, TexCoord + vec2(x, y));
        }
    }

    gl_FragColor = color / ((radius * 2) - 1);
}
