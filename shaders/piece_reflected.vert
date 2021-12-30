#version 300 es

in vec3 position;
in vec3 normal;
in vec2 uv;

out vec3 normal_interp;
out vec2 uv_interp;
out float world_y;

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

void main()
{
    normal_interp = normal;
    uv_interp = uv;
    vec4 world_pos = model*vec4(position,1.0);
    world_y = world_pos.y;
    mat4 mvp = project*view*model;
    gl_Position = mvp*vec4(position, 1.0);
}