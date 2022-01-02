#version 300 es

in mediump vec2 uv_interp;
out mediump vec4 out_colour;

uniform sampler2D colour_tex;
uniform sampler2D normal_tex;
uniform sampler2D position_tex;

void main()
{   
    out_colour = texture(normal_tex, uv_interp);
}