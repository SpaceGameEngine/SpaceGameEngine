add_library(CommonParser SHARED
        Private/CommonParserModule.cpp
        Private/Logger.cpp
        Private/Lexer.cpp
        )
target_compile_definitions(CommonParser PRIVATE COMMON_INTERMEDIATE_REPRESENTATION_EXPORTS)
target_include_directories(CommonParser PUBLIC Public)
target_link_libraries(CommonParser PUBLIC Common)
if((CMAKE_HOST_UNIX) AND (NOT CMAKE_HOST_APPLE))
target_link_libraries(CommonParser PUBLIC ${CMAKE_DL_LIBS})
endif()
# CopySharedLibrary(CommonParser)