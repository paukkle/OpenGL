#shader vertex
#version 410 core

layout(location = 0) in vec4 position;  // location, glVertexAttribPointer eka argumentti

void main()
{
   gl_Position = position;
};


#shader fragment
#version 410 core

layout(location = 0) out vec4 color;  // location, glVertexAttribPointer eka argumentti

void main()
{
   color = vec4(1.0, 0.0, 0.0, 1.0);
};