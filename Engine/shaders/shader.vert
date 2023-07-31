#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in mat4 aModelMatrix;

out vec2 TexCoords;

void main()
{
    gl_Position = aModelMatrix * vec4(aPos, 0.0, 1.0);
    TexCoords = aTexCoords;
}
