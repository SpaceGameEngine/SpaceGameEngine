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
#include "SGEStringForward.h"
#include "Error.h"
#include "MemoryManager.h"
#include "Concurrent/Thread.h"
#include <cstring>

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	template<typename T>
	struct CharTrait
	{
		inline static constexpr const bool IsMultipleByte = false;
	};

	struct UCS2Trait
	{
		inline static constexpr const bool IsMultipleByte = false;
	};

	struct UTF8Trait
	{
		inline static constexpr const bool IsMultipleByte = true;
	};

	template<typename T, typename Trait = CharTrait<T>, typename Allocator = DefaultAllocator>
	class StringImplement
	{
	public:
		enum class StringCategory : UInt8
		{
			Small = 0,
			Medium = 1,
			Large = 2
		};

		struct StorageRef
		{
			std::atomic<SizeType> m_Count;
			T m_pContent[1];

			inline StorageRef()
			{
				m_Count.store(0, std::memory_order::memory_order_release);
				m_pContent[0] = NULL;
			}

		public:
			/*!
			@brief create a new storageref, its all elements have been set to 0, its count is 1. 
			*/
			inline static T* Create(const SizeType size)
			{
				SGE_ASSERT(InvalidSizeError, size, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));
				StorageRef& re = *reinterpret_cast<StorageRef*>(new (Allocator::RawNew(sizeof(std::atomic<SizeType>) + size * sizeof(T), alignof(StorageRef))) StorageRef());
				memset(re.m_pContent, NULL, size * sizeof(T));
				re.m_Count.store(1, std::memory_order_release);
				return re.m_pContent;
			}

			/*!
			@brief create a new storageref, its all elements were copied from the given ptr, its count is 1. 
			*/
			inline static T* Create(const T* ptr, const SizeType size)
			{
				SGE_ASSERT(InvalidSizeError, size, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));
				SGE_ASSERT(NullPointerError, ptr);
				StorageRef& re = *reinterpret_cast<StorageRef*>(new (Allocator::RawNew(sizeof(std::atomic<SizeType>) + size * sizeof(T), alignof(StorageRef))) StorageRef());
				memcpy(re.m_pContent, ptr, size * sizeof(T));
				re.m_Count.store(1, std::memory_order_release);
				return re.m_pContent;
			}

			inline static void CountIncrease(const T* ptr)
			{
				SGE_ASSERT(NullPointerError, ptr);
				std::atomic<SizeType>* pcount = reinterpret_cast<std::atomic<SizeType>*>((AddressType)(ptr) - sizeof(std::atomic<SizeType>));
				pcount->fetch_add(1, std::memory_order_acq_rel);
			}

			inline static void CountDecrease(const T* ptr)
			{
				SGE_ASSERT(NullPointerError, ptr);
				std::atomic<SizeType>* pcount = reinterpret_cast<std::atomic<SizeType>*>((AddressType)(ptr) - sizeof(std::atomic<SizeType>));
				pcount->fetch_sub(1, std::memory_order_acq_rel);
			}

			inline static SizeType GetCount(const T* ptr)
			{
				SGE_ASSERT(NullPointerError, ptr);
				std::atomic<SizeType>* pcount = reinterpret_cast<std::atomic<SizeType>*>((AddressType)(ptr) - sizeof(std::atomic<SizeType>));
				return pcount->load(std::memory_order_acquire);
			}

			inline static bool TryRelease(const T* ptr, SizeType size)
			{
				SGE_ASSERT(InvalidSizeError, size, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));
				SGE_ASSERT(NullPointerError, ptr);
				std::atomic<SizeType>* pcount = reinterpret_cast<std::atomic<SizeType>*>((AddressType)(ptr) - sizeof(std::atomic<SizeType>));
				if (pcount->load(std::memory_order_acquire) == 1)
				{
					Allocator::RawDelete(pcount, sizeof(std::atomic<SizeType>) + size * sizeof(T), alignof(StorageRef));
					return true;
				}
				else
					return false;
			}
		};

		/*!
		@brief simple storage for the string, do not consider '\0'.
		*/
		class Storage
		{
		private:
			SizeType m_RealSize;
			union {
				struct
				{
					SizeType m_Size;
					T* m_pContent;
				};
				T m_Content[(sizeof(m_Size) + sizeof(m_pContent)) / sizeof(T)];
			};

		public:
			inline Storage()
				: m_RealSize(0)
			{
			}

			inline explicit Storage(const SizeType size)
				: m_RealSize(size), m_Size(size)
			{
				auto category = GetStringCategoryByRealSize(m_RealSize);
				if (category == StringCategory::Small)
				{
					memset(m_Content, NULL, sizeof(m_Content));
				}
				else
				{
					m_pContent = StorageRef::Create(m_RealSize);
				}
			}

			inline Storage(const T* ptr, const SizeType size)
				: m_RealSize(size), m_Size(size)
			{
				auto category = GetStringCategoryByRealSize(m_RealSize);
				if (category == StringCategory::Small)
				{
					memcpy(m_Content, ptr, size * sizeof(T));
				}
				else
				{
					m_pContent = StorageRef::Create(ptr, m_RealSize);
				}
			}

			inline ~Storage()
			{
				if (GetStringCategoryByRealSize(m_RealSize) != StringCategory::Small)
				{
					if (!StorageRef::TryRelease(m_pContent, m_RealSize))
						StorageRef::CountDecrease(m_pContent);
				}
			}

			inline Storage(const Storage& s)
			{
				m_RealSize = s.m_RealSize;
				m_Size = s.m_Size;
				auto category = GetStringCategoryByRealSize(s.m_RealSize);
				if (category == StringCategory::Small)
				{
					memcpy(m_Content, s.m_Content, m_RealSize * sizeof(T));
				}
				else if (category == StringCategory::Medium)
				{
					m_pContent = StorageRef::Create(s.m_pContent, m_RealSize);
				}
				else	//StringCategory::Large
				{
					StorageRef::CountIncrease(s.m_pContent);
					m_pContent = s.m_pContent;
				}
			}

			inline Storage(Storage&& s)
			{
				m_RealSize = s.m_RealSize;
				m_Size = s.m_Size;
				auto category = GetStringCategoryByRealSize(s.m_RealSize);
				if (category == StringCategory::Small)
				{
					memcpy(m_Content, s.m_Content, m_RealSize * sizeof(T));
				}
				else
				{
					m_pContent = s.m_pContent;
					s.m_RealSize = 0;
					s.m_Size = 0;
					s.m_pContent = nullptr;
				}
			}

			template<typename OtherAllocator = DefaultAllocator>
			inline Storage(const typename StringImplement<T, Trait, OtherAllocator>::Storage& s)
			{
				m_RealSize = s.GetRealSize();
				m_Size = s.GetSize();
				auto category = GetStringCategoryByRealSize(m_RealSize);
				if (category == StringCategory::Small)
				{
					memcpy(m_Content, s.GetData(), m_RealSize * sizeof(T));
				}
				else
				{
					m_pContent = StorageRef::Create(s.GetData(), m_RealSize);
				}
			}

			template<typename OtherAllocator = DefaultAllocator>
			inline Storage(typename StringImplement<T, Trait, OtherAllocator>::Storage&& s)
			{
				m_RealSize = s.GetRealSize();
				m_Size = s.GetSize();
				auto category = GetStringCategoryByRealSize(m_RealSize);
				if (category == StringCategory::Small)
				{
					memcpy(m_Content, s.GetData(), m_RealSize * sizeof(T));
				}
				else
				{
					m_pContent = StorageRef::Create(s.GetData(), m_RealSize);
				}
			}

			inline SizeType GetSize() const
			{
				auto category = GetStringCategoryByRealSize(m_RealSize);
				if (category == StringCategory::Small)
					return m_RealSize;
				else
					return m_Size;
			}

			inline SizeType GetRealSize() const
			{
				return m_RealSize;
			}

			inline T* GetData()
			{
				auto category = GetStringCategoryByRealSize(m_RealSize);
				if (category == StringCategory::Small)
					return m_Content;
				else
					return m_pContent;
			}

			inline const T* GetData() const
			{
				auto category = GetStringCategoryByRealSize(m_RealSize);
				if (category == StringCategory::Small)
					return m_Content;
				else
					return m_pContent;
			}
		};
		inline static constexpr StringCategory GetStringCategoryByRealSize(const SizeType size)
		{
			if (size > 255)
				return StringCategory::Large;
			else if (size > ((sizeof(Storage) - sizeof(SizeType)) / sizeof(T)))
				return StringCategory::Medium;
			else
				return StringCategory::Small;
		}

	public:
		/*StringImplement();
		StringImplement(const StringImplement& str);
		StringImplement(StringImplement&& str);

		StringImplement(const StdString& str);
		StringImplement(const T* pstr);

		~StringImplement();

		StringImplement& operator=(const StringImplement& str);
		StringImplement& operator=(StringImplement&& str);

		StringImplement& operator=(const StdString& str);
		StringImplement& operator=(const T* pstr);

		bool operator==(const StringImplement& str) const;
		bool operator!=(const StringImplement& str) const;
		bool operator==(const StdString& str) const;
		bool operator!=(const StdString& str) const;
		bool operator==(const T* pstr) const;
		bool operator!=(const T* pstr) const;

		StdString ToStdString() const;
		const T* ToCString() const;
		T* GetData();
		SizeType GetSize();*/

	private:
	};

	/*!
	@}
	*/
}