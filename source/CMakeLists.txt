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

# glm lib
target_link_libraries(
	kconsole
	PRIVATE
	glm_static
)

# ft2 lib
target_link_libraries(
	kconsole
	PRIVATE
	freetype
)

# glfw lib
target_link_libraries(
	kconsole
	PRIVATE
	glfw
)

#glew lib
target_compile_definitions(
	kconsole
	PRIVATE
	GLEW_STATIC
)

target_link_libraries(
	kconsole
	PRIVATE
	opengl32.lib
)

target_link_libraries(
	kconsole
	PRIVATE
	${GLEW_LIB_DIR}
)

# glew include path
target_include_directories(
	kconsole
	PRIVATE
	${GLEW_DIR}
)