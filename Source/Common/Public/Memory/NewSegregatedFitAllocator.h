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

#include "BitmapFixedSizeAllocator.h"
namespace SpaceGameEngine
{
	class NewSegregatedFitAllocator : public Uncopyable
	{
	public:
		explicit NewSegregatedFitAllocator();

		~NewSegregatedFitAllocator() = default;

		/*!
		 * @todo Add alignment support. Currently the allocated block is 8-byte aligned.
		 */
		void* Allocate(SizeType size, SizeType alignment);

		void Free(void* ptr, SizeType size, SizeType alignment);

	private:
		static UInt8 GetSizeLevel(UInt32 size);
		BitmapFixedSizeAllocator* allocatorHeaders[8];
	};
}