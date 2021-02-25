add_executable(kconsole_test "main.cpp")
target_link_libraries(kconsole_test PUBLIC kconsole)
target_include_directories(kconsole_test PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include")