set(BUILD_STATIC_LIBS TRUE)

# GLM LIB
add_subdirectory("glm/")

# FREETYPE2 LIB
add_subdirectory("freetype2/")

# GLFW LIB
add_subdirectory("opengl/glfw")

# GLEW LIB
add_subdirectory("opengl/glew")

set(GLEW_DIR ${GLEW_DIR} PARENT_SCOPE)
set(GLEW_LIB_DIR ${GLEW_LIB_DIR} PARENT_SCOPE)