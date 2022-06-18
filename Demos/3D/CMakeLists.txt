project(3D)

file(GLOB_RECURSE _sources CONFIGURE_DEPENDS "*.cpp" "*.h")

add_executable(3D ${_sources})

target_link_libraries(3D Engine)

add_custom_command(TARGET 3D PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources ${CMAKE_CURRENT_BINARY_DIR}/Resources)