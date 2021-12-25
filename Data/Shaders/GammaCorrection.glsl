-- Vertex

#version 450

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 0) out vec2 fragTexCoord;

void main() {
    fragTexCoord = vertexTexCoord;
    gl_Position = vec4(vertexPosition, 1.0);
}

-- Fragment

#version 450

layout(binding = 0) uniform sampler2D textureSampler;
layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 fragColor;

// Converts linear RGB to sRGB
vec3 toSRGB(vec3 u) {
    return mix(1.055 * pow(u, vec3(1.0 / 2.4)) - 0.055, u * 12.92, lessThanEqual(u, vec3(0.0031308)));
}

void main() {
    vec4 linearColor = texture(textureSampler, fragTexCoord);
    fragColor = vec4(toSRGB(linearColor.rgb), linearColor.a);
}
