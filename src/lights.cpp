#include "lights.hpp"

#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <streambuf>
#include <string>
#include <vector>
#include <stdexcept>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec2 texture;
	glm::vec3 normal;
};

unsigned char Light::createBuffer(glm::mat4 initial_mat, std::string obj_file_path) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	model_mat = initial_mat;

	if(!obj_file_path.empty()) {
		obj_file = obj_file_path;
		
		std::ifstream obj_stream;
		obj_stream.open(obj_file);
		if(!obj_stream.good()) {
			throw std::runtime_error(std::string("Failed to open obj file: ")+obj_file);
			return -1;
		}

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texture;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> face;

		std::string line;
		unsigned char i = 0;
		while(!obj_stream.eof()) {
			std::getline(obj_stream, line);
			std::stringstream lineStream(line);

			std::getline(lineStream, line, ' ');
			// Get type
			// Position
			if(line.compare("v") == 0) {
				i = 0;
				glm::vec3 temp_pos;
				while(std::getline(lineStream, line, ' ')) {
					temp_pos[i] = std::stof(line);
					i++;
				}
				positions.push_back(temp_pos);
			}

			// Texture
			else if(line.compare("vt") == 0) {
				i = 0;
				glm::vec2 temp_pos;
				while(std::getline(lineStream, line, ' ')) {
					temp_pos[i] = std::stof(line);
					i++;
				}
				texture.push_back(temp_pos);
			}

			// Normal
			else if(line.compare("vn") == 0) {
				i = 0;
				glm::vec3 temp_pos;
				while(std::getline(lineStream, line, ' ')) {
					temp_pos[i] = std::stof(line);
					i++;
				}
				normals.push_back(temp_pos);
			}

			// Face
			else if(line.compare("f") == 0) {
				while(std::getline(lineStream, line, ' ')) {
					i = 0;
					glm::vec3 temp_pos;
					std::stringstream subLineStream(line);
					while(std::getline(subLineStream, line, '/')) {
						temp_pos[i] = std::stof(line);
						i++;
					}
					face.push_back(temp_pos);
				}
			}
		}

		// Pass data to vertex
		for(unsigned int i = 0; i < face.size(); i++) {
			Vertex temp_vertex;
			temp_vertex.position = positions[face[i].x-1];
			temp_vertex.texture = texture[face[i].y-1];
			temp_vertex.normal = normals[face[i].z-1];
			vertices.push_back(temp_vertex);
		}
		for(unsigned int i = 0; i < vertices.size(); i++) {
			indices.push_back(i);
		}
	}
	else { // Initialize as a square if no obj is given
		Vertex v1;
		v1.position = glm::vec3{0.5f, 0.5f, 0.0f};
		v1.texture = glm::vec2{1.0f, 1.0f};
		Vertex v2;
		v2.position = glm::vec3{0.5f, -0.5f, 0.0f};
		v2.texture = glm::vec2{1.0f, 0.0f};
		Vertex v3;
		v3.position = glm::vec3{-0.5f, -0.5f, 0.0f};
		v3.texture = glm::vec2{0.0f, 0.0f};
		Vertex v4;
		v4.position = glm::vec3{-0.5f, 0.5f, 0.0f};
		v4.texture = glm::vec2{0.0f, 1.0f};
		vertices.push_back(v1);
		vertices.push_back(v2);
		vertices.push_back(v3);
		vertices.push_back(v4);
		indices = {	0,	1,	3,
					1,	2,	3};
	}

	// Create buffers
	glGenVertexArrays(1, &vertex_array);
	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &index_buffer);

	// Bind buffers and send data
	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// Save the amount of elements to draw
	element_count = indices.size();

	// Position for shaders
	int attribute_location = 0;

	// Position
	glVertexAttribPointer(attribute_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
	glEnableVertexAttribArray(attribute_location);
	glBindAttribLocation(shader_program, attribute_location, "position");
	attribute_location++;

	// Texture
	glVertexAttribPointer(attribute_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texture)));
	glEnableVertexAttribArray(attribute_location);
	glBindAttribLocation(shader_program, attribute_location, "texture");
	attribute_location++;
	
	// Normal
	glVertexAttribPointer(attribute_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(attribute_location);
	glBindAttribLocation(shader_program, attribute_location, "normal");
	attribute_location++;

	// Unbind
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return 0;
}

