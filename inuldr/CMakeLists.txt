
if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
    add_executable(inuldr)
    target_link_options(inuldr PRIVATE "-Wl,-T../targets/inuldr/IA32.ld")
    target_link_options(inuldr PRIVATE ${LOADER_LINK_FLAGS})
endif ()

add_subdirectory(arch)