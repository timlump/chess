#version 300 es
in vec3 position;
in vec2 uv;
out vec2 uv_interp;
void main()
{
    uv_interp = uv;
    gl_Position = vec4(position, 1.0);
}