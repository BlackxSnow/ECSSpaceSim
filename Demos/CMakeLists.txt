include(common)

# set_property(GLOBAL PROPERTY CMAKE_FOLDER "Demos")

add_subdirectory(3D)
add_subdirectory(PongClient)
add_subdirectory(PongServer)

subdir_to_folder_prepend(${CMAKE_CURRENT_SOURCE_DIR} "Demos")