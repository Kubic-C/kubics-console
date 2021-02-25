
if(LINUX)
    message(STATUS "glew is being built for linux.")
    add_subdirectory("linux")
elseif(MSWINDOWS)
    message(STATUS "glew is being built for mswindows.")
    add_subdirectory("mswindows")
else()
    message(STATUS "NO TARGET PLATFORM SET")
endif()

set(GLEW_DIR ${GLEW_DIR} PARENT_SCOPE)
set(GLEW_LIB_DIR ${GLEW_LIB_DIR} PARENT_SCOPE)