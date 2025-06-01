#version 450

layout(location = 0) in vec2 F;

layout (location = 0) out vec4 o;

// layout( push_constant ) uniform PushConstants {
//   vec3 iResolution;           // viewport resolution (in pixels)
//   float iTime;                // shader playback time (in seconds)
// } pushConstants;

// #define iResolution pushConstants.iResolution
// #define iTime       pushConstants.iTime

/*
    -4 chars from iapafoto 333->329 (p-p+ instead of vec3())
*/

void main() {
    vec2 u = F;
    float i=0.0,d=0.0,s=0.0,t=1;
    vec3  q,p = vec3(2560, 1600, 1);
    o=vec4(0.0);
    u = (u-p.xy/2.)/p.y;
    for(o*=i; i++<1e2;
        d += s = .01 + min(.03+abs(9.- q.y)*.1,
                           .01+abs( 1.+ p.y)*.2),
        o += 1. /  s)
        for (q = p = vec3(u*d,d+t),s = .01; s < 2.;
             p += abs(dot(sin(p * s *24.), p-p+.01)) / s,
             q += abs(dot(sin(.3*t+q * s * 16.), p-p+.005)) / s,
             s += s);
    o = tanh(o * vec4(1,2,4,0) / 1e4 / length(u-.2));
}

/*

    Playing with turbulence and translucency from
    @Xor's recent shaders, e.g.
        https://www.shadertoy.com/view/wXjSRt
        https://www.shadertoy.com/view/wXSXzV
*/