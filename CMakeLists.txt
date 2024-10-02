cmake_minimum_required(VERSION 3.28)

project(INUYASHA C ASM)

set(CMAKE_C_STANDARD 99)

include_directories(include)

add_subdirectory(inuoskrnl)
add_subdirectory(inuldr)
add_subdirectory(inuhal)
add_subdirectory(inufw)

target_link_libraries(inuldr PRIVATE inufw)
target_link_libraries(inuldr PRIVATE inuhal)

target_link_libraries(inuldr PRIVATE bm)
target_link_libraries(inuldr PRIVATE rt)
target_link_libraries(inuldr PRIVATE pm)
target_link_libraries(inuldr PRIVATE ga)
target_link_libraries(inuldr PRIVATE ob)
target_link_libraries(inuldr PRIVATE ps)
target_link_libraries(inuldr PRIVATE ki)
target_link_libraries(inuldr PRIVATE cm)
target_link_libraries(inuldr PRIVATE vm)
target_link_libraries(inuldr PRIVATE nt)
target_link_libraries(inuldr PRIVATE sm)
target_link_libraries(inuldr PRIVATE ldr)
target_link_libraries(inuldr PRIVATE pvm)

target_link_libraries(inuoskrnl PRIVATE inuhal)
target_link_libraries(inuoskrnl PRIVATE inufw)