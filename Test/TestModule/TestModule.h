/*
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
#include "CommonAPI.h"
#include "MemoryManager.h"

/*
this module is a test case for TestCommon module to test DllLoader and Module System.
*/

#ifdef SGE_USE_DLL
#ifdef TESTMODULE_EXPORTS
#define TEST_MODULE_API SGE_DLL_EXPORT
#else
#define TEST_MODULE_API SGE_DLL_IMPORT
#endif
#else
#define TEST_MODULE_API
#endif

extern "C" TEST_MODULE_API int APlusB(int a, int b);

extern "C" TEST_MODULE_API SpaceGameEngine::MemoryManager* GetMemoryManagerAddress();