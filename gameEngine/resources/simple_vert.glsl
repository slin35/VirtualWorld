#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 MV;
out vec3 fragNor;
out vec3 WPos;

uniform mat4 view;

out vec3 V;

void main()
{
	gl_Position = P * view * MV * vertPos;
	fragNor = (view * MV * vec4(vertNor, 0.0)).xyz;
	WPos = vec3(view * MV*vertPos);
	V = normalize(-1*(vec3(view* MV *vertPos)));
}
