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
#include "Utility.hpp"
#include "MemoryManager.h"
#include "Container/Vector.hpp"
#include "Function.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	/*!
	@brief a buffer which can release the temporary objects created by it when
	it is destructed.
	*/
	class AutoReleaseBuffer
	{
	public:
		AutoReleaseBuffer();
		~AutoReleaseBuffer();

		AutoReleaseBuffer(const AutoReleaseBuffer&) = delete;
		AutoReleaseBuffer& operator=(const AutoReleaseBuffer&) = delete;

		AutoReleaseBuffer(AutoReleaseBuffer&& a);
		AutoReleaseBuffer& operator=(AutoReleaseBuffer&& a);

		template<typename T, typename Allocator = DefaultAllocator, typename... Args>
		inline T* NewObject(Args&&... args)
		{
			T* re = Allocator::template New<T>(std::forward<Args>(args)...);
			m_Functions.EmplaceBack([re]() {
				Allocator::Delete(re);
			});
			return re;
		}

	private:
		Vector<Function<void()>> m_Functions;
	};

	/*!
	@}
	*/
}