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
#include "Error.h"
#include "SGEString.h"
#include "Utility/Utility.hpp"
#include <new>
#include <type_traits>
#include <utility>

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	/*!
	@todo add other member functions' information.
	@todo add more essential information.
	@todo support custom constructor.
	@todo use String instead of const char *.
	*/
	struct MetaData
	{
		const char* m_pName;
		SizeType m_Size;
		SizeType m_Alignment;
		void(*m_pDefaultConstructor)(void*);
		void(*m_pCopyConstructor)(void*, const void*);
		void(*m_pMoveConstructor)(void*, void*);
		void* (*m_pCopyAssignment)(void*, const void*);
		void* (*m_pMoveAssignment)(void*, void*);
		void(*m_pDestructor)(void*);
		bool(*m_pComparison)(const void*, const void*);

		MetaData();
		MetaData(const char* name, SizeType size, SizeType alignment, void(*default_constructor)(void*),
			void(*copy_constructor)(void*, const void*), void(*move_constructor)(void*, void*),
			void* (*copy_assignment)(void*, const void*), void* (*move_assignment)(void*, void*),
			void(*destructor)(void*), bool(*comparison)(const void*, const void*));
	};

	bool operator==(const MetaData& meta_data1, const MetaData& meta_data2);

	template <typename T>
	inline void(*QueryDefaultConstructor())(void*)
	{
		if constexpr (std::is_default_constructible_v<T>)
			return [](void* ptr) {
			SGE_ASSERT(NullPointerError, ptr);
			new (ptr) T();
		};
		else
			return nullptr;
	}

	template <typename T>
	inline void(*QueryCopyConstructor())(void*, const void*)
	{
		if constexpr (std::is_copy_constructible_v<T>)
			return [](void* ptr, const void* arg) {
			SGE_ASSERT(NullPointerError, ptr);
			SGE_ASSERT(NullPointerError, arg);
			new (ptr) T(*reinterpret_cast<const T*>(arg));
		};
		else
			return nullptr;
	}

	template <typename T>
	inline void(*QueryMoveConstructor())(void*, void*)
	{
		if constexpr (std::is_move_constructible_v<T>)
			return [](void* ptr, void* arg) {
			SGE_ASSERT(NullPointerError, ptr);
			SGE_ASSERT(NullPointerError, arg);
			new (ptr) T(std::move(*reinterpret_cast<T*>(arg)));
		};
		else
			return nullptr;
	}

	template <typename T>
	inline void* (*QueryCopyAssignment())(void*, const void*)
	{
		if constexpr (std::is_copy_assignable_v<T>)
			return [](void* ptr, const void* arg) -> void* {
			SGE_ASSERT(NullPointerError, ptr);
			SGE_ASSERT(NullPointerError, arg);
			return reinterpret_cast<void*>(
				&(*reinterpret_cast<T*>(ptr) = *reinterpret_cast<const T*>(arg)));
		};
		else
			return nullptr;
	}

	template <typename T>
	inline void* (*QueryMoveAssignment())(void*, void*)
	{
		if constexpr (std::is_move_assignable_v<T>)
			return [](void* ptr, void* arg) -> void* {
			SGE_ASSERT(NullPointerError, ptr);
			SGE_ASSERT(NullPointerError, arg);
			return reinterpret_cast<void*>(
				&(*reinterpret_cast<T*>(ptr) = std::move(*reinterpret_cast<T*>(arg))));
		};
		else
			return nullptr;
	}

	template <typename T>
	inline void(*QueryDestructor())(void*)
	{
		if constexpr (std::is_destructible_v<T>)
			return [](void* ptr) {
			SGE_ASSERT(NullPointerError, ptr);
			reinterpret_cast<T*>(ptr)->~T();
		};
		else
			return nullptr;
	}

	template <typename T>
	inline bool(*QueryComparison())(const void*, const void*)
	{
		if constexpr (IsComparable<T>::Value)
			return [](const void* arg1, const void* arg2) -> bool {
			SGE_ASSERT(NullPointerError, arg1);
			SGE_ASSERT(NullPointerError, arg2);
			return *reinterpret_cast<const T*>(arg1) == *reinterpret_cast<const T*>(arg2);
		};
		else
			return nullptr;
	}

	/*!
	@todo remove this function,use parser to fill metadata instead of using compiler.
	*/
	template <typename T>
	inline const MetaData& GetMetaData()
	{
		static MetaData instance(typeid(T).name(), sizeof(T), alignof(T), QueryDefaultConstructor<T>(),
			QueryCopyConstructor<T>(), QueryMoveConstructor<T>(), QueryCopyAssignment<T>(),
			QueryMoveAssignment<T>(), QueryDestructor<T>(), QueryComparison<T>());
		return instance;
	}
	/*!
	@}
	*/
}