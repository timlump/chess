#version 300 es
in mediump vec2 uv_interp;
out mediump vec4 outColor;

void main()
{
    mediump float x = floor(uv_interp.x*8.0);
    mediump float y = floor(uv_interp.y*8.0);
    mediump vec3 tile_colour = vec3(1.0)*mod(x + mod(y,2.0),2.0);
    
    outColor = vec4(tile_colour, 1.0);
}