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

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <string>
#include "TypeDefinition.hpp"
#include "Utility/Utility.hpp"
#define PAGE_SIZE 4096
typedef std::atomic<SpaceGameEngine::UInt32> atomic_uint;
namespace SpaceGameEngine
{
	class NewSegregatedFitAllocator;
	class BitmapFixedSizeAllocator : public Uncopyable
	{
	public:
		explicit BitmapFixedSizeAllocator(UInt8 page_type);
		~BitmapFixedSizeAllocator();
		void* Allocate();
		void Free(void* ptr);
		bool TryFree(void* ptr);
		friend NewSegregatedFitAllocator;

	private:
		BitmapFixedSizeAllocator* GetNextPage();
		std::atomic<bool>* GetNextPageLock();
		SpaceGameEngine::UInt8 GetPageType();
		std::atomic<UInt16>* GetRemaining();
		atomic_uint* GetBitmap();
		void* GetBlockBase();
		void SetNextPage(void* next_page);
		void LockNextPage();
		void UnlockNextPage();

		Byte mm_page[PAGE_SIZE];
	};
}