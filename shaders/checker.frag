#version 300 es
in mediump vec2 uv_interp;
in mediump vec3 normal_interp;
in mediump vec4 position_interp;
in mediump vec4 pos_in_lightspace;

layout(location = 0) out mediump vec4 out_colour;
layout(location = 1) out mediump vec4 out_normal;
layout(location = 2) out mediump vec4 out_position;

uniform mediump vec3 view_pos;
uniform mediump vec3 light_pos;
uniform sampler2D shadow_map;

mediump float shadow_amount(mediump vec4 frag_light_pos)
{
    mediump vec3 proj_coords = frag_light_pos.xyz / frag_light_pos.w;
    proj_coords = proj_coords*0.5 + 0.5;
    mediump float closest_depth = texture(shadow_map, proj_coords.xy).r;
    mediump float current_depth = proj_coords.z;
    mediump float bias = 0.005;
    mediump float shadow  = current_depth - bias > closest_depth ? 0.8 : 0.0;
    return shadow;
}

void main()
{
    mediump float x = floor(uv_interp.x*8.0);
    mediump float y = floor(uv_interp.y*8.0);
    mediump vec3 tile_colour = mix(vec3(0.5, 0.3, 0.1), vec3(0.77, 0.74, 0.51), mod(x + mod(y,2.0),2.0));

    mediump vec3 norm = normalize(normal_interp);

    mediump vec3 light_dir = normalize(light_pos - position_interp.xyz);
    mediump float light = dot(light_dir, norm);
    light *= 1.0 - shadow_amount(pos_in_lightspace);

    mediump vec3 view_dir = normalize(view_pos - position_interp.xyz);
    mediump vec3 reflect_dir = reflect(-light_dir, norm);
    mediump float specular = pow(max(dot(view_dir, reflect_dir), 0.0), 64.0);

    out_colour = vec4((tile_colour + specular)*light, 1.0);
    out_normal = vec4(normal_interp, 1.0);
    out_position = position_interp;
}