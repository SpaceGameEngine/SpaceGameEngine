add_library(Common
        Private/Error.cpp
        Private/MemoryManager.cpp
        Private/SGEString.cpp
        Private/StringImplement/StdTStringImplement.cpp
        private/Concurrent/Thread.cpp
        )
target_include_directories(Common PUBLIC Public)
target_link_libraries(Common PRIVATE ${THIRD_PARTY_LIBRARIES})