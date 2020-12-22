cmake_minimum_required (VERSION 3.10)
project (Bergimus)

# Only supporting Linux right now
if(NOT UNIX)
	message(WARNING "Not properly tested for your Operational System")
endif()

# Manage OpenGL policy and set fallback
cmake_policy(SET CMP0072 NEW)
include(FindOpenGL)
if(${OpenGL_GLX_FOUND} STREQUAL "FALSE")
	cmake_policy(SET CMP0072 OLD)
endif()

find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)

# The main executable
add_executable(Bergimus
  src/lights.cpp
  src/objects.cpp
  src/bergimus.cpp
)

# Copy config file
configure_file(config.json ${CMAKE_BINARY_DIR}/config.json COPYONLY)
file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})

set_property(TARGET Bergimus PROPERTY CXX_STANDARD 11)
target_compile_options(Bergimus PRIVATE -Wall)

# glfw
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_INSTALL OFF)
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
target_link_libraries(Bergimus PRIVATE glfw)

# glew
add_definitions(-DGLEW_STATIC)
add_subdirectory(lib/glew EXCLUDE_FROM_ALL)
target_link_libraries(Bergimus PRIVATE libglew_static)

# glm
add_subdirectory(lib/glm EXCLUDE_FROM_ALL)
target_link_libraries(Bergimus PRIVATE glm)

# jsoncpp
add_subdirectory(lib/jsoncpp EXCLUDE_FROM_ALL)
target_link_libraries(Bergimus PRIVATE jsoncpp)

# stb
include_directories("lib/stb")
