cmake_minimum_required(VERSION "3.12")
project("kubics-console")

# target platform
set(MSWINDOWS TRUE)
#set(LINUX TRUE)

# uncomment/comment to or to not generate test exe
set(GENERATE_TEST_EXE TRUE)

# setup dependencies
add_subdirectory("dependencies")
set(GLEW_DIR ${GLEW_DIR})
set(GLEW_LIB_DIR ${GLEW_LIB_DIR})

# setup kconsole library
add_subdirectory("source")

# setup text exe
if(GENERATE_TEST_EXE)
    add_subdirectory("test")
endif()