//#include <utility/CCXShader.h>
//#include <utility/CCXFile.h>
//#include <utility/ConsoleLogging.h>
//
//const std::filesystem::path _ShaderPath("Resources/Shaders");
//
//bgfx::ShaderHandle CCX::LoadShaderComponent(bx::FileReaderI* reader, std::filesystem::path& path)
//{
//	std::filesystem::path fullPath = _ShaderPath / "bin";
//
//	switch (bgfx::getRendererType())
//	{
//		case bgfx::RendererType::Noop:
//		case bgfx::RendererType::Direct3D9:  fullPath /= "dx9";   break;
//		case bgfx::RendererType::Direct3D11:
//		case bgfx::RendererType::Direct3D12: fullPath /= "s_5_0";  break;
//		case bgfx::RendererType::Agc:
//		case bgfx::RendererType::Gnm:        fullPath /= "pssl";  break;
//		case bgfx::RendererType::Metal:      fullPath /= "metal"; break;
//		case bgfx::RendererType::Nvn:        fullPath /= "nvn";   break;
//		case bgfx::RendererType::OpenGL:     fullPath /= "glsl";  break;
//		case bgfx::RendererType::OpenGLES:   fullPath /= "essl";  break;
//		case bgfx::RendererType::Vulkan:     fullPath /= "spirv"; break;
//		case bgfx::RendererType::WebGPU:     fullPath /= "spirv"; break;
//
//		case bgfx::RendererType::Count:
//			BX_ASSERT(false, "You should not be here!");
//			break;
//	}
//
//	fullPath /= path.replace_extension(".bin");
//	LogInfo(fullPath.string());
//	const bgfx::Memory* shaderMem = LoadMemory(reader, fullPath.string());
//
//	if (shaderMem == nullptr)
//	{
//		LogError("Failed to load shader at " + path.string(), true);
//	}
//
//	return bgfx::createShader(shaderMem);
//}
//
//bgfx::ProgramHandle CCX::LoadShaderProgram(std::filesystem::path&& vertexShaderPath, std::filesystem::path&& fragShaderPath)
//{
//	bx::FileReaderI* reader = GetFileReader();
//    auto vs = LoadShaderComponent(reader, vertexShaderPath);
//    auto fs = LoadShaderComponent(reader, fragShaderPath);
//
//	return bgfx::createProgram(vs, fs, true);
//}
