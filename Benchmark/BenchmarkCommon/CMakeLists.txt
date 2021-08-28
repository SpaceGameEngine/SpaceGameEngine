add_executable(BenchmarkCommon main.cpp)
target_include_directories(BenchmarkCommon PRIVATE .)
target_link_libraries(BenchmarkCommon Bootstrap Common ${THIRD_PARTY_TEST_LIBRARIES})