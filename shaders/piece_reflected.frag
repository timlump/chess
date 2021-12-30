#version 300 es
in mediump vec3 normal_interp;
in mediump vec2 uv_interp;
in mediump float world_y;
out mediump vec4 out_colour;

void main()
{   
    mediump float alpha = (-1.0*world_y)*8.0;
    out_colour = vec4(normal_interp,1.0 - alpha);
}