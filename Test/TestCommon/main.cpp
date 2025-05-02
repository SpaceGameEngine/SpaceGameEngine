/*
Copyright 2025 creatorlxd

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "gtest/gtest.h"
#include "Bootstrap.h"
#include "TestError.hpp"
#include "TestString.hpp"
#include "TestMemoryManager.hpp"
#include "TestFunction.hpp"
#include "TestMetaData.hpp"
#include "Utility/TestAutoReleaseBuffer.hpp"
#include "Utility/TestControllableObject.hpp"
#include "Utility/TestDebugInformation.hpp"
#include "Utility/TestEndian.hpp"
#include "Utility/TestFixedSizeBuffer.hpp"
#include "Utility/TestHash.hpp"
#include "Utility/TestOptional.hpp"
#include "Utility/TestPair.hpp"
#include "Utility/TestUtility.hpp"
#include "Concurrent/TestConcurrent.hpp"
#include "Container/TestVector.hpp"
#include "Container/TestContainerConcept.hpp"
#include "TestMeta.hpp"
#include "Time/TestTimeCounter.hpp"
#include "Container/Detail/TestRedBlackTree.hpp"
#include "Container/TestMap.hpp"
#include "Container/TestSet.hpp"
#include "Container/Detail/TestHashTable.hpp"
#include "Container/TestHashMap.hpp"
#include "Container/TestHashSet.hpp"
#include "Container/TestList.hpp"
#include "Container/TestStack.hpp"
#include "Container/TestQueue.hpp"
#include "Container/TestPriorityQueue.hpp"
#include "System/TestDllLoader.hpp"
#include "TestModule.hpp"
#include "TestFormat.hpp"
#include "TestFile.hpp"
#include "Time/TestDate.hpp"
#include "TestLog.hpp"

SpaceGameEngine::Bootstrap bootstrap;

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}