/*
Copyright 2021 Chenxi Xu (@xsun2001)

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
#include "Memory/AllocatorBase.h"
#include "Memory/SegregatedFitAllocator.h"
#include "Memory/NewSegregatedFitAllocator.h"
#include "Memory/NativeAllocator.h"
#include "Utility/Utility.hpp"

namespace SpaceGameEngine
{
	template<typename AllocatorType>
	class AllocatorWrapper : SpaceGameEngine::Singleton<AllocatorType>
	{
		friend SpaceGameEngine::Singleton<AllocatorType>;

	public:
		static void* RawNew(SizeType size, SizeType alignment = 0)
		{
			return AllocatorWrapper<AllocatorType>::GetSingleton().Allocate(size, alignment);
		}
		static void RawDelete(void* ptr, SizeType size, SizeType alignment = 0)
		{
			AllocatorWrapper<AllocatorType>::GetSingleton().Free(ptr, size, alignment);
		}

		template<typename T, typename... Args>
		static T* New(Args&&... args)
		{
			return new (RawNew(sizeof(T), alignof(T))) T(std::forward<Args>(args)...);
		}

		template<typename T>
		static void Delete(T* ptr)
		{
			SGE_ASSERT(NullPointerError, ptr);
			ptr->~T();
			RawDelete(ptr, sizeof(T), alignof(T));
		}

	private:
		AllocatorWrapper() = default;
	};

	using DefaultAllocator = AllocatorWrapper<SegregatedFitAllocator>;
}