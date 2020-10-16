#version 410

in vec4 vertex_color;
in vec2 texture_coord;

out vec4 color;

uniform sampler2D texture_data;
uniform sampler2D normal_map_data;

uniform vec3 light_color;

void main(void)
{
	color = vec4(light_color.r, light_color.g, light_color.b, 1.0);
}
