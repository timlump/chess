#version 300 es

in mediump vec2 uv_interp;
out mediump vec4 out_colour;

uniform sampler2D colour_tex;

void main()
{   
    mediump vec2 uv = uv_interp;
    out_colour = texture(colour_tex, uv);
}