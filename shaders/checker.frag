#version 300 es
in mediump vec2 uv_interp;
in mediump vec3 normal_interp;
in mediump vec4 position_interp;

layout(location = 0) out mediump vec4 out_colour;
layout(location = 1) out mediump vec4 out_normal;
layout(location = 2) out mediump vec4 out_position;

void main()
{
    mediump float x = floor(uv_interp.x*8.0);
    mediump float y = floor(uv_interp.y*8.0);
    mediump vec3 tile_colour = vec3(1.0)*mod(x + mod(y,2.0),2.0);
    
    out_colour = vec4(tile_colour, 1.0);
    out_normal = vec4(normal_interp, 1.0);
    out_position = position_interp;
}