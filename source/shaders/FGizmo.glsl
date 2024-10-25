#version 330 core

flat in int axisIndex;
out vec4 color;

void main()
{
    if (axisIndex == 0) color = vec4(1.0, 0.0, 0.0, 1.0);  // Red for x-axis
    else if (axisIndex == 1) color = vec4(0.0, 1.0, 0.0, 1.0);  // Green for y-axis
    else color = vec4(0.0, 0.0, 1.0, 1.0);  // Blue for z-axis
}