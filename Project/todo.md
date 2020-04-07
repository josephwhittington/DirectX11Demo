# Next episode
- Figure out what the fuck a render to texture is

# Shit that needs to get done
- add local, global rotation variables
- Fix transparency
- fix the sorting for transparency
- add moving camera
- add multiple viewports
- add themese

## Serious Known bugs
- Fix the hardcoded lighting values

## First Milestone - Depreciating Points
- (Complex Materials) Shader that modifies outgoing data using sin, cos etc.
## Second milestone depreciating points
- (Complex Materials) Reflective material
- (Geometry) - Use DrawInstanced()/ DrawIndexedInstanced()
- (Geometry) - Procedurally created geometry (Geometry shader), such as point to quad, particles or whatever
- (Geometry) - Use vertex shader to make a waving flag

### High Priority Shit
- Point Light
- Spotlight
- Multpile light support
- Add rotation support for models
- Handle Window Resize
- Procedural line mesh
- Skybox
- Waving flag
- Cube mapped reflections
- Alpha blending with depth sorting
- Fix FBX Loader

### Low Priority Shit (Nice to have's) [may become high priority if the project calls for rearchitecting]
- OBJ Loader
- OBJ/ FBX to Header exporter
- OBJ/ FBX property editor
- Single-file Shaders
	- This would use a pragma style header to mark the beginning of a shader type 
	- The shader would then concat the types into a string and then compile
- Shader class that stores the precompiled binaries in memory and keeps shit low key