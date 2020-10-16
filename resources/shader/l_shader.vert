#version 410

in vec3 position;
in vec2 texture;
in vec3 normal;

out vec4 vertex_color;
out vec2 texture_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
	gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0);
	texture_coord = texture;
	vertex_color = vec4(gl_Position.x, gl_Position.y, gl_Position.x * gl_Position.y, 1.0);
}
