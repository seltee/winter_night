# Winter Night Engine

My personal Vulkan 3d/2d engine I'm constructing using C++ 2020 

---

### Features

- Forward+ rendering
- MSAA antialiasing
- 3d Model (fbx) and texture (any major format) loading
- Windows and Linux support
- Gamepads support
- Perspective and Orthographic cameras
- Rotation using quaternions
- Directional, Spot and omni lights
- Directional light shadows
- Sprites
- Animations and bones

# Compilation

## Windows

- Install Msys2. In Msys2 install gcc and make
- Clone shaderc and compile it into a static library
- Clone the repo and use make under Msys2 terminal

## Linux

- Make sure gcc, git and make tools are installed
- Clone shaderc and compile it into static library (check paths for linux in makefile)
- Clone the repo and run make from the terminal

---

### How engine is being developed

Pretty much randomly I add features that I need for future projects