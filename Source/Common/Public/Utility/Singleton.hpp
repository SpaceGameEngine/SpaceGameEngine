/*
Copyright 2024 creatorlxd

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
#include "GlobalVariable.h"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{

	/*!
	@attention inherit `Singleton<T>` to use it,and define the Allocator which you want to use as a friend in the class and
	define the class's constructor as a private function at the same time.
	*/
	template<typename T, typename Allocator = DefaultAllocator>
	struct Singleton
	{
		inline static T& GetSingleton()
		{
			static GlobalVariable<T, Allocator> g_Instance;
			return g_Instance.Get();
		}
	};

}

/*!
@}
*/