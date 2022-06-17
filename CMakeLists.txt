cmake_minimum_required(VERSION 3.22.2)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmakeModules")

project("Thera" 
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib/src)
add_subdirectory(Engine)
add_subdirectory(Demos)
# add_subdirectory(EngineTests)
# add_subdirectory(Demos)