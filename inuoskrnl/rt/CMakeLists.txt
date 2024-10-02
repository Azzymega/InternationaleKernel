
list(APPEND source
        base.c
        math.c
)

add_library(rt STATIC ${source})