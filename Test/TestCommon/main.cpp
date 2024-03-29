﻿/*
Copyright 2022 creatorlxd

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
#include "TestUtility.hpp"
#include "TestConcurrent.hpp"
#include "TestVector.hpp"
#include "TestContainerConcept.hpp"
#include "TestRange.hpp"
#include "TestMeta.hpp"
#include "TestTimeCounter.hpp"
#include "TestMap.hpp"
#include "TestHashMap.hpp"
#include "TestList.hpp"
#include "TestStack.hpp"
#include "TestQueue.hpp"
#include "TestPriorityQueue.hpp"
#include "TestDllLoader.hpp"
#include "TestModule.hpp"
#include "TestFormat.hpp"
#include "TestFile.hpp"
#include "TestDate.hpp"
#include "TestLog.hpp"

SpaceGameEngine::Bootstrap bootstrap;

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}