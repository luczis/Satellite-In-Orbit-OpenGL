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

	const float ambient_coefficient = 0.8;
	const float diffuse_coefficient = 0.3;
	const float specular_coefficient = 0.3;

	vec3 light_direction = normalize(light_pos - vertex_pos);
	vec3 view_direction = normalize(view_pos - vertex_pos);
	vec3 reflect_direction = reflect(-light_direction, vertex_normal);

	float specular_intensity = specular_coefficient * pow(max(dot(view_direction, reflect_direction), 0.0), 32);
	float normal_intensity = diffuse_coefficient * max(dot(vertex_normal, light_direction), 0.0);

	vec3 rgb_color = (ambient_coefficient + normal_intensity + specular_intensity) * light_color * texture_color.rgb;
	color = vec4(rgb_color, texture_color.a);
}
