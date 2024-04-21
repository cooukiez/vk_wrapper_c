#version 450

layout (binding = 0) uniform UBO {
    mat4 data;
} ubo;

layout (location = 0) out vec3 frag_pos;
layout (location = 1) out vec2 uv;
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_uv;

layout (push_constant) uniform PushConstant {
    mat4 view;
    vec2 res;
    uint time;
} pc;

void main() {
    gl_Position = pc.view * vec4(in_pos, 1.0);
    frag_pos = in_pos;
    uv = in_uv;
}
