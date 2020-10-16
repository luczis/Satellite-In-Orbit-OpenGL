#version 410

in vec3 vertex_pos;
in vec2 texture_coord;
in vec3 vertex_normal;
in vec3 view_pos;
in mat4 model_mat;

out vec4 color;

uniform vec3 light_pos;
uniform vec3 light_color;

uniform sampler2D texture_data;
uniform sampler2D normal_map_data;

void main(void)
{
	vec4 texture_color = texture(texture_data, texture_coord);
	float intensity = length(texture_color.rgb);
	color = vec4(0.6 * pow(intensity, 3) * texture_color.rgb, texture_color.a);
}
