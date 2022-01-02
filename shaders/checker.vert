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
    mat4 normal_mat = transpose(inverse(model));
    normal_interp = (normal_mat*vec4(normal,0)).xyz;
    uv_interp = uv;
    mat4 mv = view*model;
    mat4 mvp = project*mv;
    position_interp = mv*vec4(position,1.0);
    gl_Position = mvp*vec4(position, 1.0);
}