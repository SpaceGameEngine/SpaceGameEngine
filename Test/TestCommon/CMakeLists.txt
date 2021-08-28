add_executable(TestCommon main.cpp)
target_include_directories(TestCommon PRIVATE .)
target_link_libraries(TestCommon Bootstrap Common TestModule ${THIRD_PARTY_TEST_LIBRARIES})