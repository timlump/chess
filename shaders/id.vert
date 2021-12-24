#version 300 es

in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

void main()
{
    mat4 mvp = project*view*model;
    gl_Position = mvp*vec4(position, 1.0);
}