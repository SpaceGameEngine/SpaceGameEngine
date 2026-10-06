/*
Copyright 2026 creatorlxd

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
#include "CommonIntermediateRepresentationAPI.h"
#include "Container/HashMap.hpp"
#include "Utility/Utility.hpp"
#include "Utility/TypeId.hpp"

/*!
@ingroup CommonIntermediateRepresentation
@{
*/

namespace SpaceGameEngine::CommonIntermediateRepresentation
{
	struct InterfaceNotFoundError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("The interface is not found in given type.");
		static COMMON_INTERMEDIATE_REPRESENTATION_API bool Judge(bool found);
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API InterfaceContainer : public UncopyableAndUnmovable
	{
	public:
		template<typename T, typename... InterfaceTypes>
		friend class Interfaces;

		template<typename T, typename InterfaceType>
			requires std::derived_from<T, InterfaceType>
		inline bool HasInterface() const
		{
			return m_InterfaceTypeIds.Contains(GetTypeId<InterfaceType>());
		}

		template<typename T, typename InterfaceType>
			requires std::derived_from<T, InterfaceType>
		inline InterfaceType& GetInterface(T& obj)
		{
			auto iter = m_InterfaceTypeIds.Find(GetTypeId<InterfaceType>());
			SGE_ASSERT(InterfaceNotFoundError, iter != m_InterfaceTypeIds.GetEnd());
			return *reinterpret_cast<InterfaceType*>(reinterpret_cast<UInt64>(&obj) + iter->m_Second);
		}

		template<typename T, typename InterfaceType>
			requires std::derived_from<T, InterfaceType>
		inline const InterfaceType& GetInterface(const T& obj) const
		{
			auto iter = m_InterfaceTypeIds.Find(GetTypeId<InterfaceType>());
			SGE_ASSERT(InterfaceNotFoundError, iter != m_InterfaceTypeIds.GetConstEnd());
			return *reinterpret_cast<const InterfaceType*>(reinterpret_cast<UInt64>(&obj) + iter->m_Second);
		}

		template<typename T, typename InterfaceType>
			requires std::derived_from<T, InterfaceType>
		inline InterfaceType* QueryInterface(T& obj)
		{
			auto iter = m_InterfaceTypeIds.Find(GetTypeId<InterfaceType>());
			if (iter != m_InterfaceTypeIds.GetEnd())
				return reinterpret_cast<InterfaceType*>(reinterpret_cast<UInt64>(&obj) + iter->m_Second);
			else
				return nullptr;
		}

		template<typename T, typename InterfaceType>
			requires std::derived_from<T, InterfaceType>
		inline const InterfaceType* QueryInterface(const T& obj) const
		{
			auto iter = m_InterfaceTypeIds.Find(GetTypeId<InterfaceType>());
			if (iter != m_InterfaceTypeIds.GetConstEnd())
				return reinterpret_cast<const InterfaceType*>(reinterpret_cast<UInt64>(&obj) + iter->m_Second);
			else
				return nullptr;
		}

	private:
		template<typename T, typename InterfaceType>
		// requires std::derived_from<T, InterfaceType>
		inline void AddInterface()
		{
			m_InterfaceTypeIds.Insert(GetTypeId<InterfaceType>(), GetOffsetOfBase<T, InterfaceType>());
		}

	private:
		HashMap<UInt64, UInt64> m_InterfaceTypeIds;	   // type id -> offset
	};

	template<typename T, typename... InterfaceTypes>
	class Interfaces : public InterfaceTypes...
	{
	public:
		inline Interfaces()
		{
			(static_cast<InterfaceContainer*>(static_cast<T*>(this))->template AddInterface<T, InterfaceTypes>(), ...);
		}
	};
}

/*!
@}
*/