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
#include "AllocatorCommon.h"

namespace SpaceGameEngine::Detail
{
	template<typename TinyAllocatorType, int dividingPoint, typename LargeAllocatorType,
			 typename = std::enable_if_t<is_greater(dividingPoint, 0)>>
	struct DoubleGradedAllocator
	{
	public:
		ALLOCATE_FUNCTION_SIGNATURE
		{
			return size <= dividingPoint ? tinyAllocator.allocate(size, alignment)
										 : largeAllocator.allocate(size, alignment);
		}

		DEALLOCATE_FUNCTION_SIGNATURE
		{
			size <= dividingPoint ? tinyAllocator.deallocate(pointer, size, alignment)
								  : largeAllocator.deallocate(pointer, size, alignment);
		}

	private:
		TinyAllocatorType tinyAllocator;
		LargeAllocatorType largeAllocator;
	};

	template<typename FirstLevelAllocatorType, int firstDividingPoint,
			 typename SecondLevelAllocatorType, int secondDividingPoint,
			 typename ThirdLevelAllocatorType,
			 typename = std::enable_if_t<is_greater(firstDividingPoint, 0)>,
			 typename = std::enable_if_t<is_greater(secondDividingPoint, firstDividingPoint)>>
	struct TripleGradedAllocator
	{
	public:
		ALLOCATE_FUNCTION_SIGNATURE
		{
			if (size <= firstDividingPoint)
				return firstAllocator.allocate(size, alignment);
			if (size <= secondDividingPoint)
				return secondAllocator.allocate(size, alignment);
			return thirdAllocator.allocate(size, alignment);
		}

		DEALLOCATE_FUNCTION_SIGNATURE
		{
			if (size <= firstDividingPoint)
				firstAllocator.deallocate(pointer, size, alignment);
			else if (size <= secondDividingPoint)
				secondAllocator.deallocate(pointer, size, alignment);
			else
				thirdAllocator.deallocate(pointer, size, alignment);
		}

	private:
		FirstLevelAllocatorType firstAllocator;
		SecondLevelAllocatorType secondAllocator;
		ThirdLevelAllocatorType thirdAllocator;
	};

	struct StdAllocator
	{
		ALLOCATE_FUNCTION_SIGNATURE
		{
			return _mm_malloc(size, alignment);
		}
		DEALLOCATE_FUNCTION_SIGNATURE
		{
			_mm_free(pointer);
		}
	};
}