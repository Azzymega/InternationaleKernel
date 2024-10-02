
add_executable(inuenv base.c)
target_compile_options(inuenv PRIVATE -fno-pic)
target_link_options(inuenv PRIVATE -r)