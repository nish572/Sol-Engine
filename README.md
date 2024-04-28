# Sol-Engine

## To Run Sol
To run Sol, please download the latest release from the sidebar. Open the directory and find the Sol Engine.exe. Run this file to run Sol. Please ensure SDL2.dll and Engine.dll are present in the same directory (alternatively install to PATH, however for ease of use it is ideal to keep these .dll files in the same directory as the executable). Additionally, ensure the imgui configuration file is present in this same directory to allow the Editor to load with the same visual settings as I have set.

If running Sol in the Visual Studio solution debugger, please ensure SDL2.dll is copied and pasted directly into the appropriate directory (or installed to PATH) outlined below.
- For running the Visual Studio solution for the Debug-x64 configuration, this SDL2.dll should be pasted inside the following directory: "\Sol-Engine\Sol-Engine\bin\Debug-x64\Editor".
- For running the Visual Studio solution for the Release-x64 configuration, this SDL2.dll should be pasted inside the following directory: "\Sol-Engine\Sol-Engine\bin\Release-x64\Editor".
(Please note that the specific location of the Sol-Engine root directory will vary depending on where you download this codebase to on your system.)

### Known Issues
- Certain images may load either in black/white, or not load at all. However, this appears to be an issue with the STB image loading library and appears to only affect loading images imported from Apple phones. I believe this issue arises due to the manner in which Apple (and certain other devices) constructs the metadata of images from their platform. Please be careful when choosing images to load, and should this error present an issue, simply delete the affecting image, reload the Editor, and use a different image.
- PNG blending issues. Images with transparent backgrounds load fine, with a transparent background. However, upon displaying them over other images, the background of the PNG turns black. This issue will be resolved in the future by introducing manual image layer setting to allow images to be layered more appropriately. OpenGL blending and depth testing has been enabled, and this allow PNGs to appear with a transparent background, but only when the background is not displayed over another image. This issue arises from the Painter's Algorithm, and will be rectified by introducing appropriate image layering functionality.
- Transform Component scalar value modification leads to faulty physics. If you wish to resize an Entity on the screen, please directly modify the size values within the Sprite Component in the Inspector viewport, and  then directly modify the size values within the Collider Component (width/height for a Box collider, radius for a square collider) in the Inspector viewport.

### Further Work
- The introduction of a 'max force/impulse/torque' would enable forces/impulses/torques to be applied until a maximum value is reached. Additionally, a 'key press limiter' would enable the resultant action of a key press to only be applied under certain circumstances, e.g. if the player is on the ground the key can be pressed to jump otherwise this action will not be executed.
- Layer introduction to allow determination of which physics interact (i.e. physics bodies can be set to operate on specific layers to prevent collisions with physics from other layers), and to allow determination of which order sprites should be rendered on the screen (i.e. sprites with a lower layer will be rendered behind sprites with a higher layer).
- Audio Element and Audio System implementation.
- Full completion of the shader system to bring this functionality to the Editor. Currently mostly exists onthe backend and allows shader creation from two separate shader files (one for a vertex shader, one for a fragment shader), however as of present, rendering uses the default shader programs created.

## External Libraries
- spdlog
  - Usage: To build the Log Element to provide solution-wide logging functionality to a .txt file for debugging purposes.
  -	Found at: https://github.com/gabime/spdlog
  -	License: MIT
-	OpenGL 3.3
  -	Usage: To interact with the GPU to achieve hardware-accelerated rendering.
  -	OpenGL 3.3 is not a library, but rather a standard or specification supported by the graphics drivers of desktop devices. The specific version I have chosen to use however is OpenGL 3.3.
  -	Information found at: https://www.opengl.org/
  -	License: Not Applicable
-	GLAD
  -	Usage: To simplify the process of managing OpenGL function pointers, as a loader-generator, a utility (of various implementations) which The Khronos Group (the group responsible for the deployment of OpenGL) instructs developers to make use of in order to use OpenGL.
  -	Found at: https://glad.dav1d.de/
  -	License: The GLAD loader-generator itself is under the MIT license however the code it generates is under the Apache License. (GitHub, 2017)
-	SDL2
  -	Usage: To manage the window upon which an OpenGL context can be used for rendering. Additionally, to poll keyboard and mouse input.
  -	Found at: https://www.libsdl.org/
  -	License: zlib
-	GLM
  -	Usage: For matrix calculations for transformation data representation.
  -	Found at: https://www.opengl.org/sdk/libs/GLM/
  -	License: MIT
-	Box2D
  -	Usage: As the physics engine powering the physics of this game engine. Specifically used in the Physics System of the ECS in order to provide a physics world for collision detection and resolution as well as physics simulations.
  -	Found at: https://github.com/erincatto/box2d
  -	License: MIT
-	Dear ImGui
  -	Usage: To build the Graphical User Interface of the Editor.
  -	Found at: https://github.com/ocornut/imgui
  -	License: MIT
-	JSON for Modern C++
  -	Usage: For scene serialisation and deserialisation. Specifically used by the Scene Element in order to load and unload scenes by deserialising and serialising (respectively) the ECS’s main data structure of Entity-Components mappings into .scn files which are essentially JSON files. 
  -	Found at: https://github.com/nlohmann/json
  -	License: MIT
-	STB image loader (stb_image.h)
  -	Usage: To load images into a texture format usable by the Render System.
  -	Found at: https://github.com/nothings/stb/blob/master/stb_image.h
  -	License: Both MIT and Public Domain
