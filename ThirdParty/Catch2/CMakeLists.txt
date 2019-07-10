add_library(Catch2 INTERFACE)
# Isn't the best method. To be improved.
target_include_directories(Catch2 INTERFACE ${CMAKE_SOURCE_DIR}/ThirdParty)