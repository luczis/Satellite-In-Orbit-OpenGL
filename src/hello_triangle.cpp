#include "hello_triangle.hpp"
#include <stdexcept>

int main(int argc, const char* argv[]) {
	Application take_me_to_the_moon;

	// Check if the .json file is passed when running the program
	if(argc < 2)
		throw std::runtime_error("Specify .json configuration file");

	take_me_to_the_moon.initializeApplication(argv[1]);
	take_me_to_the_moon.mainLoop();
	take_me_to_the_moon.terminateApplication();

	return 0;
}
