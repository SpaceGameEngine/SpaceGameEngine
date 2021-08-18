﻿/*
Copyright 2021 creatorlxd

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
#pragma once
#include "gtest/gtest.h"
#include "System/DllLoader.h"

using namespace SpaceGameEngine;

TEST(DllLoader, InvokeTest)
{
#ifdef SGE_WINDOWS
	String dll_path = GetDllPath(String(SGE_STR("TestModule")));
	DllHandle test_module_dll = LoadDll(dll_path);
	int (*apb)(int, int) = nullptr;
	apb = (decltype(apb))GetAddressFromDll(test_module_dll, String(SGE_STR("APlusB")));
	ASSERT_EQ(apb(12, 34), 46);
	MemoryManager* (*get_mm)() = nullptr;
	get_mm = (decltype(get_mm))GetAddressFromDll(test_module_dll, String(SGE_STR("GetMemoryManagerAddress")));
	ASSERT_EQ(get_mm(), &MemoryManager::GetSingleton());
	UnloadDll(test_module_dll);
#elif defined(SGE_UNIX)
#endif
}