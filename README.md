## DirectX 11 Rendering Engine
> A 3D rendering engine built from scratch in C++ and DirectX 11 — no engine abstraction, raw hardware interface.
---
### Overview
Built for GDENG03 (Game Engine Development) at De La Salle University - Laguna Campus. The course covered the full pipeline of building a game engine from the ground up: from rendering a first triangle to implementing an Entity Component System, scene management, and a live editor UI.
This repository is my personal implementation, developed incrementally across the full course. A separate group variant (linked below) includes collaborative contributions, including fog and shader systems.

---
### Author
**Sydrenz Anthony P. Cao**

---
### How to Run
This project is built and run using Visual Studio 2022.
1. Open `GDENG03_DirectX3D_Triangle.sln` in Visual Studio 2022
2. Ensure the startup project is set to `GDENG03_DirectXGame`
3. Press F5 or click the green "Local Windows Debugger" button to build and run

**Entry Point:** `Game/main.cpp`

---
### Controls
| **Input** | **Action** |
|-----------|------------|
| Right-click + Mouse Move | Look around (camera rotation) |
| Right-click + W/A/S/D | Move camera forward/left/backward/right |
 
---
 
## Systems Implemented
 
The engine was built incrementally across the following modules:
 
### Rendering Pipeline
- DirectX 11 device, swap chain, render target, and depth stencil setup from scratch
- Rendering primitives: triangles → quads → multiple objects → depth-correct 3D objects
- 3D model loading and rendering
- Texturing and shader library
 
### Engine Time & Animation
- Engine time system controlling delta time and frame-independent animation
- Animation loop driving object transforms over time
 
### Camera & Input System
- Freefly camera with right-click-to-activate mouse look
- WASD movement relative to camera orientation
- Raw DirectX input handling — no engine input abstraction
 
### Editor UI (IMGUI)
- In-engine editor UI implemented via IMGUI
- UI Manager with multiple screens/panels
- Live scene inspection and control
 
### Entity Component System
- Component-based architecture for scene objects
- Rigid body components
- Component system management and lifecycle
 
### Scene Management
- Engine state machine for managing application lifecycle
- Scene saving and loading
 
### Group Variant Contributions
- **Atmospheric fog** — distance-based fog implemented in HLSL pixel shader
- **Shader system architecture** — shader library structure for the collaborative engine build
 
---
 
## Repository Links
 
| Variant | Link |
|---|---|
| Personal implementation | [GDENG03_DirectX3D_Triangle](https://github.com/loldope19/GDENG03_DirectX3D_Triangle) |
| Group implementation | [GDENG03-Engine](https://github.com/FurehXD/GDENG03-Engine) |
 
---
 
## Course Context
 
| Field | Detail |
|---|---|
| Course | GDENG03 — Game Engine Development |
| Institution | De La Salle University Laguna Campus |
| Period | May – July 2025 |
| Language | C++ |
| Graphics API | DirectX 11 |
| UI Library | IMGUI |
 
---
 
*Built as a learning exercise in low-level graphics and engine architecture. Every system was implemented without engine abstraction to develop a ground-up understanding of how game engines work.*
