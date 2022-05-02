/*
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
#include "SGEString.hpp"
#include "Module/ModuleManager.h"
#include "TestVirtualMachine.hpp"
#include "TestLexer.hpp"
#include "TestAssembler.hpp"
#include "TestCoreModule.hpp"

SpaceGameEngine::Bootstrap bootstrap;

int main(int argc, char** argv)
{
	SpaceGameEngine::ModuleManager::GetSingleton().LoadModule(SGE_STR("SpaceLanguage"));
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}