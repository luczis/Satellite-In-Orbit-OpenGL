#version 410

in vec3 position;
in vec2 texture;
in vec3 normal;

out vec3 vertex_pos;
out vec2 texture_coord;
out vec3 vertex_normal;
out vec3 view_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
	gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0);
	texture_coord = texture;
	vertex_normal = normalize(vec3(model * vec4(normal, 0.0)));
	vertex_pos = vec3(model * vec4(position.x, position.y, position.z, 1.0));
	view_pos = vec3(-view[3]);
}
