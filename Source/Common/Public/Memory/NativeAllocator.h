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

namespace SpaceGameEngine
{
	class NativeAllocator
	{
	public:
		/*!
		@attention the alignment can not be larger than 128 or equal to 0.
		*/
		void* Allocate(SizeType size, SizeType alignment);
		/*!
		@attention the alignment can not be larger than 128 or equal to 0.
		*/
		void Free(void* ptr, SizeType size, SizeType alignment);
	};
}