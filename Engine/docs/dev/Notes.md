## Input
### Actions / Bindings
- Implement control scheme settings on Binding Instances ^567d10
	- Allows for choosing binding to poll based on last active scheme
- Gamepad bindings in GLFW for controllers and joysticks should make these inputs easier
- 
### Processors
- Base class that defines `T Process(T data)` for each output type.
- Base implementation of `Process` directly returns data (default behaviour)
- Overrides used to create useful processors
- Store and send binding info to increase modularity or processors? ^477310
	- Expected bounds of input device's data?

### InputEventData
- Hold a stack allocated float[6] to represent data?

### Action Maps
- Include priorities and overriding based on that
	- Useful for things like allowing limited gameplay while in a UI?

### Utility
- `InputDevice GetDeviceType(int bindingID)` to convert bindings to legible device types?

---
## Graphics
### Lighting
- Need a way to do shadow calculations for extreme range lights (sun)
	- [Calculation of spacecraft Umbra and penumbra](https://ntrs.nasa.gov/api/citations/19950023025/downloads/19950023025.pdf)
		- May be more relevant as application feature, possible addon to ECSE
- Need distance-limited directional lights and/or infinite distance point lights
	- [Percentage Closer Soft Shadows to emulate light size](https://http.download.nvidia.com/developer/presentations/2005/SIGGRAPH/Percentage_Closer_Soft_Shadows.pdf)