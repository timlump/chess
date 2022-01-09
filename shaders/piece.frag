#version 300 es
in mediump vec4 position_interp;
in mediump vec3 normal_interp;
in mediump vec2 uv_interp;

layout(location = 0) out mediump vec4 out_colour;
layout(location = 1) out mediump vec4 out_normal;
layout(location = 2) out mediump vec4 out_position;

uniform mediump vec3 light_pos;

void main()
{   
    mediump vec3 light_dir = normalize(light_pos - position_interp.xyz);
    mediump float light = dot(light_dir, normal_interp);

    out_colour = vec4(vec3(1.0)*light, 1.0);
    out_normal = vec4(normal_interp, 1.0);
    out_position = position_interp;
}