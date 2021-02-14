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
		using ValueType = T;
		inline static constexpr const bool IsMultipleByte = false;
	};

	struct UCS2Trait
	{
		using ValueType = Char16;
		inline static constexpr const bool IsMultipleByte = false;
	};

	struct UTF8Trait
	{
		using ValueType = char;
		inline static constexpr const bool IsMultipleByte = true;
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
			static_assert(std::is_same_v<T, Trait::ValueType>, "invalid trait : the value type is different");
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
			static_assert(std::is_same_v<T, Trait::ValueType>, "invalid trait : the value type is different");
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
			static_assert(std::is_same_v<T, Trait::ValueType>, "invalid trait : the value type is different");
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
	}

	template<typename T, typename Trait = CharTrait<T>, typename Allocator = DefaultAllocator>
	class StringCore
	{
	public:
		using ValueType = std::conditional_t<Trait::IsMultipleByte, T*, T>;
		using ValueTrait = Trait;

		inline static const constexpr SizeType sm_MaxSize = (SGE_MAX_MEMORY_SIZE / sizeof(T)) - 1;

		static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

		template<typename _T, typename _Trait, typename _Allocator>
		friend class StringCore;

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
		class IteratorImpl
		{
		public:
			struct OutOfRangeError
			{
				inline static const TChar sm_pContent[] = SGE_TSTR("The iterator is out of range.");
				inline static bool Judge(const IteratorImpl& iter, _T* begin, _T* end)
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

			inline static IteratorImpl GetBegin(std::conditional_t<std::is_same_v<_T, std::remove_const_t<_T>>, StringCore, const StringCore>& str)
			{
				return IteratorImpl(reinterpret_cast<_T*>(str.GetData()));
			}

			inline static IteratorImpl GetEnd(std::conditional_t<std::is_same_v<_T, std::remove_const_t<_T>>, StringCore, const StringCore>& str)
			{
				return IteratorImpl(reinterpret_cast<_T*>(str.GetData()) + str.GetNormalSize());
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

			inline ValueType operator*() const
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

		private:
			inline explicit IteratorImpl(_T* ptr)
			{
				SGE_ASSERT(NullPointerError, ptr);
				m_pContent = ptr;
			}

		private:
			_T* m_pContent;
		};

		using Iterator = IteratorImpl<T>;
		using ConstIterator = IteratorImpl<const T>;

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