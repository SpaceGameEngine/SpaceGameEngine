/*
Copyright 2019 Chenxi Xu (@xsun2001)

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

#include <type_traits>
#include <functional>
#include "TypeDefinition.hpp"
#ifdef SGE_WINDOWS
#include <malloc.h>
#else
#include <mm_malloc.h>
#endif

#define ALLOCATE_FUNCTION_SIGNATURE void* allocate(SizeType size, SizeType alignment)
#define DEALLOCATE_FUNCTION_SIGNATURE void deallocate(void* pointer, SizeType size, SizeType alignment)

namespace SpaceGameEngine::Detail
{
	constexpr bool is_greater(int x, int y)
	{
		return x > y;
	}
}