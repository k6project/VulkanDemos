#version 310 es

#if defined(VERTEX)
#define VARYING out
#elif defined(FRAGMENT)
#define VARYING in
#endif

layout (location = 0) VARYING vec4 vColor; 

#if defined(VERTEX)

const vec2 VERTICES[] = vec2[](
    vec2(0.0, -1.0), vec2(-1.0, 0.0), vec2( 1.0, 0.0),
    vec2(0.0,  1.0), vec2( 1.0, 0.0), vec2(-1.0, 0.0)
);

const vec3 COLORS[] = vec3[](
    vec3(1.0, 1.0, 0.0), vec3(1.0, 0.0, 0,0), vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0), vec3(1.0, 0.0, 0,0)
);

void main() 
{
    gl_Position = vec4(VERTICES[gl_VertexIndex], 0.0, 1.0);
    vColor = vec4(COLORS[gl_VertexIndex], 1.0);
}

#elif defined(FRAGMENT)

layout (location = 0) out vec4 outFragColor;

void main()
{
    outFragColor = vColor;
}

#endif
