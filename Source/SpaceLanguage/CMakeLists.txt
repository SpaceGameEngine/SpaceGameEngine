add_library(SpaceLanguage SHARED
        Private/SpaceLanguageModule.cpp
        Private/Logger.cpp
        Private/Lexer.cpp
        Private/VirtualMachine/Instruction.cpp
        Private/VirtualMachine/Register.cpp
        Private/VirtualMachine/ExternalCaller.cpp
        Private/VirtualMachine/VirtualMachine.cpp
        Private/VirtualMachine/ExternalCallerModule/CoreModule.cpp
        Private/VirtualMachine/Assembler.cpp
        Private/IntermediateRepresentation/Type.cpp
        Private/IntermediateRepresentation/Variable.cpp
        Private/IntermediateRepresentation/Operation.cpp
        Private/IntermediateRepresentation/BasicBlock.cpp
        Private/IntermediateRepresentation/Function.cpp
        Private/IntermediateRepresentation/RegisterAllocator.cpp
        Private/IntermediateRepresentation/Translator.cpp
        Private/Linker/CompiledObject.cpp
        )
target_compile_definitions(SpaceLanguage PRIVATE SPACE_LANGUAGE_EXPORTS)
target_include_directories(SpaceLanguage PUBLIC Public)
target_link_libraries(SpaceLanguage PUBLIC Common)
if((CMAKE_HOST_UNIX) AND (NOT CMAKE_HOST_APPLE))
target_link_libraries(SpaceLanguage PUBLIC ${CMAKE_DL_LIBS})
endif()
# CopySharedLibrary(SpaceLanguage)