#version 450

#define ITERATIONS 1

layout(binding = 0) uniform UniformBuffer {
	mat4 model;
	mat4 view;
	mat4 proj;
	vec2 resolution;
	uint time;
} ubo;

layout(location=0) out vec4 outColor;
layout(location=0) in vec3 fragColor;

float hashOld12(vec2 p)
{
	// Two typical hashes...
	return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);

	// This one is better, but it still stretches out quite quickly...
	// But it's really quite bad on my Mac(!)
	//return fract(sin(dot(p, vec2(1.0,113.0)))*43758.5453123);

}

vec3 hashOld33( vec3 p )
{
	p = vec3( dot(p,vec3(127.1,311.7, 74.7)),
			  dot(p,vec3(269.5,183.3,246.1)),
			  dot(p,vec3(113.5,271.9,124.6)));

	return fract(sin(p)*43758.5453123);
}

float hash11(float p)
{
	p = fract(p * .1031);
	p *= p + 33.33;
	p *= p + p;
	return fract(p);
}

//----------------------------------------------------------------------------------------
//  1 out, 2 in...
float hash12(vec2 p)
{
	vec3 p3  = fract(vec3(p.xyx) * .1031);
	p3 += dot(p3, p3.yzx + 33.33);
	return fract((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  1 out, 3 in...
float hash13(vec3 p3)
{
	p3  = fract(p3 * .1031);
	p3 += dot(p3, p3.zyx + 31.32);
	return fract((p3.x + p3.y) * p3.z);
}
//----------------------------------------------------------------------------------------
// 1 out 4 in...
float hash14(vec4 p4)
{
	p4 = fract(p4  * vec4(.1031, .1030, .0973, .1099));
	p4 += dot(p4, p4.wzxy+33.33);
	return fract((p4.x + p4.y) * (p4.z + p4.w));
}

//----------------------------------------------------------------------------------------
//  2 out, 1 in...
vec2 hash21(float p)
{
	vec3 p3 = fract(vec3(p) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx + 33.33);
	return fract((p3.xx+p3.yz)*p3.zy);

}

//----------------------------------------------------------------------------------------
///  2 out, 2 in...
vec2 hash22(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx+33.33);
	return fract((p3.xx+p3.yz)*p3.zy);

}

//----------------------------------------------------------------------------------------
///  2 out, 3 in...
vec2 hash23(vec3 p3)
{
	p3 = fract(p3 * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx+33.33);
	return fract((p3.xx+p3.yz)*p3.zy);
}

//----------------------------------------------------------------------------------------
//  3 out, 1 in...
vec3 hash31(float p)
{
	vec3 p3 = fract(vec3(p) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx+33.33);
	return fract((p3.xxy+p3.yzz)*p3.zyx);
}


//----------------------------------------------------------------------------------------
///  3 out, 2 in...
vec3 hash32(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yxz+33.33);
	return fract((p3.xxy+p3.yzz)*p3.zyx);
}

//----------------------------------------------------------------------------------------
///  3 out, 3 in...
vec3 hash33(vec3 p3)
{
	p3 = fract(p3 * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yxz+33.33);
	return fract((p3.xxy + p3.yxx)*p3.zyx);

}

//----------------------------------------------------------------------------------------
// 4 out, 1 in...
vec4 hash41(float p)
{
	vec4 p4 = fract(vec4(p) * vec4(.1031, .1030, .0973, .1099));
	p4 += dot(p4, p4.wzxy+33.33);
	return fract((p4.xxyz+p4.yzzw)*p4.zywx);

}

//----------------------------------------------------------------------------------------
// 4 out, 2 in...
vec4 hash42(vec2 p)
{
	vec4 p4 = fract(vec4(p.xyxy) * vec4(.1031, .1030, .0973, .1099));
	p4 += dot(p4, p4.wzxy+33.33);
	return fract((p4.xxyz+p4.yzzw)*p4.zywx);

}

//----------------------------------------------------------------------------------------
// 4 out, 3 in...
vec4 hash43(vec3 p)
{
	vec4 p4 = fract(vec4(p.xyzx)  * vec4(.1031, .1030, .0973, .1099));
	p4 += dot(p4, p4.wzxy+33.33);
	return fract((p4.xxyz+p4.yzzw)*p4.zywx);
}

//----------------------------------------------------------------------------------------
// 4 out, 4 in...
vec4 hash44(vec4 p4)
{
	p4 = fract(p4  * vec4(.1031, .1030, .0973, .1099));
	p4 += dot(p4, p4.wzxy+33.33);
	return fract((p4.xxyz+p4.yzzw)*p4.zywx);
}


void main(){
	vec2 fragCoord = gl_FragCoord.xy;
	vec2 iResolution = ubo.resolution;
	float iTime = float(ubo.time);
	/*
	//outColor=vec4(sin(ubo.time + fragCoord.x), sin(ubo.time - fragCoord.y), sin(ubo.time * fragCoord.x),1);


	vec2 position = fragCoord.xy;
	vec2 uv = fragCoord.xy / iResolution.xy;
	#if 1
	float a = 0.0, b = a;
	for (int t = 0; t < ITERATIONS; t++)
	{
		float v = float(t+1)*.152;
		vec2 pos = (position * v + iTime * 1500. + 50.0);
		a += hash12(pos.xy);
		b += hashOld12(pos);
	}
	vec3 col = vec3(mix(b, a, step(uv.x, .5))) / float(ITERATIONS);
	#else
	vec3 a = vec3(0.0), b = a;
	for (int t = 0; t < ITERATIONS; t++)
	{
		float v = float(t+1)*.132;
		vec3 pos = vec3(position, iTime*.3) + iTime * 500. + 50.0;
		a += hash33(pos);
		b += hashOld33(pos);
	}
	vec3 col = vec3(mix(b, a, step(uv.x, .5))) / float(ITERATIONS);
	#endif

    col = mix(vec3(.4, 0.0, 0.0), col, smoothstep(.5, .495, uv.x) + smoothstep(.5, .505, uv.x));
	iTime *= 0.0001;
	outColor = vec4(col * vec3(sin(iTime), cos(iTime), 1 / sin(iTime / 1000)), 1.0);
	*/
	vec2 uv = fragCoord/iResolution.xy;

	// Time varying pixel color
	vec3 col = 0.5 + 0.5*cos(iTime/100+uv.xyx+vec3(0,2,4));

	// Output to screen
	outColor = vec4(col,1.0);
}
