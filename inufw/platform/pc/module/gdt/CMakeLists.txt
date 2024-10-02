
list(APPEND source
        base.c
        gdt.s
)

add_library(gdtmodule STATIC ${source})