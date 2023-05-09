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

uniform vec4 u_Color;

void main()
{
	color = u_Color;
};