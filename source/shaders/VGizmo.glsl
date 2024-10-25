#version 330 core

uniform mat4 mvp;  // Model-View-Projection matrix
layout(location = 0) in vec3 aPos;
flat out int axisIndex;

void main()
{
    gl_Position = mvp * vec4(aPos, 1.0f);

    // Pass an index for each axis to use in the fragment shader
    if (aPos.x == 1.0) axisIndex = 0;  // X-axis
    else if (aPos.y == 1.0) axisIndex = 1;  // Y-axis
    else axisIndex = 2;  // Z-axis
}