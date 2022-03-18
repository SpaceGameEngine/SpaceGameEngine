add_library(Common SHARED
        Private/Error.cpp
        Private/MemoryManager.cpp
        Private/Concurrent/Thread.cpp
        Private/Concurrent/Lock.cpp
        Private/MetaData/MetaData.cpp
        Private/Utility/AutoReleaseBuffer.cpp
        Private/Utility/Endian.cpp
        Private/Utility/DebugInformation.cpp
        Private/Time/TimeCounter.cpp
        Private/Time/WindowsTimeImplement.cpp
        Private/Time/UnixTimeImplement.cpp
        Private/Time/Date.cpp
        Private/System/DllLoader.cpp
        Private/Module/Module.cpp
        Private/Module/ModuleManager.cpp
        Private/Module/CommonModule.cpp
        Private/GlobalVariable.cpp
        Private/File.cpp
        )
target_include_directories(Common PUBLIC Public)
target_link_libraries(Common PUBLIC ${THIRD_PARTY_LIBRARIES})
if((CMAKE_HOST_UNIX) AND (NOT CMAKE_HOST_APPLE))
target_link_libraries(Common PUBLIC ${CMAKE_DL_LIBS})
endif()
# CopySharedLibrary(Common)