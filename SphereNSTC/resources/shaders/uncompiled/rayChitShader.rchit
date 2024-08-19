#version 460
#extension GL_KHR_raytracing : require

layout(location = 0) rayPayloadInKHR vec3 ResultColor;
layout(location = 1) hitAttributeKHR vec2 HitAttribs;

void main() {
    const vec3 barycentrics = vec3(1.0f - HitAttribs.x - HitAttribs.y, HitAttribs.x, HitAttribs.y);
    ResultColor = vec3(barycentrics);
}