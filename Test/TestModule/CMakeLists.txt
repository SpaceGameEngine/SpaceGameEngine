add_library(TestModule SHARED TestModule.cpp)
target_include_directories(TestModule PUBLIC .)
target_link_libraries(TestModule PUBLIC Common)
# CopySharedLibrary(TestModule)