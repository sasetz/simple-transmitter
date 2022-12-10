add_executable(tests_run PacketTest.cpp)
target_link_libraries(tests_run packet)
target_include_directories(tests_run PUBLIC ${DOCTEST_INCLUDE_DIR})
