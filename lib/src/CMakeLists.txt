include(common)
include(asio.cmake)

# Configure Assimp
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build assimp as shared lib" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Build assimp with tools" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Build assimp unit tests" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "Install assimp library" FORCE)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "Build assimp view tool (DirectX)" FORCE)

add_subdirectory(assimp)
subdir_to_folder(assimp assimp)

# Configure flecs
set(FLECS_SHARED_LIBS OFF CACHE BOOL "Build flecs shared lib" FORCE)

add_subdirectory(flecs)

# Configure GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)

add_subdirectory(glfw)
subdir_to_folder(glfw glfw)

add_subdirectory(glm)
subdir_to_folder(glm glm)

include(imgui.cmake)

add_subdirectory(DiligentCore)
subdir_to_folder(DiligentCore Diligent)

# Place all targets into Dependencies/ while preserving their internal structure
subdir_to_folder_prepend(${CMAKE_CURRENT_SOURCE_DIR} "Dependencies")