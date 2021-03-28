# make the library

set(headers
    "kconsole/headers/base.hpp"
    "kconsole/headers/kconsole_font.hpp"
    "kconsole/headers/kconsole.hpp"
    "kconsole/headers/kconsole_buffer.hpp"
	"kconsole/headers/kconsole_shader.hpp"
	"kconsole/headers/helper.hpp"
	"kconsole/headers/kconsole_utility.hpp"
)

set(sources
    "kconsole/base.cpp"
    "kconsole/kconsole_font.cpp"
    "kconsole/kconsole.cpp"
    "kconsole/kconsole_buffer.cpp"
	"kconsole/kconsole_shader.cpp"
	"kconsole/helper.cpp"
	"kconsole/kconsole_utility.cpp"
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
	PUBLIC
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

# openGL lib
target_link_libraries(
	kconsole
	PRIVATE
	opengl32.lib
)

# glew lib
target_compile_definitions(
	kconsole
	PRIVATE
	GLEW_STATIC
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