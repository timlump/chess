#version 300 es

uniform uvec3 id_colour;
out mediump vec4 out_colour;

void main()
{   
    out_colour = vec4(id_colour, 1.0);
}