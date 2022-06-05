# Macro (expand upon)
- [ ] Physics (Find library, integrate)
- [ ] C# interop
- [ ] Interpreted interop?
- [ ] Audio handling

# Input
- [ ] [[Notes#Processors|Implement binding processors]]
- [ ] [[Notes#^477310|Store input bounds data in Master Bindings]]
- [ ] [[Notes#Action Maps]]
- [ ] [[Notes#^567d10|Binding Instance control schemes (Keyboard, Controller, etc.)]]
-  Composite Bindings
	- [x] Implementation
	- [ ] Event support
	- [ ] Constituent Absolute Signs (Option)
- [ ] [[Notes#^aa15e4|Gamepad bindings (Joystick / Controller)]]
- [ ] (De)serialization from data format
- [ ] Multi-window input handling
- [ ] Develop input manager GUI utility (Code generation?)

# Application
- [ ] Multi-Window support
- [ ] Determine Linux/OSX compatilities

# Code Health  / Configuration
- [x] Remove CCX::Event string handles
	- [x] Remove all references to string handles
- [ ] Add thorough XML code documentation
- [ ] Add / swap to CMake configuration
- [ ] Further modularise engine

# Graphics
- [ ] Automatic runtime shader compilation
	- [ ] Runtime auto re-compilation
- [ ] Texture support
- [ ] Post processing
	- [ ] Bloom
	- [ ] Anti-Aliasing
	- [ ] Tonemapping
- [ ] Multi-Camera support
- [ ] Runtime modification / creation of meshes

## Lighting
- [ ] Phong
	- [ ] Diffuse
	- [ ] Specular
- [ ] Lights
	- [ ] Directional
	- [ ] Spot
	- [ ] Point
- [ ] Basic Shadow Mapping
	- [ ] Directional
	- [ ] Spot
	- [ ] Point
- [ ] [Improved Shadows](https://docs.microsoft.com/en-us/windows/win32/dxtecharts/common-techniques-to-improve-shadow-depth-maps)
	- [ ] Directional
		- [ ] [Cascading shadow maps](https://docs.microsoft.com/en-us/windows/win32/dxtecharts/cascaded-shadow-maps)
		- [ ] Tight fitting light frustrum
		- [ ] Texel incremented movement
		- [ ] Slope-scale depth bias

# UI
- [ ] Rudimentary Chromium Embedded Framework (CEF) integration
- [ ] GPU accelerated OSR
- [ ] API (expand)
- [x] Integrate IMGUI for debug

# Networking
- [ ] UDP
	- [x] Functional implementation
	- [ ] Packet split on MTU
		- [x] MTU Discovery
		- [ ] API to assign MTU (modify buffer size)
	- [ ] Reliable non-sequenced transfer
- [ ] TCP
	- [x] Functional implementation
	- [ ] TCP MTU Discovery
	- [ ] Refactor source
- [ ] Improved error handling and logging
- [ ] High level 'bootstrap' wrapper

# Misc
- [ ] Add on-demand world transform validation
	- [ ] Add parent and Transform `flecs::ref` members to WorldTransform
- [ ] Configurable logging levels (ConsoleLogging.h)

# Documentation
- [ ] Finish Input manual
	- [ ] Resolve hanging links
- [ ] Generate / write technical API docs