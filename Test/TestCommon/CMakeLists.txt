add_executable(TestCommon main.cpp)
target_include_directories(TestCommon PRIVATE .)
target_link_libraries(TestCommon Bootstrap Common TestModule ${THIRD_PARTY_TEST_LIBRARIES})
add_custom_command(TARGET TestCommon POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/TestData ${CMAKE_BINARY_DIR})