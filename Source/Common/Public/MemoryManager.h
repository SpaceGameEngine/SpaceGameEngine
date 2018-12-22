/*
Copyright 2018 creatorlxd

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
#include "TypeDefination.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

#ifdef _WIN32
#define SGE_MAX_MEMORY_SIZE INT32_MAX
#else
#define SGE_MAX_MEMORY_SIZE INT64_MAX
#endif

	struct StdAllocator
	{
		static void* RawNew(SizeType size);
		static void RawDelete(void* ptr);

		template<typename T,typename... Arg>
		static T* New(Arg&&... arg)
		{
			return new (RawNew(sizeof(T))) T(std::forward<Arg>(arg)...);
		}

		template<typename T>
		static void Delete(T* ptr)
		{
			AssertNullPointer(ptr);
			ptr->~T();
			RawDelete(ptr);
		}
	};

	/*!@todo change the std allocator to my allocator*/
	using DefaultAllocator = StdAllocator;

	/*!
	@}
	*/
}