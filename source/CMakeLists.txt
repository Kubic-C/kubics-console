# make the library

set(headers
    "kconsole/headers/base.hpp"
    "kconsole/headers/font_loading.hpp"
    "kconsole/headers/kconsole.hpp"
    "kconsole/headers/kconsole_buffer.hpp"
    "kconsole/headers/kconsole_character.hpp"
)

set(sources
    "kconsole/base.cpp"
    "kconsole/font_loading.cpp"
    "kconsole/kconsole.cpp"
    "kconsole/kconsole_buffer.cpp"
    "kconsole/kconsole_character.cpp"
)

add_library(
    kconsole
    STATIC
    ${headers}
    ${sources}
)