#version 310 es
precision mediump float;
precision highp int;

layout(location = 0) out highp vec4 outFragColor;

//layout(location = 0) in highp vec3 inColor;

void main()
{
    outFragColor = vec4(0.0, 0.5, 0.5, 1.0);//vec4(inColor, 1.0);
}

