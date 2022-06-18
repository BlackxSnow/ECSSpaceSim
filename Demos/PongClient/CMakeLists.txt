project(PongClient)

file(GLOB_RECURSE _sources CONFIGURE_DEPENDS "*.cpp" "*.h")

add_executable(${PROJECT_NAME} ${_sources})

target_link_libraries(${PROJECT_NAME} Engine)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources ${CMAKE_CURRENT_BINARY_DIR}/Resources)