#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <json/reader.h>
#include <string.h>
#include <string>
#include <vector>
#include <chrono>
#include "json/json.h"
#include "json/writer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "lights.hpp"
#include "objects.hpp"
#include "mathFunk.hpp"

#define APPLICATION_FAILURE -1
#define APPLICATION_SUCCESS 0

namespace keys {
	enum keystatusvalues {
		UP_KEY,
		DOWN_KEY,
		RIGHT_KEY,
		LEFT_KEY,
		W_KEY,
		A_KEY,
		S_KEY,
		D_KEY,
		Y_KEY,
		U_KEY,
		I_KEY,
		O_KEY,
		H_KEY,
		J_KEY,
		K_KEY,
		L_KEY,
		SPACE_KEY,
		CTRL_KEY,
		SHIFT_KEY,
		NR_OF_KEYS_STATUS
	};
}
bool keyStatus[keys::NR_OF_KEYS_STATUS];

class Application {
private:
	GLFWwindow* window;
	int width;
	int height;

	int past_width;
	int past_height;

	Json::Value config;

	std::vector<Light> world_lights;
	std::vector<Object> world_objects;

	uint8_t createObjects();
	uint8_t drawObjects();

	unsigned int earth_number = 0;
	unsigned int satellite_number = 0;

	float camera_yaw = 0;
	float camera_pitch = 0;
	float camera_distance = 5.0f;

	float rotation_speed = 0.0f;
	float zoom_speed = 0.0f;

	float camera_min_distance = 0.0f;
	float camera_max_distance = 0.0f;
	float camera_max_pitch = 0.0f;

	float day_hours = 1.0f;
	float time_multiplier = 1.0f;

	std::chrono::high_resolution_clock::time_point system_time;
	std::chrono::high_resolution_clock::time_point past_system_time;
	std::chrono::duration<float> time_span;
	float real_time_sec = 0.0f;

	float satellite_speed = 0.0f;
	float satellite_height = 0.0f;

	float wheel_nw_acc = 0.0f;
	float wheel_ne_acc = 0.0f;
	float wheel_sw_acc = 0.0f;
	float wheel_se_acc = 0.0f;
	
public:
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	uint8_t initializeApplication(std::string config_file);
	uint8_t mainLoop();
	uint8_t terminateApplication();

	static void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mode) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(win, GL_TRUE);
		if (key == GLFW_KEY_W)
			keyStatus[keys::W_KEY] = action;
		if (key == GLFW_KEY_A)
			keyStatus[keys::A_KEY] = action;
		if (key == GLFW_KEY_S)
			keyStatus[keys::S_KEY] = action;
		if (key == GLFW_KEY_D)
			keyStatus[keys::D_KEY] = action;
		if (key == GLFW_KEY_Y)
			keyStatus[keys::Y_KEY] = action;
		if (key == GLFW_KEY_U)
			keyStatus[keys::U_KEY] = action;
		if (key == GLFW_KEY_I)
			keyStatus[keys::I_KEY] = action;
		if (key == GLFW_KEY_O)
			keyStatus[keys::O_KEY] = action;
		if (key == GLFW_KEY_H)
			keyStatus[keys::H_KEY] = action;
		if (key == GLFW_KEY_J)
			keyStatus[keys::J_KEY] = action;
		if (key == GLFW_KEY_K)
			keyStatus[keys::K_KEY] = action;
		if (key == GLFW_KEY_L)
			keyStatus[keys::L_KEY] = action;

		if (key == GLFW_KEY_UP)
			keyStatus[keys::UP_KEY] = action;
		if (key == GLFW_KEY_DOWN)
			keyStatus[keys::DOWN_KEY] = action;
		if (key == GLFW_KEY_RIGHT)
			keyStatus[keys::RIGHT_KEY] = action;
		if (key == GLFW_KEY_LEFT)
			keyStatus[keys::LEFT_KEY] = action;
	}
};

