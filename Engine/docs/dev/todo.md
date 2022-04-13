# Macro (expand upon)
- [ ] Physics (Find library, integrate)
- [ ] Networking
- [ ] C# interop
- [ ] Interpreted interop?
- [ ] Audio handling
- [ ] Electron (?) UI integration

# Input
- [ ] [[Notes#Processors|Implement binding processors]]
- [ ] [[Notes#^477310|Store input bounds data in Master Bindings]]
- [ ] [[Notes#Action Maps]]
- [ ] [[Notes#Composite Bindings]]
- [ ] [[Notes#^aa15e4|Gamepad bindings (Joystick / Controller)]]
- [ ] (De)serialization from data format
- [ ] Multi-window input handling
- [ ] Develop input manager utility (Code generation?)

# Application
- [ ] Multi-Window support
- [ ] Determine Linux/OSX compatilities

# Code Health
- [ ] Remove CCX::Event string handles
	- [ ] Remove all references to string handles
- [ ] Add thorough XML code documentation

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

# Misc
- [ ] Add on-demand world transform validation
	- [ ] Add parent and Transform `flecs::ref` members to WorldTransform

# Documentation
- [ ] Finish Input manual
	- [ ] Resolve hanging links
- [ ] Generate / write technical API docs