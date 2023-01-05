/*
Copyright 2023 creatorlxd

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
#include "MetaData.h"
#include "MemoryManager.h"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{

	/*!
	@brief a wrapper of object which erases its type.
	*/
	template<typename Allocator = DefaultAllocator>
	class MetaObject
	{
	public:
		struct ImproperMetaDataError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The MetaData is improper for the MetaObject");
			inline static bool Judge(const MetaData& meta_data)
			{
				return !(meta_data.m_pName != nullptr && InvalidValueError::Judge(meta_data.m_Size, 1, SGE_MAX_MEMORY_SIZE) == false && ((meta_data.m_Alignment & (meta_data.m_Alignment - 1)) == 0 && meta_data.m_Alignment != 0) && meta_data.m_pDestructor != nullptr);
			}
		};

		struct DifferentMetaDataError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The two MetaDatas are different");
			inline static bool Judge(const MetaData& meta_data1, const MetaData& meta_data2)
			{
				return !(meta_data1 == meta_data2);
			}
		};

		using AllocatorType = Allocator;

	public:
		template<typename _Allocator>
		friend class MetaObject;

		inline MetaObject() = delete;
		inline ~MetaObject()
		{
			m_pMetaData->m_pDestructor(m_pContent);
			Allocator::RawDelete(m_pContent);
		}

		inline MetaObject(const MetaObject& obj)
		{
			SGE_ASSERT(NullPointerError, obj.m_pMetaData->m_pCopyConstructor);
			m_pMetaData = obj.m_pMetaData;
			m_pContent = Allocator::RawNew(m_pMetaData->m_Size, m_pMetaData->m_Alignment);
			m_pMetaData->m_pCopyConstructor(m_pContent, obj.m_pContent);
		}
		inline MetaObject(MetaObject&& obj)
		{
			SGE_ASSERT(NullPointerError, obj.m_pMetaData->m_pMoveConstructor);
			m_pMetaData = obj.m_pMetaData;
			m_pContent = Allocator::RawNew(m_pMetaData->m_Size, m_pMetaData->m_Alignment);
			m_pMetaData->m_pMoveConstructor(m_pContent, obj.m_pContent);
		}
		inline MetaObject& operator=(const MetaObject& obj)
		{
			SGE_ASSERT(SelfAssignmentError, this, &obj);
			SGE_ASSERT(NullPointerError, obj.m_pMetaData->m_pCopyAssignment);
			SGE_ASSERT(DifferentMetaDataError, *m_pMetaData, *obj.m_pMetaData);
			m_pMetaData->m_pCopyAssignment(m_pContent, obj.m_pContent);
			return *this;
		}
		inline MetaObject& operator=(MetaObject&& obj)
		{
			SGE_ASSERT(SelfAssignmentError, this, &obj);
			SGE_ASSERT(NullPointerError, obj.m_pMetaData->m_pMoveAssignment);
			SGE_ASSERT(DifferentMetaDataError, *m_pMetaData, *obj.m_pMetaData);
			m_pMetaData->m_pMoveAssignment(m_pContent, obj.m_pContent);
			return *this;
		}

		template<typename OtherAllocator>
		inline MetaObject(const MetaObject<OtherAllocator>& obj)
		{
			SGE_ASSERT(NullPointerError, obj.m_pMetaData->m_pCopyConstructor);
			m_pMetaData = obj.m_pMetaData;
			m_pContent = Allocator::RawNew(m_pMetaData->m_Size, m_pMetaData->m_Alignment);
			m_pMetaData->m_pCopyConstructor(m_pContent, obj.m_pContent);
		}
		template<typename OtherAllocator>
		inline MetaObject(MetaObject<OtherAllocator>&& obj)
		{
			SGE_ASSERT(NullPointerError, obj.m_pMetaData->m_pMoveConstructor);
			m_pMetaData = obj.m_pMetaData;
			m_pContent = Allocator::RawNew(m_pMetaData->m_Size, m_pMetaData->m_Alignment);
			m_pMetaData->m_pMoveConstructor(m_pContent, obj.m_pContent);
		}
		template<typename OtherAllocator>
		inline MetaObject& operator=(const MetaObject<OtherAllocator>& obj)
		{
			SGE_ASSERT(NullPointerError, obj.m_pMetaData->m_pCopyAssignment);
			SGE_ASSERT(DifferentMetaDataError, *m_pMetaData, *obj.m_pMetaData);
			m_pMetaData->m_pCopyAssignment(m_pContent, obj.m_pContent);
			return *this;
		}
		template<typename OtherAllocator>
		inline MetaObject& operator=(MetaObject<OtherAllocator>&& obj)
		{
			SGE_ASSERT(NullPointerError, obj.m_pMetaData->m_pMoveAssignment);
			SGE_ASSERT(DifferentMetaDataError, *m_pMetaData, *obj.m_pMetaData);
			m_pMetaData->m_pMoveAssignment(m_pContent, obj.m_pContent);
			return *this;
		}

		/*!
		@todo add support for custom constructors
		*/
		explicit inline MetaObject(const MetaData& meta_data)
		{
			SGE_ASSERT(ImproperMetaDataError, meta_data);
			SGE_ASSERT(NullPointerError, meta_data.m_pDefaultConstructor);
			m_pMetaData = &meta_data;
			m_pContent = Allocator::RawNew(m_pMetaData->m_Size, m_pMetaData->m_Alignment);
			m_pMetaData->m_pDefaultConstructor(m_pContent);
		}
		template<typename T>
		inline MetaObject(const MetaData& meta_data, T&& obj)
		{
			SGE_ASSERT(ImproperMetaDataError, meta_data);
			SGE_ASSERT(DifferentMetaDataError, meta_data, SpaceGameEngine::GetMetaData<std::decay_t<T>>());
			m_pMetaData = &meta_data;
			m_pContent = Allocator::RawNew(m_pMetaData->m_Size, m_pMetaData->m_Alignment);
			if constexpr (std::is_same_v<std::remove_reference_t<T>, T>)
			{
				SGE_ASSERT(NullPointerError, m_pMetaData->m_pMoveConstructor);
				m_pMetaData->m_pMoveConstructor(m_pContent, &obj);
			}
			else
			{
				SGE_ASSERT(NullPointerError, m_pMetaData->m_pCopyConstructor);
				m_pMetaData->m_pCopyConstructor(m_pContent, &obj);
			}
		}

		inline const MetaData& GetMetaData() const
		{
			return *m_pMetaData;
		}
		inline void* GetData()
		{
			return m_pContent;
		}
		inline const void* GetData() const
		{
			return m_pContent;
		}

		/*!
		@todo add support for the conversion between base class object and derived class object.
		*/
		template<typename T>
		inline T& Get()
		{
			SGE_ASSERT(DifferentMetaDataError, *m_pMetaData, SpaceGameEngine::GetMetaData<std::decay_t<T>>());
			return *reinterpret_cast<T*>(m_pContent);
		}
		template<typename T>
		inline const T& Get() const
		{
			SGE_ASSERT(DifferentMetaDataError, *m_pMetaData, SpaceGameEngine::GetMetaData<std::decay_t<T>>());
			return *reinterpret_cast<T*>(m_pContent);
		}

	private:
		const MetaData* m_pMetaData;
		void* m_pContent;
	};

	template<typename Allocator1, typename Allocator2>
	inline bool operator==(const MetaObject<Allocator1>& m1, const MetaObject<Allocator2>& m2)
	{
		if (m1.GetMetaData() == m2.GetMetaData())
		{
			SGE_ASSERT(NullPointerError, m1.GetMetaData().m_pComparison);
			return m1.GetMetaData().m_pComparison(m1.GetData(), m2.GetData());
		}
		else
			return false;
	}

}

/*!
@}
*/