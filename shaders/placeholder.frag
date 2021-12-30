#version 300 es

in mediump vec2 uv_interp;
out mediump vec4 out_colour;

uniform sampler2D tex;

void main()
{   
    out_colour = texture(tex, uv_interp);
}