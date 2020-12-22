# Satellite In Orbit OpenGL

A software with proper scale of a satellite orbiting earth, utilizing OpenGL for rendering the models, with shadows and reflection. It also provides a proper skeletal structure for incrementating the amout of models and customizing them via a .json file.

Developed from scratch, using OpenGL, GLFW and Glew.

With keys inputs for movement, and a .json configuration file for tweaking the simulation and window parameters.

# How to build

First, this project utilizes git submodules, so please clone using the --recursive flag.

```
git clone --recursive https://github.com/luczis/Bergimus
```

Standard building process:

```
mkdir build
cd build
cmake ..
make
```

Run via command line, and specify the config.json file utilized, example:

```
./Bergimus config.json
```

# Results

Sample video showing the software running with default settings:

![Video of a simulation of a satellite and earth](./md/output.gif)

