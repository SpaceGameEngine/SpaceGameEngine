add_library(CommonIntermediateRepresentation SHARED
        Private/CommonIntermediateRepresentationModule.cpp
        Private/Logger.cpp
        Private/Dialect.cpp
        Private/Context.cpp
        Private/Value.cpp
        Private/Operation.cpp
        Private/Assembler/Lexer.cpp
        Private/Assembler/Parser.cpp
        )
target_compile_definitions(CommonIntermediateRepresentation PRIVATE COMMON_INTERMEDIATE_REPRESENTATION_EXPORTS)
target_include_directories(CommonIntermediateRepresentation PUBLIC Public)
target_link_libraries(CommonIntermediateRepresentation PUBLIC Common CommonParser)
if((CMAKE_HOST_UNIX) AND (NOT CMAKE_HOST_APPLE))
target_link_libraries(CommonIntermediateRepresentation PUBLIC ${CMAKE_DL_LIBS})
endif()
# CopySharedLibrary(CommonIntermediateRepresentation)