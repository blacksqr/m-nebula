# Medical graphic engine (also still be a game engine) used in therapeutic system. #

### The modification of Nebula1 game engine, originated by Radon Labs GmbH and continued and modified by InSightec Inc. Engine is currently in use in medical therapeutic systems produced by InSightec. ###

The features :
  * Robust C++ game engine
  * Script support: TCL and Python
  * Wrapped API's for files, script, IPC, graphic entities etc.
  * OpenGL and GLSL support
  * OpenSource


The improvements provided by Insightec :
  * Kernel:
    1. IPC - general improvements and file transfer protocol
    1. FileServer - Memory file server added (allows to work with memory using nFileServer2 interface).
  * Engine:
    1. Shading Programs/Language general wrapper
    1. Selection general wrapper using render buffer or raypick
    1. Multiwindow and multiviewport general wrapper
    1. Font and text - TrueType, size and positioning
    1. 3D textures general wrapper
    1. Application template
  * Graphic (GLServer):
    1. GLSL
    1. Antialiasing
    1. Font and text implemetation by FTGL
    1. 3D textures implementation
    1. Multiwindow and multiviewport implementation
