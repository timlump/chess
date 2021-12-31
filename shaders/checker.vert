#version 300 es

in vec3 position;
in vec3 normal;
in vec2 uv;

out vec3 normal_interp;
out vec2 uv_interp;
out vec4 position_interp;

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

void main()
{
    normal_interp = normal;
    uv_interp = uv;
    mat4 mvp = project*view*model;
    mediump vec4 pos = mvp*vec4(position, 1.0);
    position_interp = pos;
    gl_Position = pos;
}