# Controls
## Camera
- WASD to move around local space relative to camera
- UP/DOWN, LEFT/RIGHT to rotate
- SHIFT + UP/DOWN to move up/down in global space
- hold SHIFT with WASD to move in global space
- UP/DOWN arrow keys move vertically in global space

## Feature specific
- N + UP/DOWN adjusts near plane
- F + UP/DOWN adjusts far plane
- SHIFT + RIGHT/LEFT arrow keys toggle through scenes
- IJKL to move light in the first scene with the stone henge
    - IJKL to move directional light direction
    - SHIFT IJKL to move point light
    - CTRL IJKL to move the spot light
- CTRL + SPACEBAR to cycle through post processing effects
- CTRL + SHIFT + SPACEBAR to cycle through post processing effects
- CTRL + B to toggle wireframe mode
- LEFT/ RIGHT bracket ("[", "]") to adjust tesselation level(0-64)
    - Warnings 
        - If you exceed tesselation level 32 graphics cards with less than 4GB or VRAM will crash
        - This feature tanks performance, please turn off to not lose your mind when grading

## Where to find the features
- Multi-texturing on the cube with the whittington logo on it
- Alpha masking on the cloud in the middle of the stone henge
- Time and wave based pixel shader is the flat plane with the scrolling rock texture in the middle of the stone henge
- OBJ2Header is the stonehenge in scene1
- Procedural plane with 400 verts in scene2
- Indexed model is the teapot & the cubes
- InstancedDraw() is in scene 2, I have like 12 teapots being instanced
- Geometry instancing is in scene 2 right on top of the InstancedDraw() teapots i have 2 more teapots being instanced through a geometry shader
- Point to quad is done in scene1 there are 4 white quads generated from a pointlist
- Modified vertex grid is in scene2, I have a waving mesh
- MSAA enabled in code, samplecount set to 4
- POST PROCESSING
 - Cycle through post processing effects by using CTRL + SPACEBAR to move forward and CTRL + SHIFT + SPACEBAR to sycle in reverse order
 - Effects in order are 
    - No effect, default state
    - Blur
    - Black & White
    - Warp
    - Depth based fog
    - No effect (saving for later)
 - Fog requires you to zoom out to see it in effect(it is adjusted to show the fog effect without zooming too far, this was hard coded)
 - Transparent stuff is the 3 see-trhough cubes in scene 1
 - Emission map on wood cube with text on it in scene 2, the text is emissive
 - Tesselation on stone henge and mountain in mountain theme scene