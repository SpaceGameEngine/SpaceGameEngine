/*
Copyright 2019 creatorlxd

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
#include "TypeDefinition.hpp"
#include "Utility/Utility.hpp"
#include "Error.h"
#include "Platform.hpp"
#include "Concurrent/Lock.h"
#include "LockedFixedSizeAllocator.h"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	/*!
	@brief the memory manager for the engine to use
	@note The SegregatedFitAllocator is just a common memory manager using the different allocators which represent
	the different memory allocation strategies.It will choose the proper allocator to manage the memory depend
	on the current condition.
	*/
	class SegregatedFitAllocator : public Uncopyable
	{
	public:
		SegregatedFitAllocator();

		~SegregatedFitAllocator();

		/*!
		@attention the alignment can not be larger than 128 or equal to 0.
		*/
		void* Allocate(SizeType size, SizeType alignment);

		/*!
		@attention the alignment can not be larger than 128 or equal to 0.
		*/
		void Free(void* ptr, SizeType size, SizeType alignment);

	private:
		/*!
		@attention the first is size,the second is alignment.
		*/
		using RequestInformation = Pair<SizeType, SizeType>;

		struct InvalidRequestInformationError
		{
			inline static const TChar sm_pContent[] = SGE_TSTR("The RequestInformation is invalid");
			/*!
			@note the request_info's size must be less than or equal to 1024,
			the alignment of it can not be larger than 128.
			*/
			static bool Judge(const RequestInformation& request_info);
		};

		/*!
		@attention the request_info's size must be less than or equal to 1024,
		the alignment of it can not be larger than 128.
		*/
		UInt32 RequestInformationToIndex(const RequestInformation& request_info);

	private:
		/*!
		@note 262273=((1024<<8)|128)+1
		*/
		inline static const SizeType sm_MaxFixedSizeAllocatorQuantity = 262273;
		inline static const SizeType sm_MaxMemoryBlockSize = 1024;
		inline static const SizeType sm_MemoryPageSize = 8192;

		LockedFixedSizeAllocator* m_FixedSizeAllocators[sm_MaxFixedSizeAllocatorQuantity];
	};

	/*!
	@}
	*/
}