uint8_t Application::createObjects() {
	for(char i = 0; !config["Lights"][std::to_string(i)].empty(); i++) {
		Light new_object;
		new_object.name = config["Lights"][std::to_string(i)]["Name"].asString();
		world_lights.push_back(new_object);
	}
	for(char i = 0; !config["Objects"][std::to_string(i)].empty(); i++) {
		Object new_object;
		new_object.name = config["Objects"][std::to_string(i)]["Name"].asString();
		if(new_object.name.compare("Earth") == 0)
			earth_number = i;
		else if(new_object.name.compare("Satellite") == 0)
			satellite_number = i;
		world_objects.push_back(new_object);
	}
	
	for(char i = 0; i < (char)world_lights.size(); i++) {
		// Define initial settings parameters of light
		float x_scl = config["Lights"][std::to_string(i)]["Scale"]["X"].asFloat();
		float y_scl = config["Lights"][std::to_string(i)]["Scale"]["Y"].asFloat();
		float z_scl = config["Lights"][std::to_string(i)]["Scale"]["Z"].asFloat();
		float x_pos = config["Lights"][std::to_string(i)]["Position"]["X"].asFloat();
		float y_pos = config["Lights"][std::to_string(i)]["Position"]["Y"].asFloat();
		float z_pos = config["Lights"][std::to_string(i)]["Position"]["Z"].asFloat();
		float x_rot = config["Lights"][std::to_string(i)]["Rotate"]["X"].asFloat();
		float y_rot = config["Lights"][std::to_string(i)]["Rotate"]["Y"].asFloat();
		float z_rot = config["Lights"][std::to_string(i)]["Rotate"]["Z"].asFloat();
		float angle = config["Lights"][std::to_string(i)]["Rotate"]["Angle"].asFloat();
		float r_color = config["Lights"][std::to_string(i)]["Color"]["R"].asFloat();
		float g_color = config["Lights"][std::to_string(i)]["Color"]["G"].asFloat();
		float b_color = config["Lights"][std::to_string(i)]["Color"]["B"].asFloat();
		glm::mat4 model_mat = glm::translate(glm::mat4(1.0f), glm::vec3(x_pos, y_pos, z_pos));
		model_mat = glm::rotate(model_mat, glm::radians(angle), glm::vec3(x_rot, y_rot, z_rot));
		model_mat = glm::scale(model_mat, glm::vec3(x_scl, y_scl, z_scl));

		world_lights[i].color = glm::vec3(r_color, g_color, b_color);
		world_lights[i].createShaderProgram(config["Lights"][std::to_string(i)]["Shader"]["Vertex"].asString(), config["Lights"][std::to_string(i)]["Shader"]["Fragment"].asString());
		world_lights[i].createBuffer(model_mat, config["Lights"][std::to_string(i)]["Obj File"].asString());
	}
	
	for(char i = 0; i < (char)world_objects.size(); i++) {
		// Define initial settings parameters of object
		float x_scl = config["Objects"][std::to_string(i)]["Scale"]["X"].asFloat();
		float y_scl = config["Objects"][std::to_string(i)]["Scale"]["Y"].asFloat();
		float z_scl = config["Objects"][std::to_string(i)]["Scale"]["Z"].asFloat();
		float x_pos = config["Objects"][std::to_string(i)]["Position"]["X"].asFloat();
		float y_pos = config["Objects"][std::to_string(i)]["Position"]["Y"].asFloat();
		float z_pos = config["Objects"][std::to_string(i)]["Position"]["Z"].asFloat();
		float x_rot = config["Objects"][std::to_string(i)]["Rotate"]["X"].asFloat();
 		float y_rot = config["Objects"][std::to_string(i)]["Rotate"]["Y"].asFloat();
		float z_rot = config["Objects"][std::to_string(i)]["Rotate"]["Z"].asFloat();
		float angle = config["Objects"][std::to_string(i)]["Rotate"]["Angle"].asFloat();
		glm::mat4 model_mat = glm::translate(glm::mat4(1.0f), glm::vec3(x_pos, y_pos, z_pos));
		model_mat = glm::rotate(model_mat, glm::radians(angle), glm::vec3(x_rot, y_rot, z_rot));
		model_mat = glm::scale(model_mat, glm::vec3(x_scl, y_scl, z_scl));

		world_objects[i].createShaderProgram(config["Objects"][std::to_string(i)]["Shader"]["Vertex"].asString(), config["Objects"][std::to_string(i)]["Shader"]["Fragment"].asString());
		world_objects[i].createBuffer(model_mat, config["Objects"][std::to_string(i)]["Obj File"].asString());
		world_objects[i].createTexture(config["Objects"][std::to_string(i)]["Texture"].asString(), config["Objects"][std::to_string(i)]["Normal_Map"].asString());
	}

	return APPLICATION_SUCCESS;
}

