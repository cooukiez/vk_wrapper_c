#version 450

layout (location = 0) out vec4 out_color;

layout (location = 0) in vec3 frag_pos;
layout (location = 1) in vec2 uv;

void main() {
    /*
    vec2 frag = gl_FragCoord.xy;
    vec2 res = pc.res;
    float time = float(pc.time);
    vec2 uv = frag / res.xy;

    vec3 col = 0.5 + 0.5 * cos(time / 100 + uv.xyx + vec3(0, 2, 4));
    out_color = vec4(col, 1.0);
    */

    out_color = vec4(1);
}
