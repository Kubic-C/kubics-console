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

target_link_libraries(
	kconsole
	PUBLIC
	glm_static
)

target_link_libraries(
	kconsole
	PUBLIC
	freetype
)

target_include_directories(
	kconsole
	PUBLIC
	${FREETYPE2_DIR}
)

message(STATUS "DIR TO FT2: ${FREETYPE2_DIR}")