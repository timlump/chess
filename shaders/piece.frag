#version 300 es
in mediump vec2 uv_interp;
out mediump vec4 outColor;

void main()
{   
    outColor = vec4(uv_interp, 1.0, 1.0);
}