set(BUILD_STATIC_LIBS TRUE)
add_subdirectory("glm/")
set(FREETYPE2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/freetype2/include")
add_subdirectory("freetype2/")