uint8_t Application::drawObjects() {
	glm::vec3 rotation_center;
	for(char i = 0; i < (char)world_lights.size(); i++) {
		model = world_lights[i].model_mat;
		if(world_lights[i].name.compare("Sun") == 0) {
			// Rotate around earth
			rotation_center = world_objects[earth_number].getPosition();
			model = glm::translate(glm::mat4(1.0f), -rotation_center) * glm::rotate(glm::mat4(1.0f), math::m_2_pi/(day_hours * 3600.0f) * time_multiplier * real_time_sec, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), rotation_center) * model;
			world_lights[i].model_mat = model;
		}
		world_lights[i].draw(&projection, &view, &model);
	}
	satellite_height = glm::length(world_objects[earth_number].getPosition() - world_objects[satellite_number].getPosition());
	float satellite_angular_speed = satellite_speed/(satellite_height) * time_multiplier * real_time_sec;
	camera_yaw -= satellite_angular_speed;
	for(char i = 0; i < (char)world_objects.size(); i++) {
		model = world_objects[i].model_mat;
		if(world_objects[i].name.compare("Skybox") == 0) {
			model = glm::rotate(glm::mat4(1.0f), 0.00005f * time_multiplier * real_time_sec, glm::vec3(0.0f, 1.0f, 0.0f)) * model;
		}
		else if(world_objects[i].name.compare("Earth") == 0) {
			//Rotate around satellite
			rotation_center = world_objects[satellite_number].getPosition();
			model = glm::translate(glm::mat4(1.0f), rotation_center) * glm::rotate(glm::mat4(1.0f), satellite_angular_speed, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), -rotation_center) * model;

			//Rotate around itself
			model = glm::rotate(model, -satellite_angular_speed, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if(world_objects[i].name.compare("Earth Clouds") == 0) {
			//Rotate around satellite
			rotation_center = world_objects[satellite_number].getPosition();
			model = glm::translate(glm::mat4(1.0f), rotation_center) * glm::rotate(glm::mat4(1.0f), satellite_angular_speed, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), -rotation_center) * model;

			//Rotate around itself
			model = glm::rotate(model, -satellite_angular_speed - 0.0001f * time_multiplier * real_time_sec, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if(world_objects[i].name.compare("Satellite") == 0) {
			//model = glm::rotate(glm::mat4(1.0f), wheel_ne_acc * time_multiplier * real_time_sec, glm::vec3(1.0f, -1.0f, 1.0f)) * glm::rotate(glm::mat4(1.0f), wheel_nw_acc * time_multiplier * real_time_sec, glm::vec3(1.0f, -1.0f, -1.0f)) * glm::rotate(glm::mat4(1.0f), wheel_sw_acc * time_multiplier * real_time_sec, glm::vec3(-1.0f, -1.0f, -1.0f)) * glm::rotate(glm::mat4(1.0f), wheel_se_acc * time_multiplier * real_time_sec, glm::vec3(-1.0f, -1.0f, 1.0f)) * model;
			model = glm::rotate(model, 0.01f * (keyStatus[keys::Y_KEY]-keyStatus[keys::H_KEY]) * time_multiplier * real_time_sec, glm::vec3(1.0f, -1.0f, 1.0f));
			model = glm::rotate(model, 0.01f * (keyStatus[keys::U_KEY]-keyStatus[keys::J_KEY]) * time_multiplier * real_time_sec, glm::vec3(1.0f, -1.0f, -1.0f));
			model = glm::rotate(model, 0.01f * (keyStatus[keys::I_KEY]-keyStatus[keys::K_KEY]) * time_multiplier * real_time_sec, glm::vec3(-1.0f, -1.0f, -1.0f));
			model = glm::rotate(model, 0.01f * (keyStatus[keys::O_KEY]-keyStatus[keys::L_KEY]) * time_multiplier * real_time_sec, glm::vec3(-1.0f, -1.0f, 1.0f));
			//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.01f));
			//model = glm::rotate(model, 0.001f * time, glm::vec3(0.0f, 1.0f, 0.0f));
			//rotation_center = world_objects[1].getPosition();
			//model = glm::translate(glm::mat4(1.0f), rotation_center) * glm::rotate(glm::mat4(1.0f), 0.01f * time, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), -rotation_center) * model;
		}
		world_objects[i].draw(world_lights, &projection, &view, &model);
	}

	// Satellite is Object 2, and the center of view
	//rotation_center = world_objects[3].getPosition() + glm::vec3(0.0f, 0.0f, 5.0f);
	//view = glm::translate(glm::mat4(1.0f), -rotation_center) * glm::rotate(glm::mat4(1.0f), 0.01f * time, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), rotation_center) * view;
	//view = glm::rotate(glm::mat4(1.0f), 0.01f * time, glm::vec3(0.0f, 1.0f, 0.0f)) * view;
	return APPLICATION_SUCCESS;
}

uint8_t Application::initializeApplication(std::string config_file) {
	// Open config json file
	std::fstream config_fstream;
	config_fstream.open(config_file, std::fstream::in | std::fstream::out);
	Json::CharReaderBuilder reader_builder;
	Json::StreamWriterBuilder writer_builder;
	const std::unique_ptr<Json::StreamWriter> writer(writer_builder.newStreamWriter());
	reader_builder["collectComments"] = true;
	JSONCPP_STRING json_errs;
	if(!parseFromStream(reader_builder, config_fstream, &config, &json_errs)) {
		throw std::runtime_error(std::string("Error with .json file: \n") + json_errs);
		return APPLICATION_FAILURE;
	}

	// Set minimum OpenGL version to 3.3 (Modern OpenGL)
	if(((config["OpenGL"]["Version"]["Major"].asUInt() == 3) && (config["OpenGL"]["Version"]["Minor"].asUInt() < 3)) || (config["OpenGL"]["Version"]["Major"] < 3)) {
		config["OpenGL"]["Version"]["Major"] = 3;
		config["OpenGL"]["Version"]["Minor"] = 3;
		// Reopen file
		config_fstream.close();
		config_fstream.open(config_file, std::fstream::in | std::fstream::out | std::fstream::trunc);
		writer->write(config, &config_fstream);
	}

	// Initialize GLFW
	if(!glfwInit())
		return APPLICATION_FAILURE;

	// GLFW settings
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config["OpenGL"]["Version"]["Major"].asInt());
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config["OpenGL"]["Version"]["Minor"].asInt());
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, config["Window"]["MSAA"].asInt());

	// Get initial size
	width = config["Window"]["Size"]["Width"].asUInt();
	height = config["Window"]["Size"]["Height"].asUInt();

	// Create window
	switch(str2hash(config["Window"]["Type"].asCString())) {
		// If Borderless
		case str2hash("Borderless"):
			glfwWindowHint(GLFW_DECORATED, GL_FALSE);
			glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
			{
				const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
				glfwWindowHint(GLFW_RED_BITS, vidmode->redBits);
				glfwWindowHint(GLFW_GREEN_BITS, vidmode->greenBits);
				glfwWindowHint(GLFW_BLUE_BITS, vidmode->blueBits);
				glfwWindowHint(GLFW_REFRESH_RATE, vidmode->refreshRate);
				width = vidmode->width;
				height = vidmode->height;
				window = glfwCreateWindow(width, height, config["Window"]["Title"].asCString(), NULL, NULL);
			}
			glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
			break;
		// If Fullscreen
		case str2hash("Fullscreen"):
			window = glfwCreateWindow(config["Window"]["Size"]["Width"].asUInt(), config["Window"]["Size"]["Height"].asUInt(), config["Window"]["Title"].asCString(), glfwGetPrimaryMonitor(), NULL);
			glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
			break;
		// If Windowed
		case str2hash("Windowed"):
			window = glfwCreateWindow(width, height, config["Window"]["Title"].asCString(), NULL, NULL);
			glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
			break;
		default:
			throw std::runtime_error(std::string("Invalid window type ") + config["Window"]["Type"].asCString());
			break;
	}
	if(!window)
	{
		glfwTerminate();
		throw std::runtime_error("Error creating window");
		return APPLICATION_FAILURE;
	}
	glfwMakeContextCurrent(window);

	// Initialize glew
	glewExperimental = GL_TRUE;
	GLenum glew_err = glewInit();
	if(glew_err != GLEW_OK) {
		glfwTerminate();
		throw std::runtime_error(std::string("Error initializing GLEW, error: ") + (const char*)glewGetErrorString(glew_err));
		return APPLICATION_FAILURE;
	}

	// Callbacks
	glfwSetKeyCallback(window, keyCallback);

	// Print info
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;

	// Create Objects
	createObjects();
	glViewport(0, 0, width, height);
	
	// Initial camera
	rotation_speed = config["View"]["Camera"]["Rotation Speed"].asFloat();
	zoom_speed = config["View"]["Camera"]["Zoom Speed"].asFloat();
	camera_min_distance = config["View"]["Camera"]["Min Distance"].asFloat();
	camera_max_distance = config["View"]["Camera"]["Max Distance"].asFloat();
	camera_max_pitch = config["View"]["Camera"]["Max Pitch Radians"].asFloat();
	view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	projection = glm::perspective(glm::radians(config["View"]["FOV"].asFloat()), (float)width/height, 0.1f, glm::radians(config["View"]["Distance"].asFloat()));
	
	// Simulation parameters
	day_hours = config["Simulation"]["Day Hours"].asFloat();
	time_multiplier = config["Simulation"]["Time Multiplier"].asFloat();
	satellite_speed = config["Simulation"]["Satellite"]["Orbital Speed[Km/h]"].asFloat()/3600.0f;

	return APPLICATION_SUCCESS;
}

