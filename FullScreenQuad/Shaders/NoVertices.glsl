#version 310 es

#if defined(VERTEX)
#define VARYING out
#elif defined(FRAGMENT)
#define VARYING in
#endif

layout (location = 0) VARYING highp vec2 vTexCoord0;

#if defined(VERTEX)

const vec2 VERTICES[] = vec2[](
    vec2(-1.0, 1.0), vec2(1.0, -1.0), vec2(-1.0, -1.0),
    vec2( 1.0, 1.0), vec2(1.0, -1.0), vec2(-1.0,  1.0)
);

void main() 
{
    gl_Position = vec4(VERTICES[gl_VertexIndex], 0.0, 1.0);
    vTexCoord0 = 0.5 * VERTICES[gl_VertexIndex] + 0.5;
}

#elif defined(FRAGMENT)

precision highp float;

layout (location = 0) out vec4 outFragColor;

void main()
{
    vec3 top = mix(vec3(0.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0), vTexCoord0.x);
    vec3 bottom = mix(vec3(1.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0), vTexCoord0.x);
    outFragColor = vec4(mix(top, bottom, vTexCoord0.y), 1.0);
}

#endif
