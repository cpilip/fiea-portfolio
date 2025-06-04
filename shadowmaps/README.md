## Shadowmaps

This project can be compiled. Add GLM, GLFW, and GLEW libraries from `raytracer\libs` to environment variables:
- Set `GLM_INCLUDE_DIR` to `/path/to/GLM`
- Set `GLFW_DIR` to `/path/to/GLFW`
- Set `GLEW_DIR` to `/path/to/GLEW`

An executable can be found under `build`. This was made by generating a Visual Studio solution (Visual Studio 17 2022) on Windows using CMake with the following command, then compiling using the solution.

`cmake -S . -B build`

The demo can be run using the following arguments. Use the arrow keys to move the light/camera around, and the LMB to pan around the scene.

`> ShadowmappedLighting.exe ../../resources`
