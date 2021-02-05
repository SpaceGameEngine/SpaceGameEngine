add_library(Common
        Private/Error.cpp
        Private/MemoryManager.cpp
        Private/SGEString.cpp
        Private/StringImplement/StdTStringImplement.cpp
        Private/Concurrent/Thread.cpp
        Private/Concurrent/Lock.cpp
        Private/MetaData/MetaData.cpp
        Private/Utility/AutoReleaseBuffer.cpp
        )
target_include_directories(Common PUBLIC Public)
target_link_libraries(Common PUBLIC ${THIRD_PARTY_LIBRARIES})