
set(CMAKE_CROSSCOMPILING 1)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

set(CMAKE_SYSTEM_NAME Linux)

set(INUYASHA_PROCESSOR_TARGET i686-pc-linux-gnu)

set(INUYASHA_FIRMWARE_TARGET "PC")
set(INUYASHA_FIRMWARE_BOOTLOADER_PROTOCOL "MULTIBOOT1")

set(CMAKE_SYSTEM_PROCESSOR i686)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_ASM_COMPILER clang)

set(CMAKE_C_COMPILER_TARGET ${INUYASHA_PROCESSOR_TARGET})
set(CMAKE_CXX_COMPILER_TARGET ${INUYASHA_PROCESSOR_TARGET})
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -target ${INUYASHA_PROCESSOR_TARGET}")
#set(CMAKE_ASM_COMPILER_TARGET ${TARGET}) Hackfix! This not set target!

set(LINKING_FLAGS -nostdlib -fuse-ld=lld)
set(KERNEL_LINK_FLAGS ${LINKING_FLAGS} -fno-plt -fno-pic -Wl,--no-pie)
set(LOADER_LINK_FLAGS ${LINKING_FLAGS} -Wl,--no-pie)

set(CLANG_ERROR_FLAGS -Wno-vla)
set(COMPILING_FLAGS ${CLANG_ERROR_FLAGS} -ffreestanding -Wextra -Wfatal-errors -mno-red-zone -O0 -fno-exceptions -fno-rtti -Wno-inline-new-delete -mno-sse -fshort-wchar)

add_compile_options(${COMPILING_FLAGS})
































#set(CMAKE_MODULE_LINKER_FLAGS "${LINKING_FLAGS}")
#set(CMAKE_EXE_LINKER_FLAGS "${LINKING_FLAGS}")
#set(CMAKE_CROSSCOMPILING 1)
#set(CMAKE_C_COMPILER_WORKS 1)
#set(CMAKE_CXX_COMPILER_WORKS 1)
#
#set(CMAKE_SYSTEM_NAME Windows)
#
#set(TARGET i686-unknown-windows-gnu)
#
#function(set_entry Module Entry)
#    target_link_options(${Module} PRIVATE "-Wl,--entry=${Entry}")
#endfunction()
#
#set(CMAKE_CXX_STANDARD_LIBRARIES "")
#set(CMAKE_C_STANDARD_LIBRARIES "")
#
#set(CMAKE_MODULE_LINKER_FLAGS "-nodefaultlibs -nostdlib -fuse-ld=lld")
#
#add_compile_options(-ffreestanding -Wall)
#
#set(CMAKE_SYSTEM_PROCESSOR i686)
#set(CMAKE_C_COMPILER clang)
#set(CMAKE_CXX_COMPILER clang++)
#
#set(CMAKE_C_COMPILER_TARGET ${TARGET})
#set(CMAKE_CXX_COMPILER_TARGET ${TARGET})