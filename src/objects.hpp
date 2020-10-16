#pragma once
#include <iostream>
#include <string.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "lights.hpp"

class Object {
private:
	std::string shader_vert_file;
	std::string shader_frag_file;

	std::string obj_file;

	unsigned int shader_program;

	unsigned int vertex_buffer;
	unsigned int vertex_array;
	unsigned int index_buffer;

	unsigned int element_count = 0;

	unsigned int texture_id;
	unsigned int normal_map_id;

	unsigned int position_size;
	unsigned int texture_size;
	unsigned int normal_size;
	unsigned int face_size;

	unsigned char getLineValues();
	unsigned char getObjValues();
public:
	std::string name;
	glm::mat4 model_mat = glm::mat4(1.0f);

	unsigned char createBuffer(glm::mat4 initial_mat, std::string obj_file_path = "");
	unsigned char createShaderProgram(std::string shader_vertex, std::string shader_fragment);
	unsigned char createTexture(std::string texture_file_path = "", std::string normal_map_file_path = "");
	unsigned char draw(std::vector<Light> lights, glm::mat4* projection, glm::mat4* view, glm::mat4* model);

	glm::vec3 getPosition();
	glm::quat getRotation();

	~Object();
};
