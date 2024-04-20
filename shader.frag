#version 450

layout(location=0) out vec4 outColor;

layout(location=0) in vec3 fragColor;

void main(){
	vec2 fragCoord = gl_FragCoord.xy;
	outColor=vec4((fragCoord / vec2(1920, 1080)).yxy,1);
}
