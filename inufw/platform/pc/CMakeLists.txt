
add_subdirectory(module)

include_directories(module/include)

target_sources(inufw PUBLIC
        base.c
)

target_link_libraries(inufw PRIVATE framebuffermodule)
target_link_libraries(inufw PRIVATE piomodule)
target_link_libraries(inufw PRIVATE atamodule)
target_link_libraries(inufw PRIVATE tarmodule)
target_link_libraries(inufw PRIVATE elfmodule)
target_link_libraries(inufw PRIVATE picmodule)
target_link_libraries(inufw PRIVATE pitmodule)
target_link_libraries(inufw PRIVATE intmodule)
target_link_libraries(inufw PRIVATE gdtmodule)