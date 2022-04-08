newoption
{
	trigger = "regen-bgfx",
	description = "Regenerate bgfx project files"
}

-- if not os.isdir(path.join(BGFX_DIR, ".build/projects")) or _OPTIONS["regen-bgfx"] then
-- 	os.execute("bgfxGenie.bat " .. _ACTION)
-- end

local libDir = "lib/src"

local BGFX_DIR = path.join(libDir, "bgfx")
local BIMG_DIR = path.join(libDir, "bimg")
local BX_DIR = path.join(libDir, "bx")
local GLFW_DIR = path.join(libDir, "glfw")
local GLM_DIR = path.join(libDir, "glm")
local ASSIMP_DIR = path.join(libDir, "assimp")
local FLECS_DIR = path.join(libDir, "flecs")

solution "ECSSpaceSim"
	startproject "SpaceSim"
	configurations { "Release", "Debug" }
	if os.is64bit() and not os.istarget("windows") then
		platforms "x86_64"
	else
		platforms { "x86_64" }
	end
	filter "configurations:Release"

		defines {"NDEBUG", "BX_CONFIG_DEBUG=0"}
		optimize "Full"
	filter "configurations:Debug*"
		defines {"_DEBUG", "BX_CONFIG_DEBUG=1" }
		optimize "Debug"
		symbols "On"
	filter "platforms:x86"
		architecture "x86"
	filter "platforms:x86_64"
		architecture "x86_64"
	filter "system:macosx"
		xcodebuildsettings {
			["MACOSX_DEPLOYMENT_TARGET"] = "10.9",
			["ALWAYS_SEARCH_USER_PATHS"] = "YES", -- This is the minimum version of macos we'll be able to run on
		};

function setBxCompat()
	filter "action:vs*"
		includedirs { path.join(BX_DIR, "include/compat/msvc") }
	filter { "system:windows", "action:gmake" }
		includedirs { path.join(BX_DIR, "include/compat/mingw") }
	filter { "system:macosx" }
		includedirs { path.join(BX_DIR, "include/compat/osx") }
		buildoptions { "-x objective-c++" }
end

function linkDLLs(values)
    for i, value in ipairs(values) do
        postbuildcommands { "{COPYFILE} \"%{wks.location}/lib/dll/" .. value .. ".dll\" \"%{cfg.buildtarget.directory}/" .. value .. ".dll\"" }
        links { value }
    end
end

project "SpaceSim"
    location "SpaceSim"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    exceptionhandling "Off"
    rtti "Off"
    files { "SpaceSim/**.h", "SpaceSim/**.cpp"}
	dependson { "Engine"}
	staticruntime "off"
	defines { "flecs_STATIC" }
    includedirs
	{
		path.join(BGFX_DIR, "include"),
		path.join(BX_DIR, "include"),
		path.join(GLFW_DIR, "include"),
		path.join(BIMG_DIR, "include"),
        GLM_DIR,
        path.join(ASSIMP_DIR, "include"),
        path.join(ASSIMP_DIR, "build/x64/include/assimp"),
		path.join(FLECS_DIR, "include"),
        "Engine"
	}
    libdirs { "lib" }
    filter "configurations:Debug"
        assimpLibName = "assimp-vc142-mtd"
        linkDLLs({assimpLibName})
    filter "configurations:Release"
        assimpLibName = "assimp-vc142-mt"
        linkDLLs( {assimpLibName})
    filter "*"
    links { "bgfx", "bimg", "bx", "glfw", "Engine", "flecs" }
    postbuildcommands
    {
        "{COPYDIR} \"%{prj.location}/Resources\" \"%{cfg.buildtarget.directory}/Resources\""
    }
    filter "system:windows"
    	links { "gdi32", "kernel32", "psapi" }
    filter "system:linux"
        links { "dl", "GL", "pthread", "X11" }
    filter "system:macosx"
        links { "QuartzCore.framework", "Metal.framework", "Cocoa.framework", "IOKit.framework", "CoreVideo.framework" }
    setBxCompat()

