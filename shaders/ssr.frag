#version 300 es

// lettier.github.io
uniform mediump mat4 projection;

uniform sampler2D colour_tex;
uniform sampler2D normal_tex;
uniform sampler2D position_tex;

out mediump vec4 out_colour;

struct parameters
{
    mediump float max_distance;
    mediump float resolution;
    int steps;
    mediump float thickness;
    mediump vec2 tex_size;
    mediump vec2 tex_coord;
};

mediump vec4 to_frag(mediump vec2 tex_size, mediump vec4 pos)
{
    mediump vec4 result = pos;

    // to screen space
    result = projection * result;
    // perspective divide
    result.xyz /= result.w;
    // to uv coordinates
    result.xy = result.xy * 0.5 + 0.5;
    // to fragment coordinates
    result.xy *= tex_size;

    return result;
}

struct reflection_result
{
    mediump float visible;
    mediump vec2 uv;
    mediump float len;
};

reflection_result get_reflection_uv(parameters params, mediump vec4 start, mediump vec4 end)
{
    reflection_result result;
    result.visible = 0.0;
    result.len = 0.0;

    mediump vec4 start_frag = to_frag(params.tex_size, start);
    mediump vec4 end_frag = to_frag(params.tex_size, end);
    mediump float max_length = length(end_frag.xy - start_frag.xy);

    // dda line drawing
    mediump float delta_x = end_frag.x - start_frag.x;
    mediump float delta_y = end_frag.y - start_frag.y;
    
    mediump float use_x = abs(delta_x) >= abs(delta_y) ? 1.0 : 0.0;
    mediump float delta = mix(abs(delta_y), abs(delta_x), use_x) * clamp(params.resolution, 0.0, 1.0);
    mediump vec2 increment = vec2(delta_x, delta_y) / max(delta, 0.001);

    mediump vec2 current_frag = start_frag.xy;
    mediump float current_length = length(current_frag.xy - start_frag.xy);

    mediump float depth = start.z;

    mediump float before_hit = 0.0;
    mediump float after_hit = 0.0;

    // pass 1
    int hit = 0;
    while (current_length < max_length) 
    {
        current_frag += increment;
        current_length = length(current_frag.xy - start_frag.xy);
        
        mediump vec2 current_coords = current_frag / params.tex_size;
        mediump vec4 current_pos = texture(position_tex, current_coords);

        mediump float a = mix(
            (current_frag.y - start_frag.y) / delta_y,
            (current_frag.x - start_frag.x) / delta_x,
            use_x
        );

        mediump float current_ray_depth = (start.z * end.z) / mix(end.z, start.z, a);
        mediump float depth_diff = current_ray_depth-current_pos.z;
        if (depth_diff > 0.0 && depth_diff < params.thickness) {
            after_hit = a;
            hit = 1;
            break;
        }
        before_hit = a;
    }

    // pass 2
    int steps = params.steps * hit;
    for (int i = 0 ; i < steps ; i++)
    {
        current_frag = mix(start_frag.xy, end_frag.xy, before_hit);
        mediump vec2 current_coords = current_frag / params.tex_size;
        mediump vec4 current_pos = texture(position_tex, current_coords);
        mediump float current_ray_depth = (start.z * end.z) / mix(end.z, start.z, after_hit);
        mediump float depth_diff = current_ray_depth-current_pos.z;
        if (depth_diff > 0.0 && depth_diff < params.thickness) {
            result.uv = current_coords;
            result.len = current_length;
            result.visible = 1.0;
            after_hit = before_hit + ((after_hit - before_hit) / 2.0);
        }
        else {
            mediump float temp = after_hit;
            after_hit = after_hit + ((after_hit - before_hit) / 2.0);
            before_hit = temp;
        }
    }

    return result;
}

void main() {
    parameters params;
    params.max_distance = 30.0;
    params.resolution = 0.3;
    params.steps = 30;
    params.thickness = 1.0;
    params.tex_size = vec2(textureSize(position_tex, 0).xy);
    params.tex_coord = gl_FragCoord.xy / params.tex_size;

    mediump vec4 position_from = texture(position_tex, params.tex_coord);
    mediump vec3 unit_position_from = normalize(position_from.xyz);

    mediump vec3 normal = normalize(texture(normal_tex, params.tex_coord).xyz);
    mediump vec3 pivot = normalize(reflect(unit_position_from, normal));

    mediump vec4 start = vec4(position_from.xyz, 1.0);
    mediump vec4 end = vec4(position_from.xyz + pivot*params.max_distance, 1.0);

    reflection_result result = get_reflection_uv(params, start, end);
    //result.visible *= (1.0 - max(dot(-unit_position_from, pivot), 0.0)); // not visible if pointed at the camera
    //result.visible *= (result.uv.x < 0.0 || result.uv.x > 1.0 ? 0.0 : 1.0)*
    //                  (result.uv.y < 0.0 || result.uv.y > 1.0 ? 0.0 : 1.0);
    //result.visible *= (1.0 - clamp(result.len / params.max_distance, 0.0, 1.0)); // fade out as the ray gets further away

    mediump vec3 reflection_colour = texture(colour_tex, result.uv).xyz*result.visible;
    mediump vec3 colour = texture(colour_tex, params.tex_coord).xyz;
    mediump vec3 normal_colour = texture(normal_tex, params.tex_coord).xyz;

    out_colour = vec4(reflection_colour, 1.0);
}