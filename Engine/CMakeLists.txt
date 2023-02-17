cmake_minimum_required(VERSION 3.22.2)

project("Engine" 
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "")
message("CXX: ${CMAKE_CXX_FLAGS_DEBUG}")

file(GLOB_RECURSE engineInclude CONFIGURE_DEPENDS "include/*.h")
file(GLOB_RECURSE engineSource CONFIGURE_DEPENDS "src/*.cpp" "src/*.h")

add_library(${PROJECT_NAME} STATIC ${engineSource} ${engineInclude})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${engineSource})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${engineInclude})

target_include_directories(${PROJECT_NAME}
    PUBLIC include
    PUBLIC ${CMAKE_SOURCE_DIR}/lib/include
    PUBLIC ${CMAKE_SOURCE_DIR}/lib/src/DiligentCore)
#     PUBLIC ${CMAKE_SOURCE_DIR}/lib/src/glfw/include
#     PUBLIC ${CMAKE_SOURCE_DIR}/lib/src/glm
#     PRIVATE ${CMAKE_SOURCE_DIR}/lib/src/assimp/include
#     PRIVATE ${CMAKE_SOURCE_DIR}/lib/src/bx/include
#     PUBLIC ${CMAKE_SOURCE_DIR}/lib/src/bgfx/include
#     PUBLIC ${CMAKE_SOURCE_DIR}/lib/src/flecs/include)

target_link_libraries(${PROJECT_NAME}
    PUBLIC glfw
    PUBLIC glm
    PRIVATE assimp
#    PUBLIC DiligentCore
    PUBLIC flecs_static
    PUBLIC asio
    PUBLIC imgui
    PUBLIC Diligent-PublicBuildSettings
    PRIVATE
#    Diligent-BuildSettings
#    Diligent-GraphicsEngineD3D11-shared
    Diligent-GraphicsEngineOpenGL-shared
#    Diligent-GraphicsEngineD3D12-shared
    Diligent-GraphicsEngineVk-shared
#    Diligent-GraphicsEngineMetal-shared
    )

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE iphlpapi)
endif()