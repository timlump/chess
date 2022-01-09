#version 300 es

in vec3 position;
in vec3 normal;
in vec2 uv;

out vec3 normal_interp;
out vec2 uv_interp;
out vec4 position_interp;
out vec4 pos_in_lightspace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;
uniform mat4 light_mat;

void main()
{
    uv_interp = uv;
    mat4 mv = view*model;
    mat4 normal_mat = transpose(inverse(mv));
    normal_interp = (normal_mat*vec4(normal,0)).xyz;

    pos_in_lightspace = light_mat * model * vec4(position, 1.0);

    mat4 mvp = project*mv;
    position_interp = mv*vec4(position,1.0);
    gl_Position = mvp*vec4(position, 1.0);
}