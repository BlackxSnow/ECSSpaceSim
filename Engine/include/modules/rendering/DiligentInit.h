#pragma once

#ifndef ENGINE_DLL
#    define ENGINE_DLL 1
#endif

#ifndef D3D11_SUPPORTED
#    define D3D11_SUPPORTED 0
#endif

#ifndef D3D12_SUPPORTED
#    define D3D12_SUPPORTED 0
#endif

#ifndef GL_SUPPORTED
#    define GL_SUPPORTED 0
#endif

#ifndef VULKAN_SUPPORTED
#    define VULKAN_SUPPORTED 0
#endif

#ifndef METAL_SUPPORTED
#    define METAL_SUPPORTED 0
#endif

#include <GLFW/glfw3.h>
#include <Graphics/GraphicsEngine/interface/RenderDevice.h>

void InitialiseDiligent(GLFWwindow* window, uint32_t windowWidth, uint32_t windowHeight, Diligent::RENDER_DEVICE_TYPE renderDeviceType);