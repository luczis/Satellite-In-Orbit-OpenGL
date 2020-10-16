#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <json/reader.h>
#include <string.h>
#include <vector>
#include "json/json.h"
#include "json/writer.h"

#include "objects.hpp"
#include "mathFunk.hpp"

#define APPLICATION_FAILURE -1
#define APPLICATION_SUCCESS 0

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

class objclass{
private:
	int shaderProgram;
    	unsigned int VBO, VAO;

public:
	void createbuffer();
	void createshader();

	void draw();
};

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

void objclass::createbuffer() {
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    }; 

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

}

void objclass::createshader() {
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void objclass::draw() {
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);
}

class Application {
private:
	GLFWwindow* window;
	uint16_t width;
	uint16_t height;

	Json::Value config;

	//std::vector<Object> world_objects;
	objclass test;
	uint8_t createObjects();
	uint8_t drawObjects();

public:
	uint8_t initializeApplication(std::string config_file);
	uint8_t mainLoop();
	uint8_t terminateApplication();
};

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
	//glfwWindowHint(GLFW_SAMPLES, config["Window"]["MSAA"].asInt());

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

	// Print info
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
/*
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);



	glewExperimental = GL_TRUE;
	glewInit();
*/
	test.createbuffer();
	test.createshader();

	// Create Objects
	//createObjects();


	
	while(!glfwWindowShouldClose(window)) {

		// Rendering step
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw every object created
		//drawObjects();
		test.draw();
		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}
	return APPLICATION_SUCCESS;
}

uint8_t Application::mainLoop() {


	return APPLICATION_SUCCESS;
}

uint8_t Application::terminateApplication(){
	glfwTerminate();
	return APPLICATION_SUCCESS;
}

//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow *window);

// settings
/*
class maintest {
public:
	void mainfunction();
};

int main() {
	maintest a;
	a.mainfunction();
	return 0;
}

void maintest::mainfunction()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glewExperimental = GL_TRUE;
	glewInit();


	objclass test;
	test.createbuffer();
	test.createshader();

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        //processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
		test.draw();
        // glBindVertexArray(0); // no need to unbind it every time 
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
 //   glDeleteVertexArrays(1, &VAO);
 //   glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
}*/

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
/*void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}*/
