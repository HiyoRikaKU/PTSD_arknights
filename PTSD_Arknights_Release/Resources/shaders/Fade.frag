#version 410 core

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 fragColor;

uniform sampler2D surface;
uniform float alpha;

void main() {
    vec4 texColor = texture(surface, uv);
    
    // Apply alpha multiplier
    texColor.a *= alpha;

    if (texColor.a < 0.01)
        discard;

    fragColor = texColor;
}
