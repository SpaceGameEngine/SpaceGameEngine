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
#include "Container/ContainerConcept.hpp"
#include "Utility/Utility.hpp"
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
		using ValueType = T;
		inline static constexpr const bool IsMultipleByte = false;
		inline static constexpr const SizeType MaxValue = (1 << (sizeof(T) * 8)) - 1;
	};

	struct UCS2Trait
	{
		using ValueType = Char16;
		inline static constexpr const bool IsMultipleByte = false;
		inline static constexpr const SizeType MaxValue = (1 << (sizeof(Char16) * 8)) - 1;
	};

	struct UTF8Trait
	{
		using ValueType = char;
		inline static constexpr const bool IsMultipleByte = true;
		inline static constexpr const SizeType MaxValue = (1 << (sizeof(char) * 8)) - 1;
	};

	namespace StringImplement
	{
		enum class StringCategory : UInt8
		{
			Small = 0,
			Medium = 1,
			Large = 2
		};

		template<typename T, typename Allocator = DefaultAllocator>
		struct StorageRef
		{
			std::atomic<SizeType> m_Count;
			T m_Barrier;	//avoid reverse iterator for multi-byte char out of range
			T m_pContent[1];

			inline StorageRef()
			{
				m_Count.store(0, std::memory_order::memory_order_release);
				m_Barrier = 0;
				m_pContent[0] = NULL;
			}

		public:
			/*!
			@brief create a new storageref, its all elements have been set to 0, its count is 1. 
			*/
			inline static T* Create(const SizeType size)
			{
				SGE_ASSERT(InvalidSizeError, size, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));
				StorageRef& re = *reinterpret_cast<StorageRef*>(new (Allocator::RawNew(sizeof(std::atomic<SizeType>) + (size + 1) * sizeof(T), alignof(StorageRef))) StorageRef());
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
				StorageRef& re = *reinterpret_cast<StorageRef*>(new (Allocator::RawNew(sizeof(std::atomic<SizeType>) + (size + 1) * sizeof(T), alignof(StorageRef))) StorageRef());
				memcpy(re.m_pContent, ptr, size * sizeof(T));
				re.m_Count.store(1, std::memory_order_release);
				return re.m_pContent;
			}

			inline static void CountIncrease(const T* ptr)
			{
				SGE_ASSERT(NullPointerError, ptr);
				std::atomic<SizeType>* pcount = reinterpret_cast<std::atomic<SizeType>*>((AddressType)(ptr) - sizeof(std::atomic<SizeType>) - sizeof(T));
				pcount->fetch_add(1, std::memory_order_acq_rel);
			}

			inline static void CountDecrease(const T* ptr)
			{
				SGE_ASSERT(NullPointerError, ptr);
				std::atomic<SizeType>* pcount = reinterpret_cast<std::atomic<SizeType>*>((AddressType)(ptr) - sizeof(std::atomic<SizeType>) - sizeof(T));
				pcount->fetch_sub(1, std::memory_order_acq_rel);
			}

			inline static SizeType GetCount(const T* ptr)
			{
				SGE_ASSERT(NullPointerError, ptr);
				std::atomic<SizeType>* pcount = reinterpret_cast<std::atomic<SizeType>*>((AddressType)(ptr) - sizeof(std::atomic<SizeType>) - sizeof(T));
				return pcount->load(std::memory_order_acquire);
			}

			inline static bool TryRelease(const T* ptr, SizeType size)
			{
				SGE_ASSERT(InvalidSizeError, size, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));
				SGE_ASSERT(NullPointerError, ptr);
				std::atomic<SizeType>* pcount = reinterpret_cast<std::atomic<SizeType>*>((AddressType)(ptr) - sizeof(std::atomic<SizeType>) - sizeof(T));
				if (pcount->load(std::memory_order_acquire) == 1)
				{
					Allocator::RawDelete(pcount, sizeof(std::atomic<SizeType>) + (size + 1) * sizeof(T), alignof(StorageRef));
					return true;
				}
				else
					return false;
			}
		};

		template<typename T>
		inline static constexpr StringCategory GetStringCategoryByRealSize(const SizeType size);

		/*!
		@brief simple storage for the string, do not consider '\0'.
		*/
		template<typename T, typename Allocator = DefaultAllocator>
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
				T m_Content[(sizeof(SizeType) + sizeof(T*)) / sizeof(T)];
			};

		public:
			inline Storage()
				: m_RealSize(0)
			{
			}

			inline explicit Storage(const SizeType size)
				: m_RealSize(size), m_Size(size)
			{
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				if (category == StringCategory::Small)
				{
					memset(m_Content, NULL, sizeof(m_Content));
				}
				else
				{
					m_pContent = StorageRef<T, Allocator>::Create(m_RealSize);
				}
			}

			inline Storage(const T* ptr, const SizeType size)
				: m_RealSize(size), m_Size(size)
			{
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				if (category == StringCategory::Small)
				{
					memcpy(m_Content, ptr, size * sizeof(T));
				}
				else
				{
					m_pContent = StorageRef<T, Allocator>::Create(ptr, m_RealSize);
				}
			}

			inline ~Storage()
			{
				if (GetStringCategoryByRealSize<T>(m_RealSize) != StringCategory::Small)
				{
					if (!StorageRef<T, Allocator>::TryRelease(m_pContent, m_RealSize))
						StorageRef<T, Allocator>::CountDecrease(m_pContent);
				}
			}

			inline Storage(const Storage& s)
			{
				m_RealSize = s.m_RealSize;
				m_Size = s.m_Size;
				auto category = GetStringCategoryByRealSize<T>(s.m_RealSize);
				if (category == StringCategory::Small)
				{
					memcpy(m_Content, s.m_Content, m_RealSize * sizeof(T));
				}
				else if (category == StringCategory::Medium)
				{
					m_pContent = StorageRef<T, Allocator>::Create(s.m_pContent, m_RealSize);
				}
				else	//StringCategory::Large
				{
					StorageRef<T, Allocator>::CountIncrease(s.m_pContent);
					m_pContent = s.m_pContent;
				}
			}

			inline Storage(Storage&& s)
			{
				m_RealSize = s.m_RealSize;
				m_Size = s.m_Size;
				auto category = GetStringCategoryByRealSize<T>(s.m_RealSize);
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

			template<typename OtherAllocator>
			inline Storage(const Storage<T, OtherAllocator>& s)
			{
				m_RealSize = s.GetRealSize();
				m_Size = s.GetSize();
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				if (category == StringCategory::Small)
				{
					memcpy(m_Content, s.GetData(), m_RealSize * sizeof(T));
				}
				else
				{
					m_pContent = StorageRef<T, Allocator>::Create(s.GetData(), m_RealSize);
				}
			}

			template<typename OtherAllocator>
			inline Storage(Storage<T, OtherAllocator>&& s)
			{
				m_RealSize = s.GetRealSize();
				m_Size = s.GetSize();
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				if (category == StringCategory::Small)
				{
					memcpy(m_Content, s.GetData(), m_RealSize * sizeof(T));
				}
				else
				{
					m_pContent = StorageRef<T, Allocator>::Create(s.GetData(), m_RealSize);
				}
			}

			inline Storage& operator=(const Storage& s)
			{
				SGE_ASSERT(SelfAssignmentError, this, &s);
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				auto category_for_s = GetStringCategoryByRealSize<T>(s.m_RealSize);
				if (category == StringCategory::Small)
				{
					m_RealSize = s.m_RealSize;
					m_Size = s.m_Size;
					if (category_for_s == StringCategory::Small)
					{
						memcpy(m_Content, s.m_Content, m_RealSize * sizeof(T));
					}
					else if (category_for_s == StringCategory::Medium)
					{
						m_pContent = StorageRef<T, Allocator>::Create(s.m_pContent, m_RealSize);
					}
					else
					{
						StorageRef<T, Allocator>::CountIncrease(s.m_pContent);
						m_pContent = s.m_pContent;
					}
				}
				else if (category == StringCategory::Medium)
				{
					if (category_for_s == StringCategory::Small)
					{
						StorageRef<T, Allocator>::TryRelease(m_pContent, m_RealSize);
						m_RealSize = s.m_RealSize;
						memcpy(m_Content, s.m_Content, m_RealSize * sizeof(T));
					}
					else if (category_for_s == StringCategory::Medium)
					{
						if (m_RealSize >= s.m_RealSize)
						{
							//no need for re-allocate
							m_Size = s.m_Size;
							memcpy(m_pContent, s.m_pContent, m_Size * sizeof(T));
						}
						else
						{
							StorageRef<T, Allocator>::TryRelease(m_pContent, m_RealSize);
							m_RealSize = s.m_RealSize;
							m_Size = s.m_Size;
							m_pContent = StorageRef<T, Allocator>::Create(s.m_pContent, m_RealSize);
						}
					}
					else
					{
						StorageRef<T, Allocator>::TryRelease(m_pContent, m_RealSize);
						m_RealSize = s.m_RealSize;
						m_Size = s.m_Size;
						StorageRef<T, Allocator>::CountIncrease(s.m_pContent);
						m_pContent = s.m_pContent;
					}
				}
				else
				{
					if (!StorageRef<T, Allocator>::TryRelease(m_pContent, m_RealSize))
						StorageRef<T, Allocator>::CountDecrease(m_pContent);
					m_RealSize = s.m_RealSize;
					m_Size = s.m_Size;
					if (category_for_s == StringCategory::Small)
					{
						memcpy(m_Content, s.m_Content, m_RealSize * sizeof(T));
					}
					else if (category_for_s == StringCategory::Medium)
					{
						m_pContent = StorageRef<T, Allocator>::Create(s.m_pContent, m_RealSize);
					}
					else
					{
						StorageRef<T, Allocator>::CountIncrease(s.m_pContent);
						m_pContent = s.m_pContent;
					}
				}
				return *this;
			}

			inline Storage& operator=(Storage&& s)
			{
				SGE_ASSERT(SelfAssignmentError, this, &s);
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				auto category_for_s = GetStringCategoryByRealSize<T>(s.m_RealSize);
				if (category == StringCategory::Small)
				{
					m_RealSize = s.m_RealSize;
					m_Size = s.m_Size;
					if (category_for_s == StringCategory::Small)
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
				else if (category == StringCategory::Medium)
				{
					StorageRef<T, Allocator>::TryRelease(m_pContent, m_RealSize);
					m_RealSize = s.m_RealSize;
					m_Size = s.m_Size;
					if (category_for_s == StringCategory::Small)
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
				else
				{
					if (!StorageRef<T, Allocator>::TryRelease(m_pContent, m_RealSize))
						StorageRef<T, Allocator>::CountDecrease(m_pContent);
					m_RealSize = s.m_RealSize;
					m_Size = s.m_Size;
					if (category_for_s == StringCategory::Small)
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
				return *this;
			}

			template<typename OtherAllocator>
			inline Storage& operator=(const Storage<T, OtherAllocator>& s)
			{
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				auto category_for_s = GetStringCategoryByRealSize<T>(s.GetRealSize());
				if (category == StringCategory::Small)
				{
					m_RealSize = s.GetRealSize();
					m_Size = s.GetSize();
					if (category_for_s == StringCategory::Small)
					{
						memcpy(m_Content, s.GetData(), m_RealSize * sizeof(T));
					}
					else
					{
						m_pContent = StorageRef<T, Allocator>::Create(s.GetData(), m_RealSize);
					}
				}
				else if (category == StringCategory::Medium)
				{
					if (category_for_s == StringCategory::Small)
					{
						StorageRef<T, Allocator>::TryRelease(m_pContent, m_RealSize);
						m_RealSize = s.GetRealSize();
						memcpy(m_Content, s.GetData(), m_RealSize * sizeof(T));
					}
					else if (category_for_s == StringCategory::Medium)
					{
						if (m_RealSize >= s.GetRealSize())
						{
							//no need for re-allocate
							m_Size = s.GetSize();
							memcpy(m_pContent, s.GetData(), m_Size * sizeof(T));
						}
						else
						{
							StorageRef<T, Allocator>::TryRelease(m_pContent, m_RealSize);
							m_RealSize = s.GetRealSize();
							m_Size = s.GetSize();
							m_pContent = StorageRef<T, Allocator>::Create(s.GetData(), m_RealSize);
						}
					}
					else
					{
						StorageRef<T, Allocator>::TryRelease(m_pContent, m_RealSize);
						m_RealSize = s.GetRealSize();
						m_Size = s.GetSize();
						m_pContent = StorageRef<T, Allocator>::Create(s.GetData(), m_RealSize);
					}
				}
				else
				{
					if (!StorageRef<T, Allocator>::TryRelease(m_pContent, m_RealSize))
						StorageRef<T, Allocator>::CountDecrease(m_pContent);
					m_RealSize = s.GetRealSize();
					m_Size = s.GetSize();
					if (category_for_s == StringCategory::Small)
					{
						memcpy(m_Content, s.GetData(), m_RealSize * sizeof(T));
					}
					else
					{
						m_pContent = StorageRef<T, Allocator>::Create(s.GetData(), m_RealSize);
					}
				}
				return *this;
			}

			template<typename OtherAllocator>
			inline Storage& operator=(Storage<T, OtherAllocator>&& s)
			{
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				auto category_for_s = GetStringCategoryByRealSize<T>(s.GetRealSize());
				if (category == StringCategory::Small)
				{
					m_RealSize = s.GetRealSize();
					m_Size = s.GetSize();
					if (category_for_s == StringCategory::Small)
					{
						memcpy(m_Content, s.GetData(), m_RealSize * sizeof(T));
					}
					else
					{
						m_pContent = StorageRef<T, Allocator>::Create(s.GetData(), m_RealSize);
					}
				}
				else if (category == StringCategory::Medium)
				{
					if (category_for_s == StringCategory::Small)
					{
						StorageRef<T, Allocator>::TryRelease(m_pContent, m_RealSize);
						m_RealSize = s.GetRealSize();
						memcpy(m_Content, s.GetData(), m_RealSize * sizeof(T));
					}
					else if (category_for_s == StringCategory::Medium)
					{
						if (m_RealSize >= s.GetRealSize())
						{
							//no need for re-allocate
							m_Size = s.GetSize();
							memcpy(m_pContent, s.GetData(), m_Size * sizeof(T));
						}
						else
						{
							StorageRef<T, Allocator>::TryRelease(m_pContent, m_RealSize);
							m_RealSize = s.GetRealSize();
							m_Size = s.GetSize();
							m_pContent = StorageRef<T, Allocator>::Create(s.GetData(), m_RealSize);
						}
					}
					else
					{
						StorageRef<T, Allocator>::TryRelease(m_pContent, m_RealSize);
						m_RealSize = s.GetRealSize();
						m_Size = s.GetSize();
						m_pContent = StorageRef<T, Allocator>::Create(s.GetData(), m_RealSize);
					}
				}
				else
				{
					if (!StorageRef<T, Allocator>::TryRelease(m_pContent, m_RealSize))
						StorageRef<T, Allocator>::CountDecrease(m_pContent);
					m_RealSize = s.GetRealSize();
					m_Size = s.GetSize();
					if (category_for_s == StringCategory::Small)
					{
						memcpy(m_Content, s.GetData(), m_RealSize * sizeof(T));
					}
					else
					{
						m_pContent = StorageRef<T, Allocator>::Create(s.GetData(), m_RealSize);
					}
				}
				return *this;
			}

			/*!
			@brief set the real size for the storage.
			@warning when the new size's category is StringCategory::Small and the previous Stroage's
			category is not StringCategory::Small, not matter what new size is given, the final result
			is that the new real size will be equal to the m_Size.
			*/
			inline void SetRealSize(const SizeType size)
			{
				SGE_ASSERT(InvalidSizeError, size, GetSize(), SGE_MAX_MEMORY_SIZE / sizeof(T));
				if (m_RealSize == size)
					return;
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				auto new_category = GetStringCategoryByRealSize<T>(size);
				if (category == StringCategory::Small)
				{
					if (new_category == StringCategory::Small)
					{
						m_RealSize = size;
					}
					else
					{
						auto pbuf = StorageRef<T, Allocator>::Create(size);
						memcpy(pbuf, m_Content, m_RealSize * sizeof(T));
						m_pContent = pbuf;
						m_Size = m_RealSize;
						m_RealSize = size;
					}
				}
				else if (category == StringCategory::Medium)
				{
					if (new_category == StringCategory::Small)
					{
						//see warning
						auto pre_real_size = m_RealSize;
						auto pre_p_content = m_pContent;
						m_RealSize = m_Size;
						memcpy(m_Content, pre_p_content, m_RealSize * sizeof(T));
						StorageRef<T, Allocator>::TryRelease(pre_p_content, pre_real_size);
					}
					else
					{
						auto pbuf = StorageRef<T, Allocator>::Create(size);
						memcpy(pbuf, m_pContent, m_Size * sizeof(T));
						StorageRef<T, Allocator>::TryRelease(m_pContent, m_RealSize);
						m_pContent = pbuf;
						m_RealSize = size;
					}
				}
				else
				{
					if (new_category == StringCategory::Small)
					{
						//see warning
						auto pre_real_size = m_RealSize;
						auto pre_p_content = m_pContent;
						m_RealSize = m_Size;
						memcpy(m_Content, pre_p_content, m_RealSize * sizeof(T));
						if (!StorageRef<T, Allocator>::TryRelease(pre_p_content, pre_real_size))
							StorageRef<T, Allocator>::CountDecrease(pre_p_content);
					}
					else
					{
						auto pbuf = StorageRef<T, Allocator>::Create(size);
						memcpy(pbuf, m_pContent, m_Size * sizeof(T));
						if (!StorageRef<T, Allocator>::TryRelease(m_pContent, m_RealSize))
							StorageRef<T, Allocator>::CountDecrease(m_pContent);
						m_pContent = pbuf;
						m_RealSize = size;
					}
				}
			}

			inline void Clear()
			{
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				if (category == StringCategory::Small)
				{
					m_RealSize = 0;
				}
				else
				{
					if (!StorageRef<T, Allocator>::TryRelease(m_pContent, m_RealSize))
						StorageRef<T, Allocator>::CountDecrease(m_pContent);
					m_RealSize = 0;
					m_Size = 0;
					m_pContent = nullptr;
				}
			}

			inline void CopyOnWrite()
			{
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				if (category == StringCategory::Large)
				{
					if (StorageRef<T, Allocator>::GetCount(m_pContent) > 1)
					{
						auto ptr = StorageRef<T, Allocator>::Create(m_pContent, m_RealSize);
						StorageRef<T, Allocator>::CountDecrease(m_pContent);
						m_pContent = ptr;
					}
				}
			}

			inline SizeType GetSize() const
			{
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				if (category == StringCategory::Small)
					return m_RealSize;
				else
					return m_Size;
			}

			inline void SetSize(const SizeType size)
			{
				SGE_ASSERT(InvalidSizeError, size, 0, GetRealSize());
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				if (category == StringCategory::Small)
					m_RealSize = size;
				else
					m_Size = size;
			}

			inline SizeType GetRealSize() const
			{
				return m_RealSize;
			}

			inline T* GetData()
			{
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				if (category == StringCategory::Small)
					return m_Content;
				else
					return m_pContent;
			}

			inline const T* GetData() const
			{
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				if (category == StringCategory::Small)
					return m_Content;
				else
					return m_pContent;
			}
		};

		template<typename T>
		inline constexpr StringCategory GetStringCategoryByRealSize(const SizeType size)
		{
			if (size > 255)
				return StringCategory::Large;
			else if (size > ((sizeof(Storage<T>) - sizeof(SizeType)) / sizeof(T)))
				return StringCategory::Medium;
			else
				return StringCategory::Small;
		}

		template<typename T, typename Trait = CharTrait<T>>
		inline const T* GetNextMultipleByteChar(const T* ptr)
		{
			static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");
			static_assert(Trait::IsMultipleByte, "invalid trait : the trait is not multi-byte");
			SGE_ASSERT(NullPointerError, ptr);
			// need specialize for different situation
			return ptr + 1;
		}

		template<>
		inline const char* GetNextMultipleByteChar<char, UTF8Trait>(const char* ptr)
		{
			SGE_ASSERT(NullPointerError, ptr);
			if (static_cast<const UInt8>(*ptr) <= 0b01111111)
				return ptr + 1;
			else if (static_cast<const UInt8>(*ptr) <= 0b11011111)
				return ptr + 2;
			else if (static_cast<const UInt8>(*ptr) <= 0b11101111)
				return ptr + 3;
			else if (static_cast<const UInt8>(*ptr) <= 0b11110111)
				return ptr + 4;
		}

		template<typename T, typename Trait = CharTrait<T>>
		inline const T* GetPreviousMultipleByteChar(const T* ptr)
		{
			static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");
			static_assert(Trait::IsMultipleByte, "invalid trait : the trait is not multi-byte");
			SGE_ASSERT(NullPointerError, ptr);
			// need specialize for different situation
			return ptr - 1;
		}

		template<>
		inline const char* GetPreviousMultipleByteChar<char, UTF8Trait>(const char* ptr)
		{
			SGE_ASSERT(NullPointerError, ptr);
			do
			{
				ptr -= 1;
			} while ((static_cast<const UInt8>(*ptr) & 0b11000000) == 0b10000000);
			return ptr;
		}

		/*!
		@brief Get the multi-byte char's size(not the real memory size).
		*/
		template<typename T, typename Trait = CharTrait<T>>
		inline SizeType GetMultipleByteCharSize(const T* ptr)
		{
			static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");
			static_assert(Trait::IsMultipleByte, "invalid trait : the trait is not multi-byte");
			SGE_ASSERT(NullPointerError, ptr);
			// need specialize for different situation
			return 1;
		}

		template<>
		inline SizeType GetMultipleByteCharSize<char, UTF8Trait>(const char* ptr)
		{
			SGE_ASSERT(NullPointerError, ptr);
			if (static_cast<const UInt8>(*ptr) <= 0b01111111)
				return 1;
			else if (static_cast<const UInt8>(*ptr) <= 0b11011111)
				return 2;
			else if (static_cast<const UInt8>(*ptr) <= 0b11101111)
				return 3;
			else if (static_cast<const UInt8>(*ptr) <= 0b11110111)
				return 4;
		}

		template<typename T, typename Trait = CharTrait<T>>
		inline Int8 CompareMultipleByteChar(const T* ptr1, const T* ptr2)
		{
			static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");
			static_assert(Trait::IsMultipleByte, "invalid trait : the trait is not multi-byte");
			SGE_ASSERT(NullPointerError, ptr1);
			SGE_ASSERT(NullPointerError, ptr2);
			return 0;
		}

		inline Char16 UTF8CharToUCS2Char(const char* ptr)
		{
			SGE_ASSERT(NullPointerError, ptr);
			if (static_cast<const UInt8>(*ptr) <= 0b01111111)
				return *ptr;
			else if (static_cast<const UInt8>(*ptr) <= 0b11011111)
				return ((*(ptr + 1)) & 0b00111111) | (((*ptr) & 0b00011111) << 6);
			else if (static_cast<const UInt8>(*ptr) <= 0b11101111)
				return ((*(ptr + 2)) & 0b00111111) | (((*(ptr + 1)) & 0b00111111) << 6) | (((*ptr) & 0b00001111) << 12);
			else if (static_cast<const UInt8>(*ptr) <= 0b11110111)
				return ((*(ptr + 3)) & 0b00111111) | (((*(ptr + 2)) & 0b00111111) << 6) | (((*(ptr + 1)) & 0b00111111) << 12) | (((*ptr) & 0b00000111) << 18);	  //out of ucs2
		}

		inline SizeType GetUCS2CharToUTF8CharSize(const Char16 c)
		{
			if (c <= 0x7f)
				return 1;
			else if (c <= 0x7ff)
				return 2;
			else if (c <= 0xffff)
				return 3;
			else if (c <= 0x10ffff)	   //out of ucs2
				return 4;
		}

		/*!
		@brief Write the utf8 char to a existing memory according to the given ucs2 char.
		@return The next utf8 char's address.
		*/
		inline char* UCS2CharToUTF8Char(const Char16 c, char* pdst)
		{
			SGE_ASSERT(NullPointerError, pdst);
			if (c <= 0x7f)
			{
				*pdst = c;
				return pdst + 1;
			}
			else if (c <= 0x7ff)
			{
				*(pdst + 1) = 0b10000000 | (0b00111111 & c);
				*pdst = 0b11000000 | ((c >> 6) & 0b00011111);
				return pdst + 2;
			}
			else if (c <= 0xffff)
			{
				*(pdst + 2) = 0b10000000 | (0b00111111 & c);
				*(pdst + 1) = 0b10000000 | (0b00111111 & (c >> 6));
				*pdst = 0b11100000 | ((c >> 12) & 0b00001111);
				return pdst + 3;
			}
			else if (c <= 0x10ffff)	   //out of ucs2
			{
				*(pdst + 3) = 0b10000000 | (0b00111111 & c);
				*(pdst + 2) = 0b10000000 | (0b00111111 & (c >> 6));
				*(pdst + 1) = 0b10000000 | (0b00111111 & (c >> 12));
				*pdst = 0b11110000 | ((c >> 18) & 0b00000111);
				return pdst + 4;
			}
		}

		template<>
		inline Int8 CompareMultipleByteChar<char, UTF8Trait>(const char* ptr1, const char* ptr2)
		{
			SGE_ASSERT(NullPointerError, ptr1);
			SGE_ASSERT(NullPointerError, ptr2);
			Char16 c1 = UTF8CharToUCS2Char(ptr1);
			Char16 c2 = UTF8CharToUCS2Char(ptr2);
			if (c1 < c2)
				return -1;
			else if (c1 == c2)
				return 0;
			else if (c1 > c2)
				return 1;
		}

		template<typename T>
		struct MakeCharTypeUnsigned
		{
			using Type = T;
		};

		template<>
		struct MakeCharTypeUnsigned<char>
		{
			using Type = unsigned char;
		};

		namespace BoyerMooreSearchImplement
		{
			template<typename T, typename Trait = CharTrait<T>>
			inline void MakeBadCharTable(SizeType* pdst, const T* pstr, const SizeType nsize)
			{
				static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");
				SGE_ASSERT(NullPointerError, pdst);
				SGE_ASSERT(NullPointerError, pstr);
				SGE_ASSERT(InvalidSizeError, nsize, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));

				for (SizeType i = 0; i <= Trait::MaxValue; i++)
					pdst[i] = nsize;
				for (SizeType i = 0; i < nsize - 1; i++)
					pdst[(typename MakeCharTypeUnsigned<T>::Type)pstr[i]] = nsize - 1 - i;
			}

			template<typename T>
			inline void MakeSuffix(SizeType* pdst, const T* pstr, const SizeType nsize)
			{
				SGE_ASSERT(NullPointerError, pdst);
				SGE_ASSERT(NullPointerError, pstr);
				SGE_ASSERT(InvalidSizeError, nsize, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));
				pdst[nsize - 1] = nsize;
				SizeType last_end = nsize - 2;
				SizeType last_begin = nsize - 1;
				for (SizeType i = nsize - 2; i >= 0; --i)
				{
					if (i > last_end && pdst[i + nsize - 1 - last_begin] < i - last_end)
						pdst[i] = pdst[i + nsize - 1 - last_begin];
					else
					{
						if (i < last_end)
							last_end = i;
						last_begin = i;
						bool is_zero_pass = false;
						while (pstr[last_end] == pstr[last_end + nsize - 1 - last_begin])
						{
							if (last_end != 0)
								last_end--;
							else
							{
								is_zero_pass = true;
								break;
							}
						}
						pdst[i] = (last_begin - last_end) + (is_zero_pass ? 1 : 0);
					}
					if (i == 0)
						break;
				}
			}

			template<typename T>
			inline void MakeGoodSuffixTable(SizeType* pdst, const SizeType* psuff, const T* pstr, const SizeType nsize)
			{
				SGE_ASSERT(NullPointerError, pdst);
				SGE_ASSERT(NullPointerError, psuff);
				SGE_ASSERT(NullPointerError, pstr);
				SGE_ASSERT(InvalidSizeError, nsize, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));

				SizeType j = 0;
				for (SizeType i = 0; i < nsize; i++)
					pdst[i] = nsize;
				for (SizeType i = nsize - 1; i >= 0; i--)
				{
					if (psuff[i] == i + 1)
					{
						for (; j < nsize - 1 - i; j++)
						{
							if (pdst[j] == nsize)
								pdst[j] = nsize - 1 - i;
						}
					}
					if (i == 0)
						break;
				}

				for (SizeType i = 0; i < nsize - 1; i++)
				{
					pdst[nsize - 1 - psuff[i]] = nsize - 1 - i;
				}
			}

			template<typename T>
			inline const T* BoyerMooreSearch(const T* ptext_begin, const T* ptext_end, const T* ppat, const SizeType* pbct, const SizeType* pgst, const SizeType nsize)
			{
				SGE_ASSERT(NullPointerError, ptext_begin);
				SGE_ASSERT(NullPointerError, ptext_end);
				SGE_ASSERT(NullPointerError, ppat);
				SGE_ASSERT(NullPointerError, pbct);
				SGE_ASSERT(NullPointerError, pgst);
				SGE_ASSERT(InvalidSizeError, nsize, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));

				SizeType tsize = ptext_end - ptext_begin;
				SGE_ASSERT(InvalidSizeError, tsize, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));

				if (tsize < nsize)
				{
					return ptext_end;
				}
				else
				{
					SizeType i = 0;
					SizeType j = 0;
					while (j <= (tsize - nsize))
					{
						for (i = nsize - 1; i >= 0 && ppat[i] == ptext_begin[i + j]; --i)
						{
							if (i == 0)
							{
								return ptext_begin + j;
							}
						}
						if (pbct[(typename MakeCharTypeUnsigned<T>::Type)ptext_begin[i + j]] > (nsize - 1 - i))
							j += Max((pbct[(typename MakeCharTypeUnsigned<T>::Type)ptext_begin[i + j]] + 1 + i) - nsize, pgst[i]);
						else
							j += pgst[i];
					}
					return ptext_end;
				}
			}
		}
	}

	template<typename T, typename Trait = CharTrait<T>, typename Allocator = DefaultAllocator>
	class StringCore
	{
	public:
		using ValueType = std::conditional_t<Trait::IsMultipleByte, T*, T>;
		using ConstValueType = std::conditional_t<Trait::IsMultipleByte, const T*, const T>;
		using ValueTrait = Trait;

		inline static const constexpr SizeType sm_MaxSize = (SGE_MAX_MEMORY_SIZE / sizeof(T)) - 1;

		static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

		template<typename _T, typename _Trait, typename _Allocator>
		friend class StringCore;

		struct EmptyStringCoreError
		{
			inline static const TChar sm_pContent[] = SGE_TSTR("The StringCore is empty");
			inline static bool Judge(SizeType size)
			{
				return size == 0;
			}
		};

		/*!
		@brief Get c-style string 's size, and do not consider the \0 in the result.
		*/
		inline static SizeType GetCStringSize(const T* ptr)
		{
			SGE_ASSERT(NullPointerError, ptr);
			SizeType re = 0;
			if constexpr (!Trait::IsMultipleByte)
			{
				while ((*ptr) != static_cast<T>(0))
				{
					re += 1;
					ptr += 1;
				}
			}
			else
			{
				while ((*ptr) != static_cast<T>(0))
				{
					re += 1;
					ptr = StringImplement::GetNextMultipleByteChar<T, Trait>(ptr);
				}
			}
			return re;
		}

		/*!
		@brief Get c-style string 's size, and do not consider the multi-byte and \0 in the result.
		*/
		inline static SizeType GetCStringNormalSize(const T* ptr)
		{
			SGE_ASSERT(NullPointerError, ptr);
			SizeType re = 0;
			while ((*ptr) != static_cast<T>(0))
			{
				re += 1;
				ptr += 1;
			}
			return re;
		}

		inline StringCore()
			: m_Storage(1), m_Size(0)
		{
		}

		inline StringCore(const StringCore& str)
			: m_Storage(str.m_Storage), m_Size(str.m_Size)
		{
		}

		inline StringCore(StringCore&& str)
			: m_Storage(std::move(str.m_Storage)), m_Size(str.m_Size)
		{
		}

		template<typename OtherAllocator>
		inline StringCore(const StringCore<T, Trait, OtherAllocator>& str)
			: m_Storage(str.m_Storage), m_Size(str.m_Size)
		{
		}

		template<typename OtherAllocator>
		inline StringCore(StringCore<T, Trait, OtherAllocator>&& str)
			: m_Storage(std::move(str.m_Storage)), m_Size(str.m_Size)
		{
		}

		inline explicit StringCore(const T* ptr)
			: m_Storage(ptr, GetCStringNormalSize(ptr) + 1), m_Size(GetCStringSize(ptr))
		{
		}

		inline StringCore(const SizeType size, ConstValueType val)
			: m_Storage(1), m_Size(size)
		{
			if constexpr (!Trait::IsMultipleByte)
			{
				SetRealSize(size);
				m_Storage.SetSize(size + 1);
				*(GetData() + size) = 0;
				SizeType i = size;
				while (i--)
				{
					*(GetData() + i) = val;
				}
			}
			else
			{
				SGE_ASSERT(NullPointerError, val);
				auto csize = StringImplement::GetMultipleByteCharSize<T, Trait>(val);
				SetRealSize(size * csize);
				m_Storage.SetSize(size * csize + 1);
				*(GetData() + size * csize) = 0;
				SizeType i = size;
				while (i--)
				{
					memcpy(GetData() + i * csize, val, csize * sizeof(T));
				}
			}
		}

		/*!
		@brief Initialize the StringCore with the given iterator pair.
		@warning When the StringCore is multi-byte, the given iterator's value type
		must be the pointer to the StringCore's T(it means that the iterator's value type
		must be equal to the StringCore's ValueType).
		*/
		template<typename IteratorType, typename = std::enable_if_t<IsSequentialIterator<IteratorType>::Value, void>, typename = std::enable_if_t<std::is_same_v<decltype(new ValueType((ValueType)(IteratorValueType<IteratorType>())), true), bool>, void>>
		inline StringCore(const IteratorType& begin, const IteratorType& end)
			: m_Storage(1), m_Size(end - begin)
		{
			if constexpr (!Trait::IsMultipleByte)
			{
				SizeType size = end - begin;
				SetRealSize(size);
				m_Storage.SetSize(size + 1);
				*(GetData() + size) = 0;
				auto iter = begin;
				for (SizeType i = 0; i < size && iter != end; i++, ++iter)
				{
					*(GetData() + i) = *iter;
				}
			}
			else
			{
				SizeType nsize = 0;
				for (auto iter = begin; iter != end; ++iter)
				{
					nsize += StringImplement::GetMultipleByteCharSize<T, Trait>(*iter);
				}
				SetRealSize(nsize);
				m_Storage.SetSize(nsize + 1);
				*(GetData() + nsize) = 0;
				SizeType i = 0;
				for (auto iter = begin; iter != end; ++iter)
				{
					memcpy(GetData() + i, *iter, StringImplement::GetMultipleByteCharSize<T, Trait>(*iter) * sizeof(T));
					i += StringImplement::GetMultipleByteCharSize<T, Trait>(*iter);
				}
			}
		}

		inline StringCore& operator=(const StringCore& str)
		{
			SGE_ASSERT(SelfAssignmentError, this, &str);
			m_Storage = str.m_Storage;
			m_Size = str.m_Size;
			return *this;
		}

		inline StringCore& operator=(StringCore&& str)
		{
			SGE_ASSERT(SelfAssignmentError, this, &str);
			m_Storage = std::move(str.m_Storage);
			m_Size = str.m_Size;
			return *this;
		}

		template<typename OtherAllocator>
		inline StringCore& operator=(const StringCore<T, Trait, OtherAllocator>& str)
		{
			m_Storage = str.m_Storage;
			m_Size = str.m_Size;
			return *this;
		}

		template<typename OtherAllocator>
		inline StringCore& operator=(StringCore<T, Trait, OtherAllocator>&& str)
		{
			m_Storage = std::move(str.m_Storage);
			m_Size = str.m_Size;
			return *this;
		}

		inline StringCore& operator=(const T* ptr)
		{
			SGE_ASSERT(NullPointerError, ptr);
			SGE_ASSERT(SelfAssignmentError, GetData(), ptr);
			m_Storage = std::move(StringImplement::Storage<T, Allocator>(ptr, GetCStringNormalSize(ptr) + 1));
			m_Size = GetCStringSize(ptr);
			return *this;
		}

		inline SizeType GetSize() const
		{
			return m_Size;
		}

		inline T* GetData()
		{
			return m_Storage.GetData();
		}

		inline const T* GetData() const
		{
			return m_Storage.GetData();
		}

		inline bool operator==(const StringCore& str) const
		{
			if (m_Storage.GetSize() != str.m_Storage.GetSize())
				return false;
			else
				return memcmp(m_Storage.GetData(), str.m_Storage.GetData(), m_Storage.GetSize() * sizeof(T)) == 0;
		}

		inline bool operator!=(const StringCore& str) const
		{
			if (m_Storage.GetSize() != str.m_Storage.GetSize())
				return true;
			else
				return memcmp(m_Storage.GetData(), str.m_Storage.GetData(), m_Storage.GetSize() * sizeof(T)) != 0;
		}

		template<typename OtherAllocator>
		inline bool operator==(const StringCore<T, Trait, OtherAllocator>& str) const
		{
			if (m_Storage.GetSize() != str.m_Storage.GetSize())
				return false;
			else
				return memcmp(m_Storage.GetData(), str.m_Storage.GetData(), m_Storage.GetSize() * sizeof(T)) == 0;
		}

		template<typename OtherAllocator>
		inline bool operator!=(const StringCore<T, Trait, OtherAllocator>& str) const
		{
			if (m_Storage.GetSize() != str.m_Storage.GetSize())
				return true;
			else
				return memcmp(m_Storage.GetData(), str.m_Storage.GetData(), m_Storage.GetSize() * sizeof(T)) != 0;
		}

		inline bool operator==(const T* ptr) const
		{
			SGE_ASSERT(NullPointerError, ptr);
			auto size = GetCStringNormalSize(ptr) + 1;
			if (m_Storage.GetSize() != size)
				return false;
			else
				return memcmp(m_Storage.GetData(), ptr, m_Storage.GetSize() * sizeof(T)) == 0;
		}

		inline bool operator!=(const T* ptr) const
		{
			SGE_ASSERT(NullPointerError, ptr);
			auto size = GetCStringNormalSize(ptr) + 1;
			if (m_Storage.GetSize() != size)
				return true;
			else
				return memcmp(m_Storage.GetData(), ptr, m_Storage.GetSize() * sizeof(T)) != 0;
		}

		inline void Clear()
		{
			m_Storage = std::move(StringImplement::Storage<T, Allocator>(1));
			m_Size = 0;
		}

		/*!
		@brief Return the m_Storage's size which not consider the multi-byte and the \0.
		*/
		inline SizeType GetNormalSize() const
		{
			return m_Storage.GetSize() - 1;
		}

		/*!
		@brief Return the m_Storage's real size which not consider the multi-byte and the \0.
		*/
		inline SizeType GetRealSize() const
		{
			return m_Storage.GetRealSize() - 1;
		}

		/*!
		@brief Set the m_Storage's real size.
		@warning The parameter size should not consider the multi-byte and \0.
		*/
		inline void SetRealSize(const SizeType size)
		{
			SGE_ASSERT(InvalidSizeError, size, GetNormalSize(), sm_MaxSize);
			m_Storage.SetRealSize(size + 1);
		}

		template<typename _T>
		class ReverseIteratorImpl;

		template<typename _T>
		class IteratorImpl
		{
		public:
			struct OutOfRangeError
			{
				inline static const TChar sm_pContent[] = SGE_TSTR("The iterator is out of range.");
				inline static bool Judge(const IteratorImpl& iter, const std::remove_const_t<_T>* begin, const std::remove_const_t<_T>* end)
				{
					SGE_ASSERT(NullPointerError, begin);
					SGE_ASSERT(NullPointerError, end);
					return !(iter.m_pContent >= begin && iter.m_pContent <= end);
				}
			};

			using ValueType = std::conditional_t<Trait::IsMultipleByte, _T*, _T>;

		public:
			friend OutOfRangeError;
			friend StringCore;
			friend ReverseIteratorImpl<_T>;

			inline static IteratorImpl GetBegin(const StringCore& str)
			{
				return IteratorImpl(const_cast<_T*>(str.GetData()));
			}

			inline static IteratorImpl GetEnd(const StringCore& str)
			{
				return IteratorImpl(const_cast<_T*>(str.GetData()) + str.GetNormalSize());
			}

			inline IteratorImpl(const IteratorImpl& iter)
			{
				m_pContent = iter.m_pContent;
			}

			inline IteratorImpl& operator=(const IteratorImpl& iter)
			{
				SGE_ASSERT(SelfAssignmentError, this, &iter);
				m_pContent = iter.m_pContent;
				return *this;
			}

			inline IteratorImpl operator+(SizeType i) const
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					return IteratorImpl(m_pContent + i);
				}
				else
				{
					auto ptr = m_pContent;
					while (i--)
					{
						ptr = (decltype(ptr))StringImplement::GetNextMultipleByteChar<std::remove_const_t<_T>, Trait>(ptr);
					}
					return IteratorImpl(ptr);
				}
			}

			inline IteratorImpl& operator+=(SizeType i)
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					m_pContent += i;
				}
				else
				{
					while (i--)
					{
						m_pContent = (decltype(m_pContent))StringImplement::GetNextMultipleByteChar<std::remove_const_t<_T>, Trait>(m_pContent);
					}
				}
				return *this;
			}

			inline IteratorImpl& operator++()
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					m_pContent += 1;
				}
				else
				{
					m_pContent = (decltype(m_pContent))StringImplement::GetNextMultipleByteChar<std::remove_const_t<_T>, Trait>(m_pContent);
				}
				return *this;
			}

			inline const IteratorImpl operator++(int)
			{
				IteratorImpl re(*this);
				if constexpr (!Trait::IsMultipleByte)
				{
					m_pContent += 1;
				}
				else
				{
					m_pContent = (decltype(m_pContent))StringImplement::GetNextMultipleByteChar<std::remove_const_t<_T>, Trait>(m_pContent);
				}
				return re;
			}

			inline IteratorImpl operator-(SizeType i) const
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					return IteratorImpl(m_pContent - i);
				}
				else
				{
					auto ptr = m_pContent;
					while (i--)
					{
						ptr = (decltype(ptr))StringImplement::GetPreviousMultipleByteChar<std::remove_const_t<_T>, Trait>(ptr);
					}
					return IteratorImpl(ptr);
				}
			}

			inline IteratorImpl& operator-=(SizeType i)
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					m_pContent -= i;
				}
				else
				{
					while (i--)
					{
						m_pContent = (decltype(m_pContent))StringImplement::GetPreviousMultipleByteChar<std::remove_const_t<T>, Trait>(m_pContent);
					}
				}
				return *this;
			}

			inline IteratorImpl& operator--()
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					m_pContent -= 1;
				}
				else
				{
					m_pContent = (decltype(m_pContent))StringImplement::GetPreviousMultipleByteChar<std::remove_const_t<_T>, Trait>(m_pContent);
				}
				return *this;
			}

			inline const IteratorImpl operator--(int)
			{
				IteratorImpl re(*this);
				if constexpr (!Trait::IsMultipleByte)
				{
					m_pContent -= 1;
				}
				else
				{
					m_pContent = (decltype(m_pContent))StringImplement::GetPreviousMultipleByteChar<std::remove_const_t<_T>, Trait>(m_pContent);
				}
				return re;
			}

			inline SizeType operator-(const IteratorImpl& iter) const
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					return ((AddressType)m_pContent - (AddressType)iter.m_pContent) / sizeof(_T);
				}
				else
				{
					SizeType re = 0;
					auto ptr = iter.m_pContent;
					while (ptr != m_pContent)
					{
						ptr = (decltype(ptr))StringImplement::GetNextMultipleByteChar<std::remove_const_t<_T>, Trait>(ptr);
						re += 1;
					}
					return re;
				}
			}

			inline std::conditional_t<!Trait::IsMultipleByte, ValueType&, ValueType> operator*() const
			{
				if constexpr (!Trait::IsMultipleByte)
					return *m_pContent;
				else
					return m_pContent;
			}

			inline bool operator==(const IteratorImpl& iter) const
			{
				return m_pContent == iter.m_pContent;
			}

			inline bool operator!=(const IteratorImpl& iter) const
			{
				return m_pContent != iter.m_pContent;
			}

			inline _T* GetData()
			{
				return m_pContent;
			}

			inline _T* GetData() const
			{
				return m_pContent;
			}

		private:
			inline explicit IteratorImpl(_T* ptr)
			{
				SGE_ASSERT(NullPointerError, ptr);
				m_pContent = ptr;
			}

		private:
			_T* m_pContent;
		};

		template<typename _T>
		class ReverseIteratorImpl
		{
		public:
			struct OutOfRangeError
			{
				inline static const TChar sm_pContent[] = SGE_TSTR("The iterator is out of range.");
				inline static bool Judge(const ReverseIteratorImpl& iter, const std::remove_const_t<_T>* begin, const std::remove_const_t<_T>* end)
				{
					SGE_ASSERT(NullPointerError, begin);
					SGE_ASSERT(NullPointerError, end);
					return !(iter.m_pContent >= begin && iter.m_pContent <= end);
				}
			};

			using ValueType = std::conditional_t<Trait::IsMultipleByte, _T*, _T>;

		public:
			friend OutOfRangeError;
			friend StringCore;

			inline static ReverseIteratorImpl GetBegin(const StringCore& str)
			{
				if constexpr (!Trait::IsMultipleByte)
					return ReverseIteratorImpl(const_cast<_T*>(str.GetData()) + str.GetNormalSize() - 1);
				else
					return ReverseIteratorImpl(const_cast<_T*>(StringImplement::GetPreviousMultipleByteChar<std::remove_const_t<_T>, Trait>(str.GetData() + str.GetNormalSize())));
			}

			inline static ReverseIteratorImpl GetEnd(const StringCore& str)
			{
				if constexpr (!Trait::IsMultipleByte)
					return ReverseIteratorImpl(const_cast<_T*>(str.GetData()) - 1);
				else
					return ReverseIteratorImpl(const_cast<_T*>(StringImplement::GetPreviousMultipleByteChar<std::remove_const_t<_T>, Trait>(str.GetData())));
			}

			inline ReverseIteratorImpl(const ReverseIteratorImpl& iter)
			{
				m_pContent = iter.m_pContent;
			}

			inline ReverseIteratorImpl& operator=(const ReverseIteratorImpl& iter)
			{
				SGE_ASSERT(SelfAssignmentError, this, &iter);
				m_pContent = iter.m_pContent;
				return *this;
			}

			inline ReverseIteratorImpl operator+(SizeType i) const
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					return ReverseIteratorImpl(m_pContent - i);
				}
				else
				{
					auto ptr = m_pContent;
					while (i--)
					{
						ptr = (decltype(ptr))StringImplement::GetPreviousMultipleByteChar<std::remove_const_t<_T>, Trait>(ptr);
					}
					return ReverseIteratorImpl(ptr);
				}
			}

			inline ReverseIteratorImpl& operator+=(SizeType i)
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					m_pContent -= i;
				}
				else
				{
					while (i--)
					{
						m_pContent = (decltype(m_pContent))StringImplement::GetPreviousMultipleByteChar<std::remove_const_t<_T>, Trait>(m_pContent);
					}
				}
				return *this;
			}

			inline ReverseIteratorImpl& operator++()
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					m_pContent -= 1;
				}
				else
				{
					m_pContent = (decltype(m_pContent))StringImplement::GetPreviousMultipleByteChar<std::remove_const_t<_T>, Trait>(m_pContent);
				}
				return *this;
			}

			inline const ReverseIteratorImpl operator++(int)
			{
				ReverseIteratorImpl re(*this);
				if constexpr (!Trait::IsMultipleByte)
				{
					m_pContent -= 1;
				}
				else
				{
					m_pContent = (decltype(m_pContent))StringImplement::GetPreviousMultipleByteChar<std::remove_const_t<_T>, Trait>(m_pContent);
				}
				return re;
			}

			inline ReverseIteratorImpl operator-(SizeType i) const
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					return ReverseIteratorImpl(m_pContent + i);
				}
				else
				{
					auto ptr = m_pContent;
					while (i--)
					{
						ptr = (decltype(ptr))StringImplement::GetNextMultipleByteChar<std::remove_const_t<_T>, Trait>(ptr);
					}
					return ReverseIteratorImpl(ptr);
				}
			}

			inline ReverseIteratorImpl& operator-=(SizeType i)
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					m_pContent += i;
				}
				else
				{
					while (i--)
					{
						m_pContent = (decltype(m_pContent))StringImplement::GetNextMultipleByteChar<std::remove_const_t<T>, Trait>(m_pContent);
					}
				}
				return *this;
			}

			inline ReverseIteratorImpl& operator--()
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					m_pContent += 1;
				}
				else
				{
					m_pContent = (decltype(m_pContent))StringImplement::GetNextMultipleByteChar<std::remove_const_t<_T>, Trait>(m_pContent);
				}
				return *this;
			}

			inline const ReverseIteratorImpl operator--(int)
			{
				ReverseIteratorImpl re(*this);
				if constexpr (!Trait::IsMultipleByte)
				{
					m_pContent += 1;
				}
				else
				{
					m_pContent = (decltype(m_pContent))StringImplement::GetNextMultipleByteChar<std::remove_const_t<_T>, Trait>(m_pContent);
				}
				return re;
			}

			inline SizeType operator-(const ReverseIteratorImpl& iter) const
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					return ((AddressType)iter.m_pContent - (AddressType)m_pContent) / sizeof(_T);
				}
				else
				{
					SizeType re = 0;
					auto ptr = m_pContent;
					while (ptr != iter.m_pContent)
					{
						ptr = (decltype(ptr))StringImplement::GetNextMultipleByteChar<std::remove_const_t<_T>, Trait>(ptr);
						re += 1;
					}
					return re;
				}
			}

			inline std::conditional_t<!Trait::IsMultipleByte, ValueType&, ValueType> operator*() const
			{
				if constexpr (!Trait::IsMultipleByte)
					return *m_pContent;
				else
					return m_pContent;
			}

			inline bool operator==(const ReverseIteratorImpl& iter) const
			{
				return m_pContent == iter.m_pContent;
			}

			inline bool operator!=(const ReverseIteratorImpl& iter) const
			{
				return m_pContent != iter.m_pContent;
			}

			inline _T* GetData()
			{
				return m_pContent;
			}

			inline _T* GetData() const
			{
				return m_pContent;
			}

			operator IteratorImpl<_T>() const
			{
				return IteratorImpl<_T>(m_pContent);
			}

		private:
			inline explicit ReverseIteratorImpl(_T* ptr)
			{
				SGE_ASSERT(NullPointerError, ptr);
				m_pContent = ptr;
			}

		private:
			_T* m_pContent;
		};

		using Iterator = IteratorImpl<T>;
		using ConstIterator = IteratorImpl<const T>;
		using ReverseIterator = ReverseIteratorImpl<T>;
		using ConstReverseIterator = ReverseIteratorImpl<const T>;

		inline Iterator GetBegin()
		{
			return Iterator::GetBegin(*this);
		}

		inline Iterator GetEnd()
		{
			return Iterator::GetEnd(*this);
		}

		inline ConstIterator GetConstBegin() const
		{
			return ConstIterator::GetBegin(*this);
		}

		inline ConstIterator GetConstEnd() const
		{
			return ConstIterator::GetEnd(*this);
		}

		inline ReverseIterator GetReverseBegin()
		{
			return ReverseIterator::GetBegin(*this);
		}

		inline ReverseIterator GetReverseEnd()
		{
			return ReverseIterator::GetEnd(*this);
		}

		inline ConstReverseIterator GetConstReverseBegin() const
		{
			return ConstReverseIterator::GetBegin(*this);
		}

		inline ConstReverseIterator GetConstReverseEnd() const
		{
			return ConstReverseIterator::GetEnd(*this);
		}

		/*!
		@brief check the type to make sure that it is one of the StringCore's Iterator Types.
		*/
		template<typename IteratorType>
		struct IsStringCoreIterator
		{
			inline static constexpr const bool Value = std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator> || std::is_same_v<IteratorType, ReverseIterator> || std::is_same_v<IteratorType, ConstReverseIterator>;
		};

		inline StringCore& operator+=(const StringCore& str)
		{
			SizeType nsize = GetNormalSize() + str.GetNormalSize();
			//normal size can be changed by setrealsize when the string is small string.
			SizeType osize = GetNormalSize();
			if (GetRealSize() < nsize)
			{
				SetRealSize(nsize);
			}
			m_Storage.CopyOnWrite();
			memcpy(m_Storage.GetData() + osize, str.GetData(), (str.GetNormalSize() + 1) * sizeof(T));
			m_Size += str.GetSize();
			m_Storage.SetSize(nsize + 1);
			return *this;
		}

		template<typename OtherAllocator>
		inline StringCore& operator+=(const StringCore<T, Trait, OtherAllocator>& str)
		{
			SizeType nsize = GetNormalSize() + str.GetNormalSize();
			//normal size can be changed by setrealsize when the string is small string.
			SizeType osize = GetNormalSize();
			if (GetRealSize() < nsize)
			{
				SetRealSize(2 * nsize);
			}
			m_Storage.CopyOnWrite();
			memcpy(m_Storage.GetData() + osize, str.GetData(), (str.GetNormalSize() + 1) * sizeof(T));
			m_Size += str.GetSize();
			m_Storage.SetSize(nsize + 1);
			return *this;
		}

		inline StringCore& operator+=(const T c)
		{
			SizeType nsize = GetNormalSize() + 1;
			SizeType osize = GetNormalSize();
			if (GetRealSize() < nsize)
			{
				SetRealSize(2 * nsize);
			}
			m_Storage.CopyOnWrite();
			*(m_Storage.GetData() + osize) = c;
			*(m_Storage.GetData() + nsize) = 0;
			m_Size += 1;
			m_Storage.SetSize(nsize + 1);
			return *this;
		}

		inline StringCore& operator+=(const T* pstr)
		{
			SGE_ASSERT(NullPointerError, pstr);
			SizeType size = GetCStringNormalSize(pstr);
			SizeType nsize = GetNormalSize() + size;
			SizeType osize = GetNormalSize();
			if (GetRealSize() < nsize)
			{
				SetRealSize(2 * nsize);
			}
			m_Storage.CopyOnWrite();
			memcpy(m_Storage.GetData() + osize, pstr, (size + 1) * sizeof(T));
			m_Size += GetCStringSize(pstr);
			m_Storage.SetSize(nsize + 1);
			return *this;
		}

		inline StringCore operator+(const StringCore& str) const
		{
			StringCore re(*this);
			re += str;
			return re;
		}

		template<typename OtherAllocator>
		inline StringCore operator+(const StringCore<T, Trait, OtherAllocator>& str) const
		{
			StringCore re(*this);
			re += str;
			return re;
		}

		inline StringCore operator+(const T c) const
		{
			StringCore re(*this);
			re += c;
			return re;
		}

		inline StringCore operator+(const T* pstr) const
		{
			SGE_ASSERT(NullPointerError, pstr);
			StringCore re(*this);
			re += pstr;
			return re;
		}

		inline bool operator<(const StringCore& str) const
		{
			if constexpr (!Trait::IsMultipleByte)
			{
				for (SizeType i = 0; i < Min(GetSize(), str.GetSize()); i += 1)
				{
					const T& c = *(GetData() + i);
					const T& c2 = *(str.GetData() + i);
					if (c < c2)
						return true;
					else if (c2 < c)
						return false;
				}
				if (GetSize() < str.GetSize())
					return true;
				else
					return false;
			}
			else
			{
				auto iter = GetConstBegin();
				auto iter2 = str.GetConstBegin();
				for (; iter != GetConstEnd() && iter2 != str.GetConstEnd(); ++iter, ++iter2)
				{
					Int8 res = StringImplement::CompareMultipleByteChar<T, Trait>(*iter, *iter2);
					if (res < 0)
						return true;
					else if (res > 0)
						return false;
				}
				if (GetSize() < str.GetSize())
					return true;
				else
					return false;
			}
		}

		template<typename OtherAllocator>
		inline bool operator<(const StringCore<T, Trait, OtherAllocator>& str) const
		{
			if constexpr (!Trait::IsMultipleByte)
			{
				for (SizeType i = 0; i < Min(GetSize(), str.GetSize()); i += 1)
				{
					const T& c = *(GetData() + i);
					const T& c2 = *(str.GetData() + i);
					if (c < c2)
						return true;
					else if (c2 < c)
						return false;
				}
				if (GetSize() < str.GetSize())
					return true;
				else
					return false;
			}
			else
			{
				auto iter = GetConstBegin();
				auto iter2 = str.GetConstBegin();
				for (; iter != GetConstEnd() && iter2 != str.GetConstEnd(); ++iter, ++iter2)
				{
					Int8 res = StringImplement::CompareMultipleByteChar<T, Trait>(*iter, *iter2);
					if (res < 0)
						return true;
					else if (res > 0)
						return false;
				}
				if (GetSize() < str.GetSize())
					return true;
				else
					return false;
			}
		}

		inline bool operator<(const T* pstr) const
		{
			SGE_ASSERT(NullPointerError, pstr);

			SizeType size = GetCStringSize(pstr);
			if constexpr (!Trait::IsMultipleByte)
			{
				for (SizeType i = 0; i < Min(GetSize(), size); i += 1)
				{
					const T& c = *(GetData() + i);
					const T& c2 = *(pstr + i);
					if (c < c2)
						return true;
					else if (c2 < c)
						return false;
				}
				if (GetSize() < size)
					return true;
				else
					return false;
			}
			else
			{
				auto iter = GetConstBegin();
				auto ptr = pstr;
				for (SizeType i = 0; i < Min(GetSize(), size); i += 1, ++iter, ptr = StringImplement::GetNextMultipleByteChar<T, Trait>(ptr))
				{
					Int8 res = StringImplement::CompareMultipleByteChar<T, Trait>(*iter, ptr);
					if (res < 0)
						return true;
					else if (res > 0)
						return false;
				}
				if (GetSize() < size)
					return true;
				else
					return false;
			}
		}

		inline bool operator>(const StringCore& str) const
		{
			if constexpr (!Trait::IsMultipleByte)
			{
				for (SizeType i = 0; i < Min(GetSize(), str.GetSize()); i += 1)
				{
					const T& c = *(GetData() + i);
					const T& c2 = *(str.GetData() + i);
					if (c > c2)
						return true;
					else if (c2 > c)
						return false;
				}
				if (GetSize() > str.GetSize())
					return true;
				else
					return false;
			}
			else
			{
				auto iter = GetConstBegin();
				auto iter2 = str.GetConstBegin();
				for (; iter != GetConstEnd() && iter2 != str.GetConstEnd(); ++iter, ++iter2)
				{
					Int8 res = StringImplement::CompareMultipleByteChar<T, Trait>(*iter, *iter2);
					if (res > 0)
						return true;
					else if (res < 0)
						return false;
				}
				if (GetSize() > str.GetSize())
					return true;
				else
					return false;
			}
		}

		template<typename OtherAllocator>
		inline bool operator>(const StringCore<T, Trait, OtherAllocator>& str) const
		{
			if constexpr (!Trait::IsMultipleByte)
			{
				for (SizeType i = 0; i < Min(GetSize(), str.GetSize()); i += 1)
				{
					const T& c = *(GetData() + i);
					const T& c2 = *(str.GetData() + i);
					if (c > c2)
						return true;
					else if (c2 > c)
						return false;
				}
				if (GetSize() > str.GetSize())
					return true;
				else
					return false;
			}
			else
			{
				auto iter = GetConstBegin();
				auto iter2 = str.GetConstBegin();
				for (; iter != GetConstEnd() && iter2 != str.GetConstEnd(); ++iter, ++iter2)
				{
					Int8 res = StringImplement::CompareMultipleByteChar<T, Trait>(*iter, *iter2);
					if (res > 0)
						return true;
					else if (res < 0)
						return false;
				}
				if (GetSize() > str.GetSize())
					return true;
				else
					return false;
			}
		}

		inline bool operator>(const T* pstr) const
		{
			SGE_ASSERT(NullPointerError, pstr);

			SizeType size = GetCStringSize(pstr);
			if constexpr (!Trait::IsMultipleByte)
			{
				for (SizeType i = 0; i < Min(GetSize(), size); i += 1)
				{
					const T& c = *(GetData() + i);
					const T& c2 = *(pstr + i);
					if (c > c2)
						return true;
					else if (c2 > c)
						return false;
				}
				if (GetSize() > size)
					return true;
				else
					return false;
			}
			else
			{
				auto iter = GetConstBegin();
				auto ptr = pstr;
				for (SizeType i = 0; i < Min(GetSize(), size); i += 1, ++iter, ptr = StringImplement::GetNextMultipleByteChar<T, Trait>(ptr))
				{
					Int8 res = StringImplement::CompareMultipleByteChar<T, Trait>(*iter, ptr);
					if (res > 0)
						return true;
					else if (res < 0)
						return false;
				}
				if (GetSize() > size)
					return true;
				else
					return false;
			}
		}

		inline bool operator<=(const StringCore& str) const
		{
			return StringCore::operator<(str) || StringCore::operator==(str);
		}

		template<typename OtherAllocator>
		inline bool operator<=(const StringCore<T, Trait, OtherAllocator>& str) const
		{
			return StringCore::operator<(str) || StringCore::operator==(str);
		}

		inline bool operator<=(const T* pstr) const
		{
			return StringCore::operator<(pstr) || StringCore::operator==(pstr);
		}

		inline bool operator>=(const StringCore& str) const
		{
			return StringCore::operator>(str) || StringCore::operator==(str);
		}

		template<typename OtherAllocator>
		inline bool operator>=(const StringCore<T, Trait, OtherAllocator>& str) const
		{
			return StringCore::operator>(str) || StringCore::operator==(str);
		}

		inline bool operator>=(const T* pstr) const
		{
			return StringCore::operator>(pstr) || StringCore::operator==(pstr);
		}

		/*!
		@brief insert a StringCore to the StringCore before the iterator.
		@todo use concept instead of sfinae.
		@warning only support sequential iterator.
		@return Iterator pointing to the inserted StringCore's first char.
		*/
		template<typename IteratorType, typename = std::enable_if_t<IsStringCoreIterator<IteratorType>::Value, bool>>
		inline IteratorType Insert(const IteratorType& iter, const StringCore& str)
		{
			SGE_ASSERT(SelfAssignmentError, this, &str);
			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, (GetData()), (GetData()) + GetNormalSize());
				SizeType osize = GetNormalSize();
				if (iter == IteratorType::GetEnd(*this))
				{
					StringCore::operator+=(str);
					return IteratorType(GetData() + osize);
				}
				SizeType nsize = osize + str.GetNormalSize();
				SizeType index = ((AddressType)iter.GetData() - (AddressType)GetData()) / sizeof(T);
				if (GetRealSize() < nsize)
				{
					SetRealSize(2 * nsize);
				}
				m_Storage.CopyOnWrite();
				memmove(GetData() + index + str.GetNormalSize(), GetData() + index, (osize - index + 1) * sizeof(T));	 //include '\0'
				memcpy(GetData() + index, str.GetData(), str.GetNormalSize() * sizeof(T));
				m_Size += str.GetSize();
				m_Storage.SetSize(nsize + 1);
				return IteratorType(GetData() + index);
			}
			else	//reverse
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, (GetData() - 1), (GetData()) + GetNormalSize() - 1);
				}
				else
				{
					SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, (StringImplement::GetPreviousMultipleByteChar<T, Trait>(GetData())), (StringImplement::GetPreviousMultipleByteChar<T, Trait>(GetData() + GetNormalSize())));
				}
				SizeType osize = GetNormalSize();
				SizeType nsize = osize + str.GetNormalSize();
				SizeType index = ((AddressType)((iter - 1).GetData()) - (AddressType)GetData()) / sizeof(T);
				if (GetRealSize() < nsize)
				{
					SetRealSize(2 * nsize);
				}
				m_Storage.CopyOnWrite();
				if (index != osize)
					memmove(GetData() + index + str.GetNormalSize(), GetData() + index, (osize - index + 1) * sizeof(T));	 //include '\0'

				ReverseIterator i1(GetData() + index + str.GetNormalSize());
				auto i2 = str.GetConstBegin();
				if constexpr (!Trait::IsMultipleByte)
				{
					i1 += 1;
				}
				else
				{
					i1.m_pContent -= StringImplement::GetMultipleByteCharSize<T, Trait>(*i2);
				}
				while (i2 != str.GetConstEnd())
				{
					if constexpr (!Trait::IsMultipleByte)
					{
						*i1 = *i2;
						++i1;
						++i2;
					}
					else
					{
						memcpy(*i1, *i2, StringImplement::GetMultipleByteCharSize<T, Trait>(*i2) * sizeof(T));
						++i2;
						i1.m_pContent -= StringImplement::GetMultipleByteCharSize<T, Trait>(*i2);
					}
				}
				m_Size += str.GetSize();
				m_Storage.SetSize(nsize + 1);
				IteratorType re(GetData() + index + str.GetNormalSize());
				re += 1;
				return re;
			}
		}

		/*!
		@brief insert a StringCore to the StringCore before the iterator.
		@todo use concept instead of sfinae.
		@warning only support sequential iterator.
		@return Iterator pointing to the inserted StringCore's first char.
		*/
		template<typename IteratorType, typename OtherAllocator, typename = std::enable_if_t<IsStringCoreIterator<IteratorType>::Value, bool>>
		inline IteratorType Insert(const IteratorType& iter, const StringCore<T, Trait, OtherAllocator>& str)
		{
			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, (GetData()), (GetData()) + GetNormalSize());
				SizeType osize = GetNormalSize();
				if (iter == IteratorType::GetEnd(*this))
				{
					StringCore::operator+=(str);
					return IteratorType(GetData() + osize);
				}
				SizeType nsize = osize + str.GetNormalSize();
				SizeType index = ((AddressType)iter.GetData() - (AddressType)GetData()) / sizeof(T);
				if (GetRealSize() < nsize)
				{
					SetRealSize(2 * nsize);
				}
				m_Storage.CopyOnWrite();
				memmove(GetData() + index + str.GetNormalSize(), GetData() + index, (osize - index + 1) * sizeof(T));	 //include '\0'
				memcpy(GetData() + index, str.GetData(), str.GetNormalSize() * sizeof(T));
				m_Size += str.GetSize();
				m_Storage.SetSize(nsize + 1);
				return IteratorType(GetData() + index);
			}
			else	//reverse
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, (GetData() - 1), (GetData()) + GetNormalSize() - 1);
				}
				else
				{
					SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, (StringImplement::GetPreviousMultipleByteChar<T, Trait>(GetData())), (StringImplement::GetPreviousMultipleByteChar<T, Trait>(GetData() + GetNormalSize())));
				}
				SizeType osize = GetNormalSize();
				SizeType nsize = osize + str.GetNormalSize();
				SizeType index = ((AddressType)((iter - 1).GetData()) - (AddressType)GetData()) / sizeof(T);
				if (GetRealSize() < nsize)
				{
					SetRealSize(2 * nsize);
				}
				m_Storage.CopyOnWrite();
				if (index != osize)
					memmove(GetData() + index + str.GetNormalSize(), GetData() + index, (osize - index + 1) * sizeof(T));	 //include '\0'

				ReverseIterator i1(GetData() + index + str.GetNormalSize());
				auto i2 = str.GetConstBegin();
				if constexpr (!Trait::IsMultipleByte)
				{
					i1 += 1;
				}
				else
				{
					i1.m_pContent -= StringImplement::GetMultipleByteCharSize<T, Trait>(*i2);
				}
				while (i2 != str.GetConstEnd())
				{
					if constexpr (!Trait::IsMultipleByte)
					{
						*i1 = *i2;
						++i1;
						++i2;
					}
					else
					{
						memcpy(*i1, *i2, StringImplement::GetMultipleByteCharSize<T, Trait>(*i2) * sizeof(T));
						++i2;
						i1.m_pContent -= StringImplement::GetMultipleByteCharSize<T, Trait>(*i2);
					}
				}
				m_Size += str.GetSize();
				m_Storage.SetSize(nsize + 1);
				IteratorType re(GetData() + index + str.GetNormalSize());
				re += 1;
				return re;
			}
		}

		/*!
		@brief insert a CString to the StringCore before the iterator.
		@todo use concept instead of sfinae.
		@warning only support sequential iterator.
		@return Iterator pointing to the inserted CString's first char.
		*/
		template<typename IteratorType, typename = std::enable_if_t<IsStringCoreIterator<IteratorType>::Value, bool>>
		inline IteratorType Insert(const IteratorType& iter, const T* pstr)
		{
			SGE_ASSERT(NullPointerError, pstr);
			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, (GetData()), (GetData()) + GetNormalSize());
				SizeType osize = GetNormalSize();
				if (iter == IteratorType::GetEnd(*this))
				{
					StringCore::operator+=(pstr);
					return IteratorType(GetData() + osize);
				}
				SizeType snsize = GetCStringNormalSize(pstr);
				SizeType nsize = osize + snsize;
				SizeType index = ((AddressType)iter.GetData() - (AddressType)GetData()) / sizeof(T);
				if (GetRealSize() < nsize)
				{
					SetRealSize(2 * nsize);
				}
				m_Storage.CopyOnWrite();
				memmove(GetData() + index + snsize, GetData() + index, (osize - index + 1) * sizeof(T));	//include '\0'
				memcpy(GetData() + index, pstr, snsize * sizeof(T));
				m_Size += GetCStringSize(pstr);
				m_Storage.SetSize(nsize + 1);
				return IteratorType(GetData() + index);
			}
			else	//reverse
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, (GetData() - 1), (GetData()) + GetNormalSize() - 1);
				}
				else
				{
					SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, (StringImplement::GetPreviousMultipleByteChar<T, Trait>(GetData())), (StringImplement::GetPreviousMultipleByteChar<T, Trait>(GetData() + GetNormalSize())));
				}
				SizeType osize = GetNormalSize();
				SizeType snsize = GetCStringNormalSize(pstr);
				SizeType nsize = osize + snsize;
				SizeType index = ((AddressType)((iter - 1).GetData()) - (AddressType)GetData()) / sizeof(T);
				if (GetRealSize() < nsize)
				{
					SetRealSize(2 * nsize);
				}
				m_Storage.CopyOnWrite();
				if (index != osize)
					memmove(GetData() + index + snsize, GetData() + index, (osize - index + 1) * sizeof(T));	//include '\0'

				ReverseIterator i1(GetData() + index + snsize);
				const T* i2 = pstr;
				if constexpr (!Trait::IsMultipleByte)
				{
					i1 += 1;
				}
				else
				{
					i1.m_pContent -= StringImplement::GetMultipleByteCharSize<T, Trait>(i2);
				}
				while (*i2 != 0)
				{
					if constexpr (!Trait::IsMultipleByte)
					{
						*i1 = *i2;
						++i1;
						++i2;
					}
					else
					{
						memcpy(*i1, i2, StringImplement::GetMultipleByteCharSize<T, Trait>(i2) * sizeof(T));
						i2 = StringImplement::GetNextMultipleByteChar<T, Trait>(i2);
						i1.m_pContent -= StringImplement::GetMultipleByteCharSize<T, Trait>(i2);
					}
				}
				m_Size += GetCStringSize(pstr);
				m_Storage.SetSize(nsize + 1);
				IteratorType re(GetData() + index + snsize);
				re += 1;
				return re;
			}
		}

		/*!
		@brief insert the content between the given iterator pair to the StringCore before the iterator.
		@todo use concept instead of sfinae.
		@warning only support sequential iterator.
		@return Iterator pointing to the inserted content's first char.
		*/
		template<typename IteratorType, typename OtherIteratorType, typename = std::enable_if_t<IsStringCoreIterator<IteratorType>::Value, bool>, typename = std::enable_if_t<IsSequentialIterator<OtherIteratorType>::Value, void>>
		inline IteratorType Insert(const IteratorType& iter, const OtherIteratorType& begin, const OtherIteratorType& end)
		{
			SizeType snsize = 0;
			if constexpr (!Trait::IsMultipleByte)
			{
				snsize = end - begin;
			}
			else
			{
				for (auto pbuf = begin; pbuf != end; ++pbuf)
				{
					snsize += StringImplement::GetMultipleByteCharSize<T, Trait>(*pbuf);
				}
			}
			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, (GetData()), (GetData()) + GetNormalSize());
				SizeType osize = GetNormalSize();
				SizeType nsize = osize + snsize;
				SizeType index = ((AddressType)iter.GetData() - (AddressType)GetData()) / sizeof(T);
				if (GetRealSize() < nsize)
				{
					SetRealSize(2 * nsize);
				}
				m_Storage.CopyOnWrite();
				memmove(GetData() + index + snsize, GetData() + index, (osize - index + 1) * sizeof(T));	//include '\0'
				SizeType cnt = 0;
				for (auto i = begin; i != end; ++i)
				{
					if constexpr (!Trait::IsMultipleByte)
					{
						*(GetData() + index + cnt) = *i;
						++cnt;
					}
					else
					{
						memcpy(GetData() + index + cnt, *i, StringImplement::GetMultipleByteCharSize<T, Trait>(*i) * sizeof(T));
						cnt += StringImplement::GetMultipleByteCharSize<T, Trait>(*i);
					}
				}
				m_Size += (end - begin);
				m_Storage.SetSize(nsize + 1);
				return IteratorType(GetData() + index);
			}
			else	//reverse
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, (GetData() - 1), (GetData()) + GetNormalSize() - 1);
				}
				else
				{
					SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, (StringImplement::GetPreviousMultipleByteChar<T, Trait>(GetData())), (StringImplement::GetPreviousMultipleByteChar<T, Trait>(GetData() + GetNormalSize())));
				}
				SizeType osize = GetNormalSize();
				SizeType nsize = osize + snsize;
				SizeType index = ((AddressType)((iter - 1).GetData()) - (AddressType)GetData()) / sizeof(T);
				if (GetRealSize() < nsize)
				{
					SetRealSize(2 * nsize);
				}
				m_Storage.CopyOnWrite();
				if (index != osize)
					memmove(GetData() + index + snsize, GetData() + index, (osize - index + 1) * sizeof(T));	//include '\0'

				ReverseIterator i1(GetData() + index + snsize);
				auto i2 = begin;
				if constexpr (!Trait::IsMultipleByte)
				{
					i1 += 1;
				}
				else
				{
					i1.m_pContent -= StringImplement::GetMultipleByteCharSize<T, Trait>(*i2);
				}
				while (i2 != end)
				{
					if constexpr (!Trait::IsMultipleByte)
					{
						*i1 = *i2;
						++i1;
						++i2;
					}
					else
					{
						memcpy(*i1, *i2, StringImplement::GetMultipleByteCharSize<T, Trait>(*i2) * sizeof(T));
						++i2;
						i1.m_pContent -= StringImplement::GetMultipleByteCharSize<T, Trait>(*i2);
					}
				}
				m_Size += (end - begin);
				m_Storage.SetSize(nsize + 1);
				IteratorType re(GetData() + index + snsize);
				re += 1;
				return re;
			}
		}

		/*!
		@brief remove a element in StringCore by giving a iterator.
		@todo use concept instead of sfinae.
		@warning only support sequential iterator.
		@return the iterator which points to the next element after the removing has been done.
		*/
		template<typename IteratorType, typename = std::enable_if_t<IsStringCoreIterator<IteratorType>::Value, bool>>
		inline IteratorType Remove(const IteratorType& iter)
		{
			SGE_ASSERT(EmptyStringCoreError, m_Size);
			if constexpr (!Trait::IsMultipleByte)
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, GetData(), GetData() + GetNormalSize() - 1);
			}
			else
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, GetData(), StringImplement::GetPreviousMultipleByteChar<T, Trait>(GetData() + GetNormalSize()));
			}
			SizeType nsize = GetNormalSize();
			if constexpr (!Trait::IsMultipleByte)
			{
				nsize -= 1;
			}
			else
			{
				nsize -= StringImplement::GetMultipleByteCharSize<T, Trait>(iter.GetData());
			}
			Iterator ni((T*)iter.GetData());
			ni += 1;
			m_Storage.CopyOnWrite();
			memmove((void*)iter.GetData(), ni.GetData(), (GetData() + GetNormalSize() - ni.GetData() + 1) * sizeof(T));
			m_Size -= 1;
			m_Storage.SetSize(nsize + 1);
			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				return iter;
			}
			else	//reverse
			{
				return iter + 1;
			}
		}

		/*!
		@brief remove some elements in StringCore by giving the iterators.
		@todo use concept instead of sfinae.
		@warning only support sequential iterator.
		@warning the begin&end's Container must be current StringCore.
		@return the iterator which points to the next element after the removing has been done.
		*/
		template<typename IteratorType, typename = std::enable_if_t<IsStringCoreIterator<IteratorType>::Value, bool>>
		inline IteratorType Remove(const IteratorType& begin, const IteratorType& end)
		{
			SGE_ASSERT(EmptyStringCoreError, m_Size);
			SizeType size = end - begin;
			SGE_ASSERT(InvalidSizeError, size, 1, m_Size);
			SizeType nsize = GetNormalSize();
			if constexpr (!Trait::IsMultipleByte)
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, begin, GetData(), GetData() + GetNormalSize() - 1);
				nsize -= size;
			}
			else
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, begin, GetData(), StringImplement::GetPreviousMultipleByteChar<T, Trait>(GetData() + GetNormalSize()));
				for (auto i = begin; i != end; ++i)
				{
					nsize -= StringImplement::GetMultipleByteCharSize<T, Trait>(*i);
				}
			}
			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, end, GetData(), GetData() + GetNormalSize());
				m_Storage.CopyOnWrite();
				memcpy((void*)begin.GetData(), end.GetData(), (GetData() + GetNormalSize() - end.GetData() + 1) * sizeof(T));
				m_Size -= size;
				m_Storage.SetSize(nsize + 1);
				return begin;
			}
			else	//reverse
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					SGE_ASSERT(typename IteratorType::OutOfRangeError, end, (GetData() - 1), (GetData()) + GetNormalSize() - 1);
				}
				else
				{
					SGE_ASSERT(typename IteratorType::OutOfRangeError, end, (StringImplement::GetPreviousMultipleByteChar<T, Trait>(GetData())), (StringImplement::GetPreviousMultipleByteChar<T, Trait>(GetData() + GetNormalSize())));
				}
				m_Storage.CopyOnWrite();
				auto end2 = end - 1;
				auto begin2 = begin - 1;
				memcpy((void*)end2.GetData(), begin2.GetData(), (GetData() + GetNormalSize() - begin2.GetData() + 1) * sizeof(T));
				m_Size -= size;
				m_Storage.SetSize(nsize + 1);
				return end;
			}
		}

		inline std::conditional_t<!Trait::IsMultipleByte, T&, T*> operator[](const SizeType i)
		{
			SGE_ASSERT(InvalidSizeError, i, 0, m_Size - 1);
			m_Storage.CopyOnWrite();
			if constexpr (!Trait::IsMultipleByte)
			{
				return *(GetData() + i);
			}
			else
			{
				return *(GetBegin() + i);
			}
		}

		inline std::conditional_t<!Trait::IsMultipleByte, T, const T*> operator[](const SizeType i) const
		{
			SGE_ASSERT(InvalidSizeError, i, 0, m_Size - 1);
			if constexpr (!Trait::IsMultipleByte)
			{
				return *(GetData() + i);
			}
			else
			{
				return *(GetConstBegin() + i);
			}
		}

		inline Iterator Find(const StringCore& str, const Iterator& begin, const Iterator& end)
		{
			SGE_ASSERT(typename Iterator::OutOfRangeError, begin, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(typename Iterator::OutOfRangeError, end, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(InvalidSizeError, end - begin, 1, m_Size);

			SizeType nsize = str.GetNormalSize();
			SGE_ASSERT(InvalidSizeError, nsize, 1, GetNormalSize());
			const T* pstr = str.GetData();

			SizeType* pbct = (SizeType*)Allocator::RawNew((Trait::MaxValue + 1) * sizeof(SizeType), alignof(SizeType));
			SizeType* psuff = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));
			SizeType* pgst = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));

			StringImplement::BoyerMooreSearchImplement::MakeBadCharTable<T, Trait>(pbct, pstr, nsize);
			StringImplement::BoyerMooreSearchImplement::MakeSuffix(psuff, pstr, nsize);
			StringImplement::BoyerMooreSearchImplement::MakeGoodSuffixTable(pgst, psuff, pstr, nsize);
			T* res = (T*)(StringImplement::BoyerMooreSearchImplement::BoyerMooreSearch(begin.GetData(), end.GetData(), pstr, pbct, pgst, nsize));

			Allocator::RawDelete(pbct, (Trait::MaxValue + 1) * sizeof(SizeType), alignof(SizeType));
			Allocator::RawDelete(psuff, nsize * sizeof(SizeType), alignof(SizeType));
			Allocator::RawDelete(pgst, nsize * sizeof(SizeType), alignof(SizeType));

			return Iterator(res);
		}

		inline ConstIterator Find(const StringCore& str, const ConstIterator& begin, const ConstIterator& end) const
		{
			SGE_ASSERT(typename ConstIterator::OutOfRangeError, begin, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(typename ConstIterator::OutOfRangeError, end, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(InvalidSizeError, end - begin, 1, m_Size);

			SizeType nsize = str.GetNormalSize();
			SGE_ASSERT(InvalidSizeError, nsize, 1, GetNormalSize());
			const T* pstr = str.GetData();

			SizeType* pbct = (SizeType*)Allocator::RawNew((Trait::MaxValue + 1) * sizeof(SizeType), alignof(SizeType));
			SizeType* psuff = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));
			SizeType* pgst = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));

			StringImplement::BoyerMooreSearchImplement::MakeBadCharTable<T, Trait>(pbct, pstr, nsize);
			StringImplement::BoyerMooreSearchImplement::MakeSuffix(psuff, pstr, nsize);
			StringImplement::BoyerMooreSearchImplement::MakeGoodSuffixTable(pgst, psuff, pstr, nsize);
			const T* res = StringImplement::BoyerMooreSearchImplement::BoyerMooreSearch(begin.GetData(), end.GetData(), pstr, pbct, pgst, nsize);

			Allocator::RawDelete(pbct, (Trait::MaxValue + 1) * sizeof(SizeType), alignof(SizeType));
			Allocator::RawDelete(psuff, nsize * sizeof(SizeType), alignof(SizeType));
			Allocator::RawDelete(pgst, nsize * sizeof(SizeType), alignof(SizeType));

			return ConstIterator(res);
		}

	private:
		StringImplement::Storage<T, Allocator> m_Storage;
		SizeType m_Size;
	};

	using UCS2String = StringCore<Char16, UCS2Trait, DefaultAllocator>;
	using UTF8String = StringCore<char, UTF8Trait, DefaultAllocator>;
	using String = UCS2String;
	/*!
	@}
	*/
}