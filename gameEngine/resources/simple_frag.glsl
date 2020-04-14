#version 330 core 
in vec3 fragNor;
in vec3 WPos;
//to send the color to a frame buffer
layout(location = 0) out vec4 color;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float shine;

in vec3 V;

/* Very simple Diffuse shader with a directional light*/
void main()
{
	vec3 Dcolor, Scolor;

	vec3 Dlight = vec3(1, 1, 0);

	
	vec3 normal = normalize(fragNor);
	vec3 lightDir = normalize(Dlight);
	vec3 reflectDir = reflect(-1 * lightDir, normal);

	Dcolor = MatDif*max(dot(lightDir, normal), 0)+MatAmb;

	float p = dot(V, reflectDir);

	Scolor = MatSpec*pow(max(p, 0), shine);

	color = vec4(Dcolor, 1.0);
}
