#pragma once
#include <iostream>
#include <string.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class Light {
private:
	std::string shader_vert_file;
	std::string shader_frag_file;

	std::string obj_file;

	unsigned int shader_program;

	unsigned int vertex_buffer;
	unsigned int vertex_array;
	unsigned int index_buffer;

	unsigned int element_count = 0;

	unsigned int position_size;
	unsigned int texture_size;
	unsigned int normal_size;
	unsigned int face_size;

	unsigned char getLineValues();
	unsigned char getObjValues();
public:
	glm::vec3 color;

	std::string name;
	glm::mat4 model_mat = glm::mat4(1.0f);

	unsigned char createBuffer(glm::mat4 initial_mat, std::string obj_file_path = "");
	unsigned char createShaderProgram(std::string shader_vertex, std::string shader_fragment);
	unsigned char draw(glm::mat4* projection, glm::mat4* view, glm::mat4* model);

	glm::vec3 getPosition();
	glm::quat getRotation();

	~Light();
};