project "Engine"
    location "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	exceptionhandling "Off"
	rtti "Off"
    targetname("%{prj.name}.%{cfg.buildcfg}");
	files { "Engine/**.h", "Engine/**.cpp" }
	staticruntime "off"
	defines { "flecs_STATIC" }
	includedirs
	{
		path.join(BGFX_DIR, "include"),
		path.join(BX_DIR, "include"),
		path.join(GLFW_DIR, "include"),
		path.join(BIMG_DIR, "include"),
        GLM_DIR,
        path.join(ASSIMP_DIR, "include"),
        path.join(ASSIMP_DIR, "build/x64/include/assimp"),
		path.join(FLECS_DIR, "include")
	}
    libdirs { "lib" }
    filter "configurations:Debug"
        assimpLibName = "assimp-vc142-mtd"
        links {assimpLibName}
    filter "configurations:Release"
        assimpLibName = "assimp-vc142-mt"
        links {assimpLibName}
    filter "*"
    links { "bgfx", "bimg", "bx", "glfw", "flecs"}
    -- postbuildcommands 
    -- { 
    --     "{COPYFILE} \"" .. "%{cfg.buildtarget.directory}/%{cfg.buildtarget.name}\" " .. "%{wks.location}" .. "/lib/" .. "%{cfg.buildtarget.name}\""
    -- }

	filter "system:windows"
		links { "gdi32", "kernel32", "psapi" }
	filter "system:linux"
		links { "dl", "GL", "pthread", "X11" }
	filter "system:macosx"
		links { "QuartzCore.framework", "Metal.framework", "Cocoa.framework", "IOKit.framework", "CoreVideo.framework" }
	setBxCompat()
	
filter "action:vs*"
project "EngineTests"
	location "EngineTests"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	files { "EngineTests/**.h", "EngineTests/**.cpp" }
	staticruntime "off"
	libdirs { "$(VCInstallDir)UnitTest/lib" }
	includedirs { "$(VCInstallDir)UnitTest/include" }
	links { "Engine", "kernel32", "user32", "gdi32", "winspool", "comdlg32", "advapi32", "shell32", "ole32", "oleaut32", "uuid", "odbc32", "odbccp32" }

filter "*"

group ("Dependencies")

function external(name, type)
	type = type or "StaticLib"
	externalproject (name)
		location (path.join(BGFX_DIR, ".build/projects", _ACTION))
		uuid (os.uuid(name))
		kind (type)
		language "C++"
end

project "bgfx"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	exceptionhandling "Off"
	rtti "Off"
	defines "__STDC_FORMAT_MACROS"
	files
	{
		path.join(BGFX_DIR, "include/bgfx/**.h"),
		path.join(BGFX_DIR, "src/*.cpp"),
		path.join(BGFX_DIR, "src/*.h"),
	}
	excludes
	{
		path.join(BGFX_DIR, "src/amalgamated.cpp"),
	}
	includedirs
	{
		path.join(BX_DIR, "include"),
		path.join(BIMG_DIR, "include"),
		path.join(BGFX_DIR, "include"),
		path.join(BGFX_DIR, "3rdparty"),
		path.join(BGFX_DIR, "3rdparty/dxsdk/include"),
		path.join(BGFX_DIR, "3rdparty/khronos")
	}
	filter "configurations:Debug"
		defines "BGFX_CONFIG_DEBUG=1"
	filter "action:vs*"
		defines "_CRT_SECURE_NO_WARNINGS"
		excludes
		{
			path.join(BGFX_DIR, "src/glcontext_glx.cpp"),
			path.join(BGFX_DIR, "src/glcontext_egl.cpp")
		}
	filter "system:macosx"
		files
		{
			path.join(BGFX_DIR, "src/*.mm"),
		}
	setBxCompat()

project "bimg"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	exceptionhandling "Off"
	rtti "Off"
	files
	{
		path.join(BIMG_DIR, "include/bimg/*.h"),
		path.join(BIMG_DIR, "src/image.cpp"),
		path.join(BIMG_DIR, "src/image_gnf.cpp"),
		path.join(BIMG_DIR, "src/*.h"),
		path.join(BIMG_DIR, "3rdparty/astc-codec/src/decoder/*.cc")
	}
	includedirs
	{
		path.join(BX_DIR, "include"),
		path.join(BIMG_DIR, "include"),
		path.join(BIMG_DIR, "3rdparty/astc-codec"),
		path.join(BIMG_DIR, "3rdparty/astc-codec/include"),
	}
	setBxCompat()

