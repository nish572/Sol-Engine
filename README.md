# Sol-Engine - 2D Game Engine and Editor

## Description

A free and open source 2D Game Engine and Editor created with C++ for my BSc Computer Science dissertation. This project could more aptly be described as an auxiliary game development tool for the purposes of learning game development, learning game engine development, and for rapid prototyping.

The codebase is thoroughly commented so please feel free to also use this project as an aid or basis if you are developing your own game engine. Building a game engine is a thoroughly rewarding process, one that I recommend undertaking since it has given me a plethora of skills and knowledge.

---

![main](https://github.com/nish572/Sol-Engine/assets/78385332/286966f4-1393-4bf2-8946-dd9643f65222)

The custom-built Editor (via Dear ImGui) with the visible playtest scene window.

Featuring Hubert The Dog in the test scene shown, who is the beloved mascot of this project conceived during testing.

---

## To Run Sol
**Instructions for running the v1.0 release:**
- Download the latest (v1.0) release from the sidebar.
- Open the directory and find the Sol Engine.exe.
- Run this file to run Sol.
- Note: please ensure SDL2.dll and Engine.dll are kept present in the same directory (alternatively install to PATH, however for ease of use it is ideal to keep these .dll files in the same directory as the executable). Additionally, ensure the imgui configuration file is present in this same directory to allow the Editor to load with the same layout that I have set.

**Notes for running via the Visual Studio solution debugger:**
- Sol was developed using Visual Studio 2022. If you wish to run Sol via the Visual Studio solution debugger, please read the following notes and instructions:
  - Load the solution by opening the .sln file in the root directory.
  - Note that the specific location of the Sol-Engine root directory will vary depending on where you download this codebase to on your system.
  - Upon running the Visual Studio solution debugger, an error will be generated indicating SDL2.dll is missing. This is normal, as SDL2.dll must be copied into the appropriate 'bin' directory.
  - Note that this 'bin' directory will only appear once the Visual Studio solution debugger has been run once for that configuration.
  - SDL2.dll must be copied and pasted (provided in the root directory for convenience, or available in the release build v1.0 from the sidebar) directly into the appropriate directory (or installed to PATH) as outlined below:
  - If the codebase .zip has been downloaded from the master branch: 
    - For running the Visual Studio solution for the Debug-x64 configuration, this SDL2.dll should be pasted inside the following directory: "\Sol-Engine-master\Sol-Engine-master\bin\Debug-x64\Editor".
    - For running the Visual Studio solution for the Release-x64 configuration, this SDL2.dll should be pasted inside the following directory: "\Sol-Engine-master\Sol-Engine-master\bin\Release-x64\Editor".
  - If the codebase .zip has been downloaded from the dev branch:
    - For running the Visual Studio solution for the Debug-x64 configuration, this SDL2.dll should be pasted inside the following directory: "\Sol-Engine-dev\Sol-Engine-dev\bin\Debug-x64\Editor".
    - For running the Visual Studio solution for the Release-x64 configuration, this SDL2.dll should be pasted inside the following directory: "\Sol-Engine-dev\Sol-Engine-dev\bin\Release-x64\Editor".

**Once inside the Editor:**
- Upon creating a new project, please create a new scene via the Toolbar. If not, changes cannot be saved until a new scene has been created and saved.
- Upon loading an existing project, please set load a scene via the Toolbar, or create a new scene via the Toolbar. If not, changes cannot be saved as no scene will be present.
- To close the application, please first select the playtest scene window to make this the active window, then click the cross button in the top-right.

---

## Example Projects
The Version 1.0 release build includes a directory titled "Example Projects".

### 'Platformer' Example Project
![platformer](https://github.com/nish572/Sol-Engine/assets/78385332/df00a506-3f0e-4fba-8d3e-ff8a127d9d8e)
### 'Space' Example Project
![space](https://github.com/nish572/Sol-Engine/assets/78385332/a3834d9e-7218-43e2-b9df-de4039c29e0f)


**To load and play each of these, follow the steps below:**
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
- Scene-specific user controls for playing are detailed in the README included inside each Example Project's Assets folder.

---
### Core-Element Architecture
Sol exists as two projects, the Engine and the Editor. The Editor is a project built via the Sol Core-Element architecture, which is an architecture I developed to allow a central Core manager with Elements that are, in essence, plugins.  If you wish to create further Elements as plugins, or applications with the Sol Core-Element architecture, boilerplate code has been provided as templates for your use.

---

### Co-ordinate System and Units
Box2D used to provide physics simulations presents units in the meters-kilograms-seconds convention.
Rendering is configured to use an orthogonal projection matrix. This presents units in a more user-friendly format as pixels.
The scaling factor applied is on-screen 100 pixels represents 1 meter in the physics world.

The origin of (0,0) has been configured to be the center of the scene playtest viewport, and presents a co-ordinate system as follows:
- A negative X-value is left of the origin.
- A positive X-value is right of the origin.
- A negative Y-value is down from the origin.
- A positive Y-value is up from the origin.

The gravity of the physics world is set to Earth's gravity, set internally as -9.81.

Transform Components use pixels for position, and degrees for rotation (with negative and positive representing the directions clockwise and anti-clockwise respectively).

Sprite Components use pixels for dimension.

Collider Components align with Box2D meters-kilograms-seconds convention.

Input Components use Newtons for the force magnitude. Angular magnitude internally uses Newtons if the force type is angular impulse, but Newton-meters if the force type is set to torque. It is best to not consider these units for the Input Components and instead playtest by trial and error to determine appropriate values.

---

### Scenes
When creating a new scene, or loading a scene, the file extension is .scn. This is automatically done when creating a new scene so please only enter the file name and not the extension too.

Scene files are essentially a JSON formatted storage of the ECS's entity-component map.

---
### Fixed Timestep vs Variable Delta Timestep
Fixed Timestep: Represents the timestep to be used for Systems that should not be updated with a variable delta timestep, and instead should have a fixed timestep. Using a variable delta timestep for Physics System or Events System would produce erratic physics simulations, inaccurate collision detection/resolution, and inaccurate event detection/resolution.

Variable Delta Timestep: This timestep is frame-dependent and will vary as the refresh rate varies (with factors such as VSYNC and hardware limitations affecting this value also). The refresh rate varies to ensure smooth rendering on the screen.

---
### Custom Entity-Component-System
Sol operates using a custom-built ECS (emplyoying a Data-Oriented approach), with further implementation details outlined in the following two sections (Entities, and Components).

This ECS has three custom-built Systems:
- Rendering via an OpenGL 3.3 custom-built rendering engine (with SDL2 for window management and OpenGL context creation)
- Physics via the Box2D physics engine
- Events (as-of-current, primarily keyboard inputs) via SDL2 keyboard/mouse polling

---
### Entities
Entities in Sol are merely numerical identifiers to which components can be associated.

To add an entity to the scene, click 'Add Entity' from the Scene viewport. Adding an entity adds a sprite component and a transform component by default. To remove these, add other components, or delete an entity, simply right-click the entity entry in the Scene viewport to view a context menu.

---

### Components
Components in Sol are merely data structures to be processed by the Systems of the Entity-Component-System backend.

To add a component to an entity, right-click the entity entry in the Scene viewport, and select an option from the provided context menu. To modify the properties of a component, select the entity and then select the component you wish to modify. This will show the specific component in the Inspector viewport where you can modify the properties directly.

The following components are provided to add to an entity to give it various data (and by extension, logic):
- **Transform**
  - This component is responsible for holding the transformation data of an entity, i.e. its positional, rotational, and scalar data.
  - This allows physics to move entities (provided a physics body component and a collider component are also attached to the entity), and allows rendering to render with the appropriate transform data.
  - User-modifiable X and Y position, the X and Y scale (please see the note below), and the rotation.
  - Please note that setting the Scale X and Scale Y values is not recommended. To modify an entity's scale, please re-size the sprite associated with it (if a sprite is being used) and re-size the collider associated with it (if a collider is being used, and this will in turn amend the size of the physics body if a physics body is being used, which it should be if a collider is being used).
- **Sprite**
  - This component is responsible for representing an image to be rendered.
  - User-modifiable properties include the texture file path (for ease-of-use images can be dragged from the resource browser and dropped into this input field to automatically enter them), and the sprite dimensions.
- **Physics Body**
  - This component is responsible for defining the body type to be set for physics.
  - User-modifiable properties include the body type (Kinematic which are bodies unaffected by gravity, Dynamic which are bodies affected by gravity, and Static which are bodies that can be collided with if the entity is given a collider component but will not be affected by gravity or physics simulations). Additionally, the user can toggle whether an entity with this body can be rotated by physics or not, which is useful if creating something like a platformer player that shouldn't rotate off of platforms for example.
- **Collider**
  - This component is responsible for holding the specific data for the physics body's collider, and is used to help define the physics body for physics simulations.
  - User-modifiable properties include the shape of the collider (Box, or Circle), as well as the dimensions (width and height for a Box, or radius for a Circle) and the density/friction/restitution.
  - Additionally there is a visibility toggle to allow the collision shape to be rendered as a wireframe in the playtest scene window, which is useful as a collision box editor.
  - Please note that setting the dimensions affects the mass of a physics body (which affects how much forces/impulses/torques affect the body), but the density can be modified to counter or enhance this. Default values for friction and restitution are provided but may be modified if the user wishes differing effects.
  - Box2D leaves a very small gap around collision shapes for internal use to detect collision. This is intended behaviour by Box2D, however it may be visually beneficial to reduce the collision shape by a small amount, e.g. setting a box to 0.95m x 0.95m as opposed to 1m x 1m, to reduce/remove the visibility of this gap. This can be experimented with by the user to adjust the gap appropriately until no gap is visible.
- **Input**
  - This component allows the user to add actions based upon key presses. This allows control of an entity when a scene is running in the playtest window by applying forces/impulses/torques to the physics body (if a physics body component and a collider are being used).
  - Upon adding an action, keyboard input is polled so the user can press a key to associate with this action.
  - User-modifiable properties include setting the key associated with an action, setting the force type (outlined in more detail below), setting the input type (currently only Keyboard is implemented, please see known issues), move direction, magnitude, and angular magnitude.
  - There are four force types: force is a linear force applied to the center of mass; impulse is essentially a sudden application of force (still linear, applied to the center of mass); torque (rotational force applied with the origin of rotation being the center of mass); angular impulse (essentially a sudden torque, applied with the origin of rotation being the center of mass).
  - Move direction only applies to actions with force type set to force or impulse, and is an (X,Y) value such that -1, 0, 1 for the X-value represent left, none, or right in the X-axis, and -1, 0, 1 for the Y-value represent down, none, or up in the Y-axis.
  - Magnitude is only used if the force type is set to force or impulse and is the amount of force or impulse to apply.
  - Angular magnitude is only used if the force type is set to torque or angular impulse and is the amount of torque or angular impulse to apply, with a value such that negative and positive represent the directions clockwise and anti-clockwise respectively.

---

### Project Settings

By selecting Project in the Toolbar then Project Settings you can set the project's name and filepath, as well as the playtest scene window's dimensions.

![project settings](https://github.com/nish572/Sol-Engine/assets/78385332/f22863e8-613a-445f-bbe7-e13270288c33)

---

### Known Issues
- Certain images may load either in black/white, or not load at all. However, this appears to be an issue with the STB image loading library and appears to only affect loading images imported from Apple phones. I believe this issue arises due to the manner in which Apple (and certain other devices) constructs the metadata of images from their platform. Please be careful when choosing images to load, and should this error present an issue, simply delete the affecting image, reload the Editor, and use a different image.
- PNG blending issues. Images with transparent backgrounds load fine, with a transparent background. However, upon displaying them over other images, the background of the PNG turns black. This issue will be resolved in the future by introducing manual image layer setting to allow images to be layered more appropriately. OpenGL blending and depth testing has been enabled, and this allows PNGs to appear with a transparent background, but only when the background is not displayed over another image. This issue arises from the Painter's Algorithm, and will be rectified by introducing appropriate image layering functionality.
- Transform Component scalar value modification leads to faulty physics. If you wish to resize an Entity on the screen, please directly modify the size values within the Sprite Component in the Inspector viewport, and then directly modify the size values within the Collider Component (width/height for a Box collider, radius for a square collider) in the Inspector viewport.
- MouseMovement input types currently are not fully implemented and as such have no use despite being present in the Editor.

---

### Further Work
- The introduction of a 'max force/impulse/torque' would enable forces/impulses/torques to be applied until a maximum value is reached. Additionally, a 'key press limiter' would enable the resultant action of a key press to only be applied under certain circumstances, e.g. if the player is on the ground the key can be pressed to jump otherwise this action will not be executed.
- Layer introduction to allow determination of which physics interact (i.e. physics bodies can be set to operate on specific layers to prevent collisions with physics from other layers), and to allow determination of which order sprites should be rendered on the screen (i.e. sprites with a lower layer will be rendered behind sprites with a higher layer).
- Audio Element and Audio System implementation.
- Full completion of the shader system to bring this functionality to the Editor. Currently mostly exists on the backend and allows shader creation from two separate shader files (one for a vertex shader, one for a fragment shader), however as of present rendering uses the default shader programs created.
- Introduce a way to name entities on the frontend. Currently this isn't implemented due to entities existing merely as an identifier integer, however it would be ideal to introduced naming on the frontend only for ease-of-use.

---

## External Libraries

- **spdlog**
  - **Usage**: To build the Log Element to provide solution-wide logging functionality to a .txt file for debugging purposes.
  - **Found at**: [spdlog](https://github.com/gabime/spdlog)
  - **License**: MIT
- **OpenGL 3.3**
  - **Usage**: To interact with the GPU to achieve hardware-accelerated rendering.
  - **Information found at**: [OpenGL 3.3](https://www.opengl.org/)
  - **License**: Not Applicable
- **GLAD**
  - **Usage**: To simplify the process of managing OpenGL function pointers, as a loader-generator, a utility which The Khronos Group instructs developers to make use of in order to use OpenGL.
  - **Found at**: [GLAD](https://glad.dav1d.de/)
  - **License**: The GLAD loader-generator itself is under the MIT license; however, the code it generates is under the Apache License.
- **SDL2**
  - **Usage**: To manage the window upon which an OpenGL context can be used for rendering. Additionally, to poll keyboard and mouse input.
  - **Found at**: [SDL2](https://www.libsdl.org/)
  - **License**: zlib
- **GLM**
  - **Usage**: For matrix calculations for transformation data representation.
  - **Found at**: [GLM](https://www.opengl.org/sdk/libs/GLM/)
  - **License**: MIT
- **Box2D**
  - **Usage**: As the physics engine powering the physics of this game engine. Specifically used in the Physics System of the ECS to provide a physics world for collision detection and resolution as well as physics simulations.
  - **Found at**: [Box2D](https://github.com/erincatto/box2d)
  - **License**: MIT
- **Dear ImGui**
  - **Usage**: To build the Graphical User Interface of the Editor.
  - **Found at**: [Dear ImGui](https://github.com/ocornut/imgui)
  - **License**: MIT
- **JSON for Modern C++**
  - **Usage**: For scene serialization and deserialization. Specifically used by the Scene Element to load and unload scenes by deserializing and serializing the ECS’s main data structure of Entity-Components mappings into .scn files which are essentially JSON files.
  - **Found at**: [JSON for Modern C++](https://github.com/nlohmann/json)
  - **License**: MIT
- **STB image loader (stb_image.h)**
  - **Usage**: To load images into a texture format usable by the Render System.
  - **Found at**: [STB image loader](https://github.com/nothings/stb/blob/master/stb_image.h)
  - **License**: Both MIT and Public Domain
  
---
