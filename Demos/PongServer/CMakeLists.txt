project(PongServer)

file(GLOB_RECURSE _sources CONFIGURE_DEPENDS "*.cpp" "*.h")

add_executable(${PROJECT_NAME} ${_sources})

target_link_libraries(${PROJECT_NAME} Engine)