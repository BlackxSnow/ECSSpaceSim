# file(GLOB_RECURSE _sources CONFIGURE_DEPENDS "asio/include/*.h")

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE asio/asio/include/)