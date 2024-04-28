# Sol-Engine

## Description

A 2D Game Engine and Editor created for my BSc Computer Science dissertation. This project could more aptly be described as an auxilliary game development tool for the purposes of learning game development, learning game engine development, and for rapid prototyping.

---

## To Run Sol
To run Sol, please download the latest release from the sidebar. Open the directory and find the Sol Engine.exe. Run this file to run Sol. Please ensure SDL2.dll and Engine.dll are present in the same directory (alternatively install to PATH, however for ease of use it is ideal to keep these .dll files in the same directory as the executable). Additionally, ensure the imgui configuration file is present in this same directory to allow the Editor to load with the same visual settings as I have set.

If running Sol in the Visual Studio solution debugger, please ensure SDL2.dll is copied and pasted directly into the appropriate directory (or installed to PATH) outlined below.
- For running the Visual Studio solution for the Debug-x64 configuration, this SDL2.dll should be pasted inside the following directory: "\Sol-Engine\Sol-Engine\bin\Debug-x64\Editor".
- For running the Visual Studio solution for the Release-x64 configuration, this SDL2.dll should be pasted inside the following directory: "\Sol-Engine\Sol-Engine\bin\Release-x64\Editor".
(Please note that the specific location of the Sol-Engine root directory will vary depending on where you download this codebase to on your system.)

---

## Example Projects
The Version 1.0 release build includes a directory titled "Example Projects".

![platformer](https://github.com/nish572/Sol-Engine/assets/78385332/df00a506-3f0e-4fba-8d3e-ff8a127d9d8e)
![space](https://github.com/nish572/Sol-Engine/assets/78385332/a3834d9e-7218-43e2-b9df-de4039c29e0f)


To load each of these, follow these steps:
- Run the Sol Engine.exe application executable.
- When greeted with the StartUp Dialog popup, select Load Existing Project.
- To run the Platformer example project, enter the path as follows: "Example Projects\Platformer"
- Alternatively, to run the Space example project, enter the path as follows: "Example Projects\Space"
- (Please note that the inclusion of quotation marks does not affect loading the project, these can be omitted if you wish.)
- In the Toolbar select 'Scene', then 'Load Scene'.
- If running the Platformer example project, select 'Main Platformer.scn', then click 'Yes, Load' in the confirmation popup.
- If running the Space example project, select 'Asteroid Field.scn', then click 'Yes, Load' in the confirmation popup.
- Then once you see the loaded scene in the playtest window, click 'Cancel' in the Load Existing Scene popup to exit back to the Editor.
- To playtest this scene, click 'Start' from the Toolbar.
- Scene-specific user controls for playing are detailed in the README included inside each Example Project's Asset's folder.

---

### Known Issues
- Certain images may load either in black/white, or not load at all. However, this appears to be an issue with the STB image loading library and appears to only affect loading images imported from Apple phones. I believe this issue arises due to the manner in which Apple (and certain other devices) constructs the metadata of images from their platform. Please be careful when choosing images to load, and should this error present an issue, simply delete the affecting image, reload the Editor, and use a different image.
- PNG blending issues. Images with transparent backgrounds load fine, with a transparent background. However, upon displaying them over other images, the background of the PNG turns black. This issue will be resolved in the future by introducing manual image layer setting to allow images to be layered more appropriately. OpenGL blending and depth testing has been enabled, and this allow PNGs to appear with a transparent background, but only when the background is not displayed over another image. This issue arises from the Painter's Algorithm, and will be rectified by introducing appropriate image layering functionality.
- Transform Component scalar value modification leads to faulty physics. If you wish to resize an Entity on the screen, please directly modify the size values within the Sprite Component in the Inspector viewport, and  then directly modify the size values within the Collider Component (width/height for a Box collider, radius for a square collider) in the Inspector viewport.

---

### Further Work
- The introduction of a 'max force/impulse/torque' would enable forces/impulses/torques to be applied until a maximum value is reached. Additionally, a 'key press limiter' would enable the resultant action of a key press to only be applied under certain circumstances, e.g. if the player is on the ground the key can be pressed to jump otherwise this action will not be executed.
- Layer introduction to allow determination of which physics interact (i.e. physics bodies can be set to operate on specific layers to prevent collisions with physics from other layers), and to allow determination of which order sprites should be rendered on the screen (i.e. sprites with a lower layer will be rendered behind sprites with a higher layer).
- Audio Element and Audio System implementation.
- Full completion of the shader system to bring this functionality to the Editor. Currently mostly exists onthe backend and allows shader creation from two separate shader files (one for a vertex shader, one for a fragment shader), however as of present, rendering uses the default shader programs created.

---

## External Libraries