unsigned char Light::createShaderProgram(std::string shader_vertex, std::string shader_fragment) {
	shader_vert_file = shader_vertex;
	shader_frag_file = shader_fragment;

	// VERTEX
	// Input vertex shader file to string 
	std::ifstream vert_fstream(shader_vert_file);
	const std::string vert_string((std::istreambuf_iterator<char>(vert_fstream)), (std::istreambuf_iterator<char>()));
	if(vert_string.empty()){
		throw std::runtime_error(std::string("Invalid vertex shader file: ")+shader_vert_file);
		return -1;
	}
	const char* vert_c_str = vert_string.c_str();

	// Create and compile vertex shader
	unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1 , &vert_c_str, nullptr);
	glCompileShader(vertex_shader);

	// FRAGMENT
	// Input fragment shader file to string 
	std::ifstream frag_fstream(shader_frag_file);
	const std::string frag_string((std::istreambuf_iterator<char>(frag_fstream)), (std::istreambuf_iterator<char>()));
	if(frag_string.empty()){
		throw std::runtime_error(std::string("Invalid fragment shader file: ")+shader_frag_file);
		return -1;
	}
	const char* frag_c_str = frag_string.c_str();

	// Create and compile fragment shader
	unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1 , &frag_c_str, nullptr);
	glCompileShader(fragment_shader);

	// Get compiler debug info
	int vertex_debug_id, fragment_debug_id;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_debug_id);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_debug_id);
	if(vertex_debug_id == GL_FALSE) {
		int length;
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &length);
		char* debug_log = new char[length];
		glGetShaderInfoLog(vertex_shader, length, &length, debug_log);
		
		std::cout << "Vertex shader failed to compile: " << shader_vertex << std::endl;
		std::cout << debug_log << std::endl;
	}
	if(fragment_debug_id == GL_FALSE) {
		int length;
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &length);
		char* debug_log = new char[length];
		glGetShaderInfoLog(fragment_shader, length, &length, debug_log);
		
		std::cout << "Fragment shader failed to compile: " << shader_fragment << std::endl;
		std::cout << debug_log << std::endl;
	}
	if((vertex_debug_id == GL_FALSE) || (fragment_debug_id == GL_FALSE)) {
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		throw std::runtime_error("Failed to compile shader");
		return -1;
	}
	
	// Create program
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	int link_debug_id;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &link_debug_id);
	if(link_debug_id == GL_FALSE) {
		int length;
		glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &length);
		char* debug_log = new char[length];
		glGetProgramInfoLog(shader_program, length, &length, debug_log);
		
		std::cout << "Program shader failed to link: " << shader_fragment << std::endl;
		std::cout << debug_log << std::endl;

		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		glDeleteShader(shader_program);
		throw std::runtime_error("Failed to link shader");
		return -1;
	}
	glValidateProgram(shader_program);

	// Clean up
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return 0;
}

unsigned char Light::draw(glm::mat4* projection, glm::mat4* view, glm::mat4* model) {
	model_mat = (*model);
	glUseProgram(shader_program);

	// Pass uniform data
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, &(*projection)[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, &(*view)[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, &model_mat[0][0]);
	glUniform3f(glGetUniformLocation(shader_program, "light_color"), color.r, color.g, color.b);

	// Buffers
	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

	// Draw call
	glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, 0);
	return 0;
}

glm::vec3 Light::getPosition() {
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(model_mat, scale, rotation, translation, skew,perspective);
	return translation;
}

glm::quat Light::getRotation() {
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(model_mat, scale, rotation, translation, skew,perspective);
	rotation = glm::conjugate(rotation);
	return rotation;
}

Light::~Light() {
	glDeleteShader(shader_program);
}
