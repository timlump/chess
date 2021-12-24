#version 300 es
in mediump vec3 normal_interp;
in mediump vec2 uv_interp;
out mediump vec4 out_colour;

void main()
{   
    out_colour = vec4(normal_interp, 0.5);
}