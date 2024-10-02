
add_subdirectory(module)

add_library(inufw STATIC)

target_link_libraries(inufw PRIVATE drawenginemodule)

add_subdirectory(platform)