project "bx"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	exceptionhandling "Off"
	rtti "Off"
	defines "__STDC_FORMAT_MACROS"
	files
	{
		path.join(BX_DIR, "include/bx/*.h"),
		path.join(BX_DIR, "include/bx/inline/*.inl"),
		path.join(BX_DIR, "src/*.cpp")
	}
	excludes
	{
		path.join(BX_DIR, "src/amalgamated.cpp"),
		path.join(BX_DIR, "src/crtnone.cpp")
	}
	includedirs
	{
		path.join(BX_DIR, "3rdparty"),
		path.join(BX_DIR, "include")
	}
	filter "action:vs*"
		defines "_CRT_SECURE_NO_WARNINGS"
	setBxCompat()

project "glfw"
    location(GLFW_DIR)
	kind "StaticLib"
	language "C"
	staticruntime "off"
	files
	{
		path.join(GLFW_DIR, "include/GLFW/*.h"),
		path.join(GLFW_DIR, "src/context.c"),
		path.join(GLFW_DIR, "src/egl_context.*"),
		path.join(GLFW_DIR, "src/init.c"),
		path.join(GLFW_DIR, "src/input.c"),
		path.join(GLFW_DIR, "src/internal.h"),
		path.join(GLFW_DIR, "src/monitor.c"),
		path.join(GLFW_DIR, "src/osmesa_context.*"),
		path.join(GLFW_DIR, "src/vulkan.c"),
		path.join(GLFW_DIR, "src/window.c"),

        path.join(GLFW_DIR, "src/platform.*"),
        path.join(GLFW_DIR, "src/null_*")
	}
	includedirs { path.join(GLFW_DIR, "include") }
	filter "system:windows"
		defines "_GLFW_WIN32"
		files
		{
			path.join(GLFW_DIR, "src/win32_*.*"),
			path.join(GLFW_DIR, "src/wgl_context.*")
		}
	filter "system:linux"
		defines "_GLFW_X11"
		files
		{
			path.join(GLFW_DIR, "src/glx_context.*"),
			path.join(GLFW_DIR, "src/linux*.*"),
			path.join(GLFW_DIR, "src/posix*.*"),
			path.join(GLFW_DIR, "src/x11*.*"),
			path.join(GLFW_DIR, "src/xkb*.*")
		}
	filter "system:macosx"
		defines "_GLFW_COCOA"
		files
		{
			path.join(GLFW_DIR, "src/cocoa_*.*"),
			path.join(GLFW_DIR, "src/posix_thread.h"),
			path.join(GLFW_DIR, "src/nsgl_context.h"),
			path.join(GLFW_DIR, "src/egl_context.h"),
			path.join(GLFW_DIR, "src/osmesa_context.h"),

			path.join(GLFW_DIR, "src/posix_thread.c"),
			path.join(GLFW_DIR, "src/nsgl_context.m"),
			path.join(GLFW_DIR, "src/egl_context.c"),
			path.join(GLFW_DIR, "src/nsgl_context.m"),
			path.join(GLFW_DIR, "src/osmesa_context.c"),                       
		}

	filter "action:vs*"
		defines "_CRT_SECURE_NO_WARNINGS"

project "flecs"
	kind "StaticLib"
	language "C++"
	defines { "WIN32", "_WINDOWS", "flecs_STATIC", "CMAKE_INTDIR=Debug"}
	buildoptions { "/machine:x64" }
	files  
	{  
		path.join(FLECS_DIR, "src/**.c"),
		path.join(FLECS_DIR, "src/**.cpp"),
		path.join(FLECS_DIR, "src/**.h"),
		path.join(FLECS_DIR, "include/**.h"),
		path.join(FLECS_DIR, "include/**.hpp")
	}
	includedirs { path.join(FLECS_DIR, "include") }


group ("shaderc")

external("glslang")
external("fcpp")
external("glsl-optimizer")
external("spirv-cross")
external("spirv-opt")

externalproject "shaderc"
	location (path.join(BGFX_DIR, ".build/projects", _ACTION))
	uuid (os.uuid("shaderc"))
	kind "ConsoleApp"
	language "C++"
	postbuildcommands 
    { 
        "{COPYFILE} \"" .. "%{cfg.buildtarget.directory}/%{cfg.buildtarget.name}\" " .. "%{wks.location}" .. "/lib/tools/" .. "%{cfg.buildtarget.name}\""
    }