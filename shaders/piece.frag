#version 300 es
in mediump vec4 position_interp;
in mediump vec3 normal_interp;
in mediump vec2 uv_interp;

layout(location = 0) out mediump vec4 out_colour;
layout(location = 1) out mediump vec4 out_normal;
layout(location = 2) out mediump vec4 out_position;

void main()
{   
    out_colour = vec4(vec3(0.5), 1.0);
    out_normal = vec4(normal_interp, 1.0);
    out_position = position_interp;
}