add_library(Common
        Private/Error.cpp
        Private/MemoryManager.cpp
        Private/Concurrent/Thread.cpp
        Private/Concurrent/Lock.cpp
        Private/MetaData/MetaData.cpp
        Private/Utility/AutoReleaseBuffer.cpp
        Private/Time/TimeCounter.cpp
        Private/Time/WindowsTimeImplement.cpp
        Private/Time/UnixTimeImplement.cpp
        Private/System/DllLoader.cpp
        Private/Module/Module.cpp
        Private/Module/ModuleManager.cpp
        )
target_include_directories(Common PUBLIC Public)
target_link_libraries(Common PUBLIC ${THIRD_PARTY_LIBRARIES})