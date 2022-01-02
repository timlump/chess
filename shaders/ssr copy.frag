#version 300 es

// lettier.github.io
uniform mediump mat4 projection;

uniform sampler2D colour_tex;
uniform sampler2D normal_tex;
uniform sampler2D position_tex;

out mediump vec4 out_colour;

void main() {
    // adjustments
    mediump float max_distance = 300.0; // max length of reflection ray
    mediump float resolution = 0.3; // how many fragments skipped
    int steps = 10; // how many second pass iterations
    mediump float thickness = 0.5; // threshold for hit

    mediump vec2 tex_size = vec2(textureSize(position_tex, 0).xy);
    mediump vec2 tex_coord = gl_FragCoord.xy / tex_size;

    mediump vec4 position_from = texture(position_tex, tex_coord);
    mediump vec3 unit_position_from = normalize(position_from.xyz);

    mediump vec3 normal = normalize(texture(normal_tex, tex_coord).xyz);
    mediump vec3 pivot = normalize(reflect(unit_position_from, normal));

    mediump vec4 position_to = position_from;

    // this effectively has + (pivot*0)
    mediump vec4 start_view = vec4(position_from.xyz, 1.0);
    mediump vec4 end_view = vec4(position_from.xyz + (pivot*max_distance), 1.0);

    mediump vec4 start_frag = start_view;
        // to screen space
        start_frag = projection * start_frag;
        // perspective divide
        start_frag.xyz /= start_frag.w;
        // to uv coordinates
        start_frag.xy = start_frag.xy * 0.5 + 0.5;
        // to fragment coordinates
        start_frag.xy *= tex_size;

    mediump vec4 end_frag = end_view;
        end_frag = projection * end_frag;
        end_frag.xyz /= end_frag.w;
        end_frag.xy = end_frag.xy * 0.5 + 0.5;
        end_frag.xy  *= tex_size;

    mediump vec2 frag = start_frag.xy;
    mediump vec4 uv = vec4(0.0);
        uv.xy = frag / tex_size;

    mediump float delta_x = end_frag.x - start_frag.x;
    mediump float delta_y = end_frag.y - start_frag.y;
    
    mediump float use_x = abs(delta_x) >= abs(delta_y) ? 1.0 : 0.0;
    mediump float delta = mix(abs(delta_y), abs(delta_x), use_x) * clamp(resolution, 0.0, 1.0);
    mediump vec2 increment = vec2(delta_x, delta_y) / max(delta, 0.001);

    // position of last known miss
    mediump float search0 = 0.0;
    // position between start and end fragment (0 to 1)
    mediump float search1 = 0.0;

    // intersection during first pass
    int hit0 = 0;
    // intersection during second pass
    int hit1 = 0;
    
    mediump float view_distance = start_view.z;
    mediump float depth = thickness;

    // pass 1
    mediump float i = 0.0;
    for (i = 0.0; i < 10.0 ; i++) {

        frag += increment;
        uv.xy = frag / tex_size;
        position_to = texture(position_tex, uv.xy);

        search1 = mix(
            (frag.y - start_frag.y) / delta_y,
            (frag.x - start_frag.x) / delta_x,
            use_x
        );
        // this uses perspective-correct interpolation - Kok-Lim Low 2002
        view_distance = (start_view.z * end_view.z) / mix(end_view.z, start_view.z, search1);
        depth = -(view_distance - position_to.z);

        // end of pass 1
        if (depth > 0.0 && depth < thickness) {
            hit0 = 1;
            break;
        }
        else {
            search0 = search1;
        }
    }

    // pass two
    search1 = search0 + ((search1 - search0) / 2.0);
    steps *= hit0; // skips the second pass if hit0 is 0

    for (int step = 0; step < steps; step++) {
        frag = mix(start_frag.xy, end_frag.xy, search1);
        uv.xy = frag / tex_size;
        position_to = texture(position_tex, uv.xy);

        view_distance = (start_view.z * end_view.z) / mix(end_view.z, start_view.z, search1);
        depth = -(view_distance - position_to.z);

        if (depth > 0.0 && depth < thickness) {
            hit1 = 1;
            search1 = search0 + ((search1 - search0) / 2.0);
        }
        else {
            mediump float temp = search1;
            search1 = search1 + ((search1 - search0) / 2.0);
            search0 = temp;
        }
    }

    mediump float visibility =
        float(hit1)* // not visible if no hit
        position_to.w* // no fragment is 0
        (1.0 - max(dot(-unit_position_from, pivot), 0.0))* // not visible if pointed at the camera
        (1.0 - clamp(depth / thickness, 0.0, 1.0))* // fade out if its not an exact hit
        (1.0 - clamp(length(position_to - position_from) / max_distance, 0.0, 1.0))* // fade out as the ray gets further away
        // only visible if in bounds
        (uv.x < 0.0 || uv.x > 1.0 ? 0.0 : 1.0)*
        (uv.y < 0.0 || uv.y > 1.0 ? 0.0 : 1.0);

    visibility = clamp(visibility, 0.0, 1.0);
    uv.ba = vec2(visibility);

    out_colour = texture(colour_tex, uv.xy)*visibility + texture(colour_tex, tex_coord);
}