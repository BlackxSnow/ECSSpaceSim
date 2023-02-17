#include <modules/rendering/DiligentInit.h>

#if D3D11_SUPPORTED
#    include "Graphics/GraphicsEngineD3D11/interface/EngineFactoryD3D11.h"
#endif
#if D3D12_SUPPORTED
#    include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"
#endif
#if GL_SUPPORTED
#    include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#endif
#if VULKAN_SUPPORTED
#    include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"
#endif
#if METAL_SUPPORTED
#    include "Graphics/GraphicsEngineMetal/interface/EngineFactoryMtl.h"
#endif

#if PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif PLATFORM_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#elif PLATFORM_MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <GLFW/glfw3native.h>

#include <utility/ConsoleLogging.h>

#if PLATFORM_MACOS
Diligent::MacOSNativeWindow GetNSWindowView(GLFWwindow* glfwWindow)
{
    id window = glfwGetCocoaWindow(glfwWindow);
    id view = [window contentView];

    Diligent::NSBundle* bundle = [NSBundle bundleWithPath:@"/System/Library/Frameworks/QuartzCore.framework"];
    if (!bundle) return nullptr;

    id layer = [[bundle classNamed:@"CAMetalLayer"] layer];
    if (!layer) return nullptr;

    [view setLayer:layer];
    [view setWantsLayer:YES];

    return view;
}
#endif

Diligent::IRenderDevice* _RenderDevice;
Diligent::IDeviceContext* _DeviceContext;
Diligent::ISwapChain* _SwapChain;

void InitialiseDiligent(GLFWwindow* glfwWindow, uint32_t windowWidth, uint32_t windowHeight, Diligent::RENDER_DEVICE_TYPE renderDeviceType)
{
#if PLATFORM_LINUX
    Diligent::LinuxNativeWindow nativeWindow;
    nativeWindow.WindowId = glfwGetX11Window(glfwWindow);
    nativeWindow.pDisplay = glfwGetX11Display();
    if (renderDeviceType == Diligent::RENDER_DEVICE_TYPE_GL)
    {
        glfwMakeContextCurrent(glfwWindow);
    }
#endif
#if PLATFORM_MACOS
    Diligent::MacOSNativeWindow nativeWindow{GetNSWindowView(glfwWindow)};
#endif
#if PLATFORM_WIN32
    Diligent::Win32NativeWindow nativeWindow{glfwGetWin32Window(glfwWindow)};
#endif
    Diligent::SwapChainDesc swapDesc;
    switch(renderDeviceType)
    {
#if D3D11_SUPPORTED
        case Diligent::RENDER_DEVICE_TYPE_D3D11:
        {
#if ENGINE_DLL
            auto* GetEngineFactoryD3D11 = Diligent::LoadGraphicsEngineD3D11();
#endif
            auto* pFactoryD3D11 = Diligent::GetEngineFactoryD3D11();
            Diligent::EngineD3D11CreateInfo engineCI;
            pFactoryD3D11->CreateDeviceAndContextsD3D11(engineCI, &_RenderDevice, &_DeviceContext);
            pFactoryD3D11->CreateSwapChainD3D11(_RenderDevice, _DeviceContext, swapDesc, FullScreenModeDesc{}, nativeWindow, &_SwapChain);
        }
        break;
#endif
#if D3D12_SUPPORTED
        case Diligent::RENDER_DEVICE_TYPE_D3D11:
        {
#if ENGINE_DLL
            auto* GetEngineFactoryD3D11 = Diligent::LoadGraphicsEngineD3D11();
#endif
            auto* pFactoryD3D12 = Diligent::GetEngineFactoryD3D12();
            Diligent::EngineD3D12CreateInfo engineCI;
            pFactoryD3D12->CreateDeviceAndContextsD3D12(engineCI, &_RenderDevice, &_DeviceContext);
            pFactoryD3D12->CreateSwapChainD3D12(_RenderDevice, _DeviceContext, swapDesc, FullScreenModeDesc{}, nativeWindow, &_SwapChain);
        }
        break;
#endif
#if GL_SUPPORTED
        case Diligent::RENDER_DEVICE_TYPE_GL:
        {
#if EXPLICITLY_LOAD_ENGINE_GL_DLL
            auto GetEngineFactoryOpenGL = Diligent::LoadGraphicsEngineOpenGL();
#endif
            auto* pFactoryOpenGL = Diligent::GetEngineFactoryOpenGL();

            Diligent::EngineGLCreateInfo engineCInfo;
            engineCInfo.Window = nativeWindow;
            pFactoryOpenGL->CreateDeviceAndSwapChainGL(engineCInfo, &_RenderDevice, &_DeviceContext, swapDesc, &_SwapChain);
        }
        break;
#endif
#if VULKAN_SUPPORTED
        case Diligent::RENDER_DEVICE_TYPE_VULKAN:
        {
#if EXPLICITLY_LOAD_ENGINE_VK_DLL
            auto* GetEngineFactoryVk = Diligent::LoadGraphicsEngineVk();
#endif
            auto* pFactoryVk = Diligent::GetEngineFactoryVk();

            Diligent::EngineVkCreateInfo engineCInfo;
            pFactoryVk->CreateDeviceAndContextsVk(engineCInfo, &_RenderDevice, &_DeviceContext);
            pFactoryVk->CreateSwapChainVk(_RenderDevice, _DeviceContext, swapDesc, nativeWindow, &_SwapChain);
        }
        break;
#endif
#if METAL_SUPPORTED
        case RENDER_DEVICE_TYPE_METAL:
        {
            auto* pFactoryMtl = Diligent::GetEngineFactoryMtl();

            Diligent::EngineMtlCreateInfo engineCInfo;
            pFactoryMtl->CreateDeviceAndContextsMtl(engineCInfo, &_RenderDevice, &_DeviceContext);
            pFactoryMtl->CreateSwapChainMtl(_RenderDevice, _DeviceContext, swapDesc, nativeWindow, &_SwapChain);
        }
        break;
#endif // METAL_SUPPORTED

        default:
            LogError("Unknown/unsupported device type", true);
            break;
    }
}