uint8_t Application::mainLoop() {

	// OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glRenderMode(GL_RENDER);
	glClearColor(0.2f, 0.3f, 0.3f, 0.0f);

	past_width = width;
	past_height = height;
	system_time = std::chrono::high_resolution_clock::now();
	while(!glfwWindowShouldClose(window)) {
		// Rendering step
		glClear(GL_DEPTH_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw every object created
		drawObjects();

		// Swap front and back buffers
		glfwSwapBuffers(window);
		
		// Update window size
		glfwGetWindowSize(window, &width, &height);
		if((past_width != width) || (past_height != height)) {
			past_width = width;
			past_height = height;
			projection = glm::perspective(glm::radians(config["View"]["FOV"].asFloat()), (float)width/height, 0.1f, glm::radians(config["View"]["Distance"].asFloat()));
			glViewport(0, 0, width, height);
		}

		// Update Camera
		if(keyStatus[keys::LEFT_KEY])
			camera_yaw += rotation_speed;
		if(keyStatus[keys::RIGHT_KEY])
			camera_yaw -= rotation_speed;
		if(keyStatus[keys::UP_KEY])
			camera_pitch += rotation_speed;
		if(keyStatus[keys::DOWN_KEY])
			camera_pitch -= rotation_speed;
		if(keyStatus[keys::W_KEY])
			camera_distance *= (1.0f-zoom_speed);
		if(keyStatus[keys::S_KEY])
			camera_distance *= (1.0f+zoom_speed);
	
		camera_pitch = std::min(std::max(camera_pitch, -camera_max_pitch), camera_max_pitch);
		camera_distance = std::min(std::max(camera_distance, camera_min_distance), camera_max_distance);
		view = glm::lookAt(camera_distance * glm::vec3(cos(camera_yaw) * cos(camera_pitch), sin(camera_pitch), sin(camera_yaw) * cos(camera_pitch)), -world_objects[satellite_number].getPosition(), glm::vec3(0.0f, 1.0f, 0.0f));

		// Poll for and process events
		glfwPollEvents();

		// Get time
		past_system_time = system_time;
		system_time = std::chrono::high_resolution_clock::now();
		time_span = std::chrono::duration_cast<std::chrono::duration<float>>(system_time - past_system_time);
		real_time_sec = time_span.count();
	}

	return APPLICATION_SUCCESS;
}

uint8_t Application::terminateApplication(){
	glfwTerminate();
	return APPLICATION_SUCCESS;
}

