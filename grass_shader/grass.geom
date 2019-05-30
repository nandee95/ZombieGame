#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 80) out;

const float divisions = 4.0f;

#define PI 3.1415926535897932384626433832795
#define HALF_PI PI/2.0f

out vec2 texCoords;
uniform vec2 strength;
uniform vec2 size = vec2(0.5,0.5);

const float texStep = 1.0f/divisions;
float h = size.y / divisions * strength.y;
float halfX = size.x /2.0f;

in vec4 vColor[];
out vec4 Color;

void main() {
	vec4 pos = gl_in[0].gl_Position;
	
	float angle = 0.0f;
	Color = vColor[0];
	for(int i = 0 ; i <= divisions ; i++)
	{
		texCoords = vec2(0.0f,1.0f-texStep*i);
		gl_Position = pos+vec4(sin(angle-HALF_PI)*halfX,cos(angle-HALF_PI)*halfX,0,0);
		EmitVertex();
		texCoords = vec2(1.0f,1.0f-texStep*i);
		gl_Position = pos+vec4(sin(angle+HALF_PI)*halfX,cos(angle+HALF_PI)*halfX,0,0);
		EmitVertex();
		if(i!=divisions)
		{
			angle += strength.x;
			pos.x += sin(angle)*h;
			pos.y += cos(angle)*h;
		}
	}
	EndPrimitive();
}