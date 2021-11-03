/*
Copyright 2021 creatorlxd

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
#include "CommonAPI.h"
#include "Container/Stack.hpp"
#include "Concurrent/Lock.h"
#include "Utility/Utility.hpp"
#include "Function.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	template class COMMON_API ControllableObject<MetaObject<StdAllocator>, StdAllocator>;
	template class COMMON_API Function<void(), StdAllocator>;
	template class COMMON_API List<Function<void(), StdAllocator>, StdAllocator>;
	template class COMMON_API Stack<Function<void(), StdAllocator>, StdAllocator>;

	class COMMON_API GlobalVariableManager : public UncopyableAndUnmovable
	{
	public:
		~GlobalVariableManager();

		static GlobalVariableManager& GetSingleton();

		template<typename T, typename Allocator>
		friend class GlobalVariable;

	private:
		GlobalVariableManager();
		void Add(Function<void(), StdAllocator>&& rel_func);

	private:
		Mutex m_Mutex;
		Stack<Function<void(), StdAllocator>, StdAllocator> m_Content;
	};

	template<typename T, typename Allocator = DefaultAllocator>
	class GlobalVariable : public UncopyableAndUnmovable
	{
	public:
		template<typename... Args>
		inline GlobalVariable(Args&&... args)
		{
			m_pContent = Allocator::template New<T>(std::forward<Args>(args)...);
			GlobalVariableManager::GetSingleton().Add([ptr = m_pContent]() {
				Allocator::Delete(ptr);
			});
		}

		inline T& Get()
		{
			return *m_pContent;
		}

		inline const T& Get() const
		{
			return *m_pContent;
		}

	private:
		T* m_pContent;
	};

	/*!
	@}
	*/
}