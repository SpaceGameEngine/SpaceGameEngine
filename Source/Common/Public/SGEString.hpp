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
#include "SGEStringForward.h"
#include "Error.h"
#include "MemoryManager.h"
#include "Concurrent/Thread.h"
#include "Container/ContainerConcept.hpp"
#include "Utility/Utility.hpp"
#include "Concurrent/Atomic.hpp"
#include "Container/Vector.hpp"
#include <cstring>
#include <cmath>
#include <algorithm>
#include <concepts>

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{

	template<typename T>
	struct CharTrait
	{
		using ValueType = T;
		inline static constexpr const bool IsMultipleByte = false;
		inline static constexpr const SizeType MaxValue = (1 << (sizeof(T) * 8)) - 1;
		inline static constexpr const SizeType MaxMultipleByteSize = 1;
	};

	struct UCS2Trait
	{
		using ValueType = Char16;
		inline static constexpr const bool IsMultipleByte = false;
		inline static constexpr const SizeType MaxValue = (1 << (sizeof(Char16) * 8)) - 1;
		inline static constexpr const SizeType MaxMultipleByteSize = 1;
	};

	struct UTF8Trait
	{
		using ValueType = Char8;
		inline static constexpr const bool IsMultipleByte = true;
		inline static constexpr const SizeType MaxValue = (1 << (sizeof(Char8) * 8)) - 1;
		inline static constexpr const SizeType MaxMultipleByteSize = 4;
	};

	namespace StringImplement
	{
		enum class StringCategory : UInt8
		{
			Small = 0,
			Large = 1
		};

		template<typename T>
		inline constexpr StringCategory GetStringCategoryByRealSize(const SizeType size);

		/*!
		@brief simple storage for the string, do not consider '\0'.
		*/
		template<typename T, typename Allocator = DefaultAllocator>
		class Storage
		{
		private:
			inline static T* Create(SizeType size)
			{
				SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));

				T* re = (T*)Allocator::RawNew((size + 1) * sizeof(T), alignof(T));
				memset(re, NULL, (size + 1) * sizeof(T));
				return re + 1;
			}

			inline static T* Create(const T* ptr, SizeType size)
			{
				SGE_ASSERT(NullPointerError, ptr);
				SGE_ASSERT(InvalidValueError, size, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));

				T* re = (T*)Allocator::RawNew((size + 1) * sizeof(T), alignof(T));
				re[0] = NULL;
				++re;
				memcpy(re, ptr, size * sizeof(T));
				return re;
			}

			inline static void Destroy(T* ptr)
			{
				SGE_ASSERT(NullPointerError, ptr);
				Allocator::RawDelete(ptr - 1);
			}

		public:
			using AllocatorType = Allocator;
			using ValueType = T;

			inline Storage()
				: m_RealSize(0)
			{
			}

			inline explicit Storage(const SizeType size)
				: m_RealSize(size), m_Size(size)
			{
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				if (category == StringCategory::Small)
					memset(m_Content, NULL, sizeof(m_Content));
				else
					m_pContent = Create(m_RealSize);
			}

			inline Storage(const T* ptr, const SizeType size)
				: m_RealSize(size), m_Size(size)
			{
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				if (category == StringCategory::Small)
					memcpy(m_Content, ptr, size * sizeof(T));
				else
					m_pContent = Create(ptr, m_RealSize);
			}

			inline ~Storage()
			{
				if (GetStringCategoryByRealSize<T>(m_RealSize) != StringCategory::Small)
					Destroy(m_pContent);
			}

			inline Storage(const Storage& s)
				: m_RealSize(s.m_RealSize), m_Size(s.m_Size)
			{
				auto category = GetStringCategoryByRealSize<T>(s.m_RealSize);
				if (category == StringCategory::Small)
					memcpy(m_Content, s.m_Content, m_RealSize * sizeof(T));
				else
					m_pContent = Create(s.m_pContent, m_RealSize);
			}

			inline Storage(Storage&& s)
				: m_RealSize(s.m_RealSize), m_Size(s.m_Size)
			{
				auto category = GetStringCategoryByRealSize<T>(s.m_RealSize);
				if (category == StringCategory::Small)
					memcpy(m_Content, s.m_Content, m_RealSize * sizeof(T));
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
				: m_RealSize(s.GetRealSize()), m_Size(s.GetSize())
			{
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				if (category == StringCategory::Small)
					memcpy(m_Content, s.GetData(), m_RealSize * sizeof(T));
				else
					m_pContent = Create(s.GetData(), m_RealSize);
			}

			template<typename OtherAllocator>
			inline Storage(Storage<T, OtherAllocator>&& s)
				: m_RealSize(s.GetRealSize()), m_Size(s.GetSize())
			{
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				if (category == StringCategory::Small)
					memcpy(m_Content, s.GetData(), m_RealSize * sizeof(T));
				else
					m_pContent = Create(s.GetData(), m_RealSize);
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
						memcpy(m_Content, s.m_Content, m_RealSize * sizeof(T));
					else
						m_pContent = Create(s.m_pContent, m_RealSize);
				}
				else
				{
					if (category_for_s == StringCategory::Small)
					{
						Destroy(m_pContent);
						m_RealSize = s.m_RealSize;
						memcpy(m_Content, s.m_Content, m_RealSize * sizeof(T));
					}
					else
					{
						if (m_RealSize >= s.m_RealSize)
						{
							// no need for re-allocate
							m_Size = s.m_Size;
							memcpy(m_pContent, s.m_pContent, m_Size * sizeof(T));
						}
						else
						{
							Destroy(m_pContent);
							m_RealSize = s.m_RealSize;
							m_Size = s.m_Size;
							m_pContent = Create(s.m_pContent, m_RealSize);
						}
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
						memcpy(m_Content, s.m_Content, m_RealSize * sizeof(T));
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
					Destroy(m_pContent);
					m_RealSize = s.m_RealSize;
					m_Size = s.m_Size;
					if (category_for_s == StringCategory::Small)
						memcpy(m_Content, s.m_Content, m_RealSize * sizeof(T));
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
						memcpy(m_Content, s.GetData(), m_RealSize * sizeof(T));
					else
						m_pContent = Create(s.GetData(), m_RealSize);
				}
				else
				{
					if (category_for_s == StringCategory::Small)
					{
						Destroy(m_pContent);
						m_RealSize = s.GetRealSize();
						memcpy(m_Content, s.GetData(), m_RealSize * sizeof(T));
					}
					else
					{
						if (m_RealSize >= s.GetRealSize())
						{
							// no need for re-allocate
							m_Size = s.GetSize();
							memcpy(m_pContent, s.GetData(), m_Size * sizeof(T));
						}
						else
						{
							Destroy(m_pContent);
							m_RealSize = s.GetRealSize();
							m_Size = s.GetSize();
							m_pContent = Create(s.GetData(), m_RealSize);
						}
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
						memcpy(m_Content, s.GetData(), m_RealSize * sizeof(T));
					else
						m_pContent = Create(s.GetData(), m_RealSize);
				}
				else
				{
					if (category_for_s == StringCategory::Small)
					{
						Destroy(m_pContent);
						m_RealSize = s.GetRealSize();
						memcpy(m_Content, s.GetData(), m_RealSize * sizeof(T));
					}
					else
					{
						if (m_RealSize >= s.GetRealSize())
						{
							// no need for re-allocate
							m_Size = s.GetSize();
							memcpy(m_pContent, s.GetData(), m_Size * sizeof(T));
						}
						else
						{
							Destroy(m_pContent);
							m_RealSize = s.GetRealSize();
							m_Size = s.GetSize();
							m_pContent = Create(s.GetData(), m_RealSize);
						}
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
				SGE_ASSERT(InvalidValueError, size, GetSize(), SGE_MAX_MEMORY_SIZE / sizeof(T));
				if (m_RealSize == size)
					return;
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				auto new_category = GetStringCategoryByRealSize<T>(size);
				if (category == StringCategory::Small)
				{
					if (new_category == StringCategory::Small)
						m_RealSize = size;
					else
					{
						auto pbuf = Create(size);
						memcpy(pbuf, m_Content, m_RealSize * sizeof(T));
						m_pContent = pbuf;
						m_Size = m_RealSize;
						m_RealSize = size;
					}
				}
				else
				{
					if (new_category == StringCategory::Small)
					{
						// see warning
						auto pre_p_content = m_pContent;
						m_RealSize = m_Size;
						memcpy(m_Content, pre_p_content, m_RealSize * sizeof(T));
						Destroy(pre_p_content);
					}
					else
					{
						auto pbuf = Create(size);
						memcpy(pbuf, m_pContent, m_Size * sizeof(T));
						Destroy(m_pContent);
						m_pContent = pbuf;
						m_RealSize = size;
					}
				}
			}

			inline void Clear()
			{
				auto category = GetStringCategoryByRealSize<T>(m_RealSize);
				if (category == StringCategory::Small)
					m_RealSize = 0;
				else
				{
					Destroy(m_pContent);
					m_RealSize = 0;
					m_Size = 0;
					m_pContent = nullptr;
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
				SGE_ASSERT(InvalidValueError, size, 0, GetRealSize());
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
		};

		template<typename T>
		inline constexpr StringCategory GetStringCategoryByRealSize(const SizeType size)
		{
			if (size > ((sizeof(Storage<T>) - sizeof(SizeType)) / sizeof(T)))
				return StringCategory::Large;
			else
				return StringCategory::Small;
		}

		template<typename T, typename Trait = CharTrait<T>>
		struct InvalidMultipleByteCharHeadError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The multiple byte char's head is invalid.");
		};

		template<typename T, typename Trait = CharTrait<T>>
		struct InvalidMultipleByteCharError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The multiple byte char is invalid.");
		};

		template<typename T, typename Trait = CharTrait<T>>
		struct InvalidMultipleByteStringError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The multiple byte string is invalid.");
		};

		template<>
		struct InvalidMultipleByteCharHeadError<Char8, UTF8Trait>
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The UTF8 char's head is invalid.");
			inline static bool Judge(const Char8* pc)
			{
				if (!pc)
					return true;
				return static_cast<const UInt8>(*pc) > 0b11110111;
			}
		};

		using InvalidUTF8CharHeadError = InvalidMultipleByteCharHeadError<Char8, UTF8Trait>;

		template<>
		struct InvalidMultipleByteCharError<Char8, UTF8Trait>
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The UTF8 char is invalid.");
			inline static Pair<bool, const Char8*> JudgeCharContent(const Char8* pc)
			{
				if (static_cast<const UInt8>(*pc) > 0b11110111)
					return Pair<bool, const Char8*>(true, nullptr);
				SizeType left_size = 0;
				if (static_cast<const UInt8>(*pc) <= 0b01111111)
					left_size = 0;
				else if (static_cast<const UInt8>(*pc) <= 0b11011111)
					left_size = 1;
				else if (static_cast<const UInt8>(*pc) <= 0b11101111)
					left_size = 2;
				else if (static_cast<const UInt8>(*pc) <= 0b11110111)
					left_size = 3;
				pc += 1;
				for (SizeType i = 0; i < left_size; ++i)
				{
					if ((static_cast<const UInt8>(*pc) & 0b11000000) != 0b10000000)
						return Pair<bool, const Char8*>(true, nullptr);
					pc += 1;
				}
				return Pair<bool, const Char8*>(false, pc);
			}

			inline static bool Judge(const Char8* pc)
			{
				if (!pc)
					return true;
				return JudgeCharContent(pc).m_First;
			}
		};

		using InvalidUTF8CharError = InvalidMultipleByteCharError<Char8, UTF8Trait>;

		template<>
		struct InvalidMultipleByteStringError<Char8, UTF8Trait>
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The UTF8 string is invalid.");
			inline static bool Judge(const Char8* pstr)
			{
				if (!pstr)
					return true;
				while (*pstr)
				{
					auto res = InvalidMultipleByteCharError<Char8, UTF8Trait>::JudgeCharContent(pstr);
					if (res.m_First)
						return true;
					else
						pstr = res.m_Second;
				}
				return false;
			}
		};

		using InvalidUTF8StringError = InvalidMultipleByteStringError<Char8, UTF8Trait>;

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
		inline const Char8* GetNextMultipleByteChar<Char8, UTF8Trait>(const Char8* ptr)
		{
			SGE_ASSERT(NullPointerError, ptr);
			SGE_ASSERT(InvalidUTF8CharError, ptr);
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
		inline const Char8* GetPreviousMultipleByteChar<Char8, UTF8Trait>(const Char8* ptr)
		{
			SGE_ASSERT(NullPointerError, ptr);
			do
			{
				ptr -= 1;
			} while ((static_cast<const UInt8>(*ptr) & 0b11000000) == 0b10000000);
			SGE_ASSERT(InvalidUTF8CharError, ptr);
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
		inline SizeType GetMultipleByteCharSize<Char8, UTF8Trait>(const Char8* ptr)
		{
			SGE_ASSERT(NullPointerError, ptr);
			SGE_ASSERT(InvalidUTF8CharHeadError, ptr);
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

		struct InvalidUTF8CharForUCS2CharError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The UTF8 char is invalid for UCS2 char.");
			inline static Pair<bool, const Char8*> JudgeCharContent(const Char8* pc)
			{
				if (static_cast<const UInt8>(*pc) > 0b11110111)
					return Pair<bool, const Char8*>(true, nullptr);
				SizeType left_size = 0;
				if (static_cast<const UInt8>(*pc) <= 0b01111111)
					left_size = 0;
				else if (static_cast<const UInt8>(*pc) <= 0b11011111)
					left_size = 1;
				else if (static_cast<const UInt8>(*pc) <= 0b11101111)
					left_size = 2;
				else if (static_cast<const UInt8>(*pc) <= 0b11110111)
					return Pair<bool, const Char8*>(true, nullptr);
				pc += 1;
				for (SizeType i = 0; i < left_size; ++i)
				{
					if ((static_cast<const UInt8>(*pc) & 0b11000000) != 0b10000000)
						return Pair<bool, const Char8*>(true, nullptr);
					pc += 1;
				}
				return Pair<bool, const Char8*>(false, pc);
			}

			inline static bool Judge(const Char8* pc)
			{
				if (!pc)
					return true;
				return JudgeCharContent(pc).m_First;
			}
		};

		struct InvalidUTF8StringForUCS2StringError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The UTF8 string is invalid for UCS2 string.");
			inline static bool Judge(const Char8* pc)
			{
				if (!pc)
					return true;
				while (*pc)
				{
					auto res = InvalidUTF8CharForUCS2CharError::JudgeCharContent(pc);
					if (res.m_First)
						return true;
					else
						pc = res.m_Second;
				}
				return false;
			}
		};

		inline Char16 UTF8CharToUCS2Char(const Char8* ptr)
		{
			SGE_ASSERT(NullPointerError, ptr);
			SGE_ASSERT(InvalidUTF8CharForUCS2CharError, ptr);
			if (static_cast<const UInt8>(*ptr) <= 0b01111111)
				return *ptr;
			else if (static_cast<const UInt8>(*ptr) <= 0b11011111)
				return ((*(ptr + 1)) & 0b00111111) | (((*ptr) & 0b00011111) << 6);
			else if (static_cast<const UInt8>(*ptr) <= 0b11101111)
				return ((*(ptr + 2)) & 0b00111111) | (((*(ptr + 1)) & 0b00111111) << 6) | (((*ptr) & 0b00001111) << 12);
			else if (static_cast<const UInt8>(*ptr) <= 0b11110111)
				return ((*(ptr + 3)) & 0b00111111) | (((*(ptr + 2)) & 0b00111111) << 6) | (((*(ptr + 1)) & 0b00111111) << 12) | (((*ptr) & 0b00000111) << 18);	  // out of ucs2
		}

		inline SizeType GetUCS2CharToUTF8CharSize(const Char16 c)
		{
			if (c <= 0x7f)
				return 1;
			else if (c <= 0x7ff)
				return 2;
			else if (c <= 0xffff)
				return 3;
			else if (c <= 0x10ffff)	   // out of ucs2
				return 4;
		}

		/*!
		@brief Write the utf8 char to a existing memory according to the given ucs2 char.
		@return The next utf8 char's address.
		*/
		inline Char8* UCS2CharToUTF8Char(const Char16 c, Char8* pdst)
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
			else if (c <= 0x10ffff)	   // out of ucs2
			{
				*(pdst + 3) = 0b10000000 | (0b00111111 & c);
				*(pdst + 2) = 0b10000000 | (0b00111111 & (c >> 6));
				*(pdst + 1) = 0b10000000 | (0b00111111 & (c >> 12));
				*pdst = 0b11110000 | ((c >> 18) & 0b00000111);
				return pdst + 4;
			}
		}

		template<>
		inline Int8 CompareMultipleByteChar<Char8, UTF8Trait>(const Char8* ptr1, const Char8* ptr2)
		{
			SGE_ASSERT(NullPointerError, ptr1);
			SGE_ASSERT(NullPointerError, ptr2);
			SGE_ASSERT(InvalidUTF8CharError, ptr1);
			SGE_ASSERT(InvalidUTF8CharError, ptr2);
			Char16 c1 = UTF8CharToUCS2Char(ptr1);
			Char16 c2 = UTF8CharToUCS2Char(ptr2);
			if (c1 < c2)
				return -1;
			else if (c1 == c2)
				return 0;
			else if (c1 > c2)
				return 1;
		}

		template<typename T, typename Trait = CharTrait<T>>
		struct InvalidCharError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The char is invalid.");
			inline static bool Judge(T c)
			{
				return false;
			}
		};

		template<>
		struct InvalidCharError<Char16, UCS2Trait>
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The UCS2 char is invalid.");
			inline static bool Judge(Char16 c)
			{
				return false;
			}
		};

		template<>
		struct InvalidCharError<Char8, UTF8Trait>
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The UTF8 char is invalid.");
			inline static bool Judge(const Char8* pc)
			{
				return InvalidMultipleByteCharError<Char8, UTF8Trait>::Judge(pc);
			}
		};

		template<typename T, typename Trait = CharTrait<T>>
		struct InvalidStringError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The string is invalid.");
			inline static bool Judge(const T* pstr)
			{
				return pstr == nullptr;
			}
		};

		template<>
		struct InvalidStringError<Char16, UCS2Trait>
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The UCS2 string is invalid.");
			inline static bool Judge(const Char16* pstr)
			{
				return pstr == nullptr;
			}
		};

		template<>
		struct InvalidStringError<Char8, UTF8Trait>
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The UTF8 string is invalid.");
			inline static bool Judge(const Char8* pstr)
			{
				return InvalidMultipleByteStringError<Char8, UTF8Trait>::Judge(pstr);
			}
		};

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
				SGE_ASSERT(InvalidValueError, nsize, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));

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
				SGE_ASSERT(InvalidValueError, nsize, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));
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
				SGE_ASSERT(InvalidValueError, nsize, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));

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
				SGE_ASSERT(InvalidValueError, nsize, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));

				SizeType tsize = ptext_end - ptext_begin;
				SGE_ASSERT(InvalidValueError, tsize, 0, SGE_MAX_MEMORY_SIZE / sizeof(T));

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

		namespace ReverseBoyerMooreSearchImplement
		{
			template<typename T, typename Trait = CharTrait<T>>
			inline void MakeBadCharTable(SizeType* pdst, const T* pstr, const SizeType nsize)
			{
				static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");
				SGE_ASSERT(NullPointerError, pdst);
				SGE_ASSERT(NullPointerError, pstr);
				SGE_ASSERT(InvalidValueError, nsize, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));

				for (SizeType i = 0; i <= Trait::MaxValue; i++)
					pdst[i] = nsize;
				for (SizeType i = nsize - 1; i >= 1; i--)
					pdst[(typename MakeCharTypeUnsigned<T>::Type)pstr[i]] = i;
			}

			template<typename T>
			inline void MakePrefix(SizeType* pdst, const T* pstr, const SizeType nsize)
			{
				SGE_ASSERT(NullPointerError, pdst);
				SGE_ASSERT(NullPointerError, pstr);
				SGE_ASSERT(InvalidValueError, nsize, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));
				pdst[0] = nsize;
				SizeType last_end = 0;
				SizeType last_begin = 0;
				for (SizeType i = 1; i < nsize; ++i)
				{
					if (i < last_end && pdst[i - last_begin] < last_end - i)
						pdst[i] = pdst[i - last_begin];
					else
					{
						if (i > last_end)
							last_end = i;
						last_begin = i;
						while (last_end < nsize && pstr[last_end] == pstr[last_end - last_begin])
							++last_end;
						pdst[i] = last_end - last_begin;
					}
				}
			}

			template<typename T>
			inline void MakeGoodPrefixTable(SizeType* pdst, const SizeType* ppref, const T* pstr, const SizeType nsize)
			{
				SGE_ASSERT(NullPointerError, pdst);
				SGE_ASSERT(NullPointerError, ppref);
				SGE_ASSERT(NullPointerError, pstr);
				SGE_ASSERT(InvalidValueError, nsize, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));

				SizeType j = nsize - 1;
				for (SizeType i = 0; i < nsize; i++)
					pdst[i] = nsize;

				for (SizeType i = 0; i < nsize; i++)
				{
					if (ppref[i] == nsize - i)
					{
						for (; j >= nsize - i; --j)
						{
							if (pdst[j] == nsize)
								pdst[j] = i;
						}
					}
				}

				for (SizeType i = nsize - 1; i >= 1; i--)
				{
					pdst[ppref[i]] = i;
				}
			}

			template<typename T>
			inline const T* ReverseBoyerMooreSearch(const T* ptext_begin, const T* ptext_end, const T* ppat, const SizeType* pbct, const SizeType* pgpt, const SizeType nsize)
			{
				SGE_ASSERT(NullPointerError, ptext_begin);
				SGE_ASSERT(NullPointerError, ptext_end);
				SGE_ASSERT(NullPointerError, ppat);
				SGE_ASSERT(NullPointerError, pbct);
				SGE_ASSERT(NullPointerError, pgpt);
				SGE_ASSERT(InvalidValueError, nsize, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));

				SizeType tsize = ptext_end - ptext_begin;
				SGE_ASSERT(InvalidValueError, tsize, 0, SGE_MAX_MEMORY_SIZE / sizeof(T));

				if (tsize < nsize)
				{
					return ptext_end;
				}
				else
				{
					SizeType i = 0;
					SizeType j = tsize - nsize;
					while (j >= 0)
					{
						for (i = 0; i < nsize && ppat[i] == ptext_begin[i + j]; ++i)
						{
							if (i == nsize - 1)
							{
								return ptext_begin + j;
							}
						}
						SizeType modify = 0;
						if (pbct[(typename MakeCharTypeUnsigned<T>::Type)ptext_begin[i + j]] < i)
							modify = Max(i - pbct[(typename MakeCharTypeUnsigned<T>::Type)ptext_begin[i + j]], pgpt[i]);
						else
							modify = pgpt[i];

						if (modify > j)
							return ptext_end;
						else
							j -= modify;
					}
					return ptext_end;
				}
			}
		}

		namespace SimpleSearchImplement
		{
			template<typename T>
			inline const T* SimpleSearch(const T* ptext_begin, const T* ptext_end, const T* ppat, const SizeType nsize)
			{
				SGE_ASSERT(NullPointerError, ptext_begin);
				SGE_ASSERT(NullPointerError, ptext_end);
				SGE_ASSERT(NullPointerError, ppat);
				SGE_ASSERT(InvalidValueError, nsize, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));

				SizeType tsize = ptext_end - ptext_begin;
				SGE_ASSERT(InvalidValueError, tsize, 0, SGE_MAX_MEMORY_SIZE / sizeof(T));

				if (tsize < nsize)
				{
					return ptext_end;
				}
				else
				{
					const SizeType nsize_1 = nsize - 1;
					const T last_char = ppat[nsize_1];
					SizeType skip = 0;

					const T* pi = ptext_begin;
					const T* piend = ptext_end - nsize_1;

					while (pi < piend)
					{
						while (pi[nsize_1] != last_char)
						{
							if (++pi == piend)
								return ptext_end;
						}

						for (SizeType j = 0;;)
						{
							if (pi[j] != ppat[j])
							{
								if (skip == 0)
								{
									skip = 1;
									while (skip <= nsize_1 && ppat[nsize_1 - skip] != last_char)
										++skip;
								}
								pi += skip;
								break;
							}
							if (++j == nsize)
							{
								return pi;
							}
						}
					}
					return ptext_end;
				}
			}

			template<typename T>
			inline const T* ReverseSimpleSearch(const T* ptext_begin, const T* ptext_end, const T* ppat, const SizeType nsize)
			{
				SGE_ASSERT(NullPointerError, ptext_begin);
				SGE_ASSERT(NullPointerError, ptext_end);
				SGE_ASSERT(NullPointerError, ppat);
				SGE_ASSERT(InvalidValueError, nsize, 1, SGE_MAX_MEMORY_SIZE / sizeof(T));

				SizeType tsize = ptext_end - ptext_begin;
				SGE_ASSERT(InvalidValueError, tsize, 0, SGE_MAX_MEMORY_SIZE / sizeof(T));

				if (tsize < nsize)
				{
					return ptext_end;
				}
				else
				{
					const T first_char = ppat[0];
					SizeType skip = 0;
					const T* pi = ptext_end - nsize;
					const T* piend = ptext_begin - 1;
					while (pi > piend)
					{
						while (*pi != first_char)
						{
							if (--pi == piend)
								return ptext_end;
						}

						for (SizeType j = 0;;)
						{
							if (pi[j] != ppat[j])
							{
								if (skip == 0)
								{
									skip = 1;
									while (skip < nsize && ppat[skip] != first_char)
										++skip;
								}
								if (piend + skip >= pi)
									return ptext_end;
								pi -= skip;
								break;
							}
							if (++j == nsize)
							{
								return pi;
							}
						}
					}
					return ptext_end;
				}
			}

		}

		struct SearchStrategy
		{
		};

		struct BoyerMooreSearchStrategy : public SearchStrategy
		{
		};

		struct SimpleSearchStrategy : public SearchStrategy
		{
		};
	}

	template<typename T, typename Trait = CharTrait<T>, typename Allocator = DefaultAllocator>
	class StringCore;

	template<typename Allocator = DefaultAllocator>
	inline StringCore<Char8, UTF8Trait, Allocator> UCS2StringToUTF8String(const Char16* pstr);

	template<typename T, typename Trait, typename Allocator>
	class StringCore
	{
	public:
		using CharType = T;
		using ValueType = std::conditional_t<Trait::IsMultipleByte, T*, T>;
		using ConstValueType = std::conditional_t<Trait::IsMultipleByte, const T*, const T>;
		using ValueTrait = Trait;
		using AllocatorType = Allocator;

		inline static const constexpr SizeType sm_MaxSize = (SGE_MAX_MEMORY_SIZE / sizeof(T)) - 1;

		static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

		template<typename _T, typename _Trait, typename _Allocator>
		friend class StringCore;

		template<typename _Allocator>
		friend inline StringCore<Char8, UTF8Trait, _Allocator> UCS2StringToUTF8String(const StringCore<Char16, UCS2Trait, _Allocator>& str);

		template<typename _Allocator>
		friend inline StringCore<Char8, UTF8Trait, _Allocator> UCS2StringToUTF8String(const Char16* pstr);

		struct EmptyStringCoreError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The StringCore is empty");
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
			using _InvalidStringError = StringImplement::InvalidStringError<T, Trait>;
			SGE_ASSERT(_InvalidStringError, ptr);
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
			using _InvalidStringError = StringImplement::InvalidStringError<T, Trait>;
			SGE_ASSERT(_InvalidStringError, ptr);
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

		inline StringCore(const T* ptr)
			: m_Storage(ptr, GetCStringNormalSize(ptr) + 1), m_Size(GetCStringSize(ptr))
		{
			using _InvalidStringError = StringImplement::InvalidStringError<T, Trait>;
			SGE_ASSERT(_InvalidStringError, ptr);
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
		template<typename IteratorType, typename = std::enable_if_t<IsSequentialIterator<IteratorType>, void>, typename = std::enable_if_t<std::is_same_v<decltype(new ValueType((ValueType)(IteratorValueType<IteratorType>())), true), bool>, void>>
		inline StringCore(const IteratorType& begin, const IteratorType& end)
			: m_Storage(1)
		{
			if constexpr (!Trait::IsMultipleByte)
			{
				m_Size = end - begin;
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
				using _InvalidMultipleByteCharError = StringImplement::InvalidMultipleByteCharError<T, Trait>;
				if constexpr (!std::is_pointer_v<IteratorType>)
				{
					m_Size = end - begin;
					for (auto iter = begin; iter != end; ++iter)
					{
						SGE_ASSERT(_InvalidMultipleByteCharError, *iter);
						nsize += StringImplement::GetMultipleByteCharSize<T, Trait>(*iter);
					}
				}
				else
				{
					m_Size = 0;
					for (auto iter = begin; iter != end; iter = StringImplement::GetNextMultipleByteChar<T, Trait>(iter))
					{
						SGE_ASSERT(_InvalidMultipleByteCharError, iter);
						nsize += StringImplement::GetMultipleByteCharSize<T, Trait>(iter);
						m_Size += 1;
					}
				}
				SetRealSize(nsize);
				m_Storage.SetSize(nsize + 1);
				*(GetData() + nsize) = 0;
				SizeType i = 0;
				if constexpr (!std::is_pointer_v<IteratorType>)
				{
					for (auto iter = begin; iter != end; ++iter)
					{
						memcpy(GetData() + i, *iter, StringImplement::GetMultipleByteCharSize<T, Trait>(*iter) * sizeof(T));
						i += StringImplement::GetMultipleByteCharSize<T, Trait>(*iter);
					}
				}
				else
				{
					for (auto iter = begin; iter != end; iter = StringImplement::GetNextMultipleByteChar<T, Trait>(iter))
					{
						memcpy(GetData() + i, iter, StringImplement::GetMultipleByteCharSize<T, Trait>(iter) * sizeof(T));
						i += StringImplement::GetMultipleByteCharSize<T, Trait>(iter);
					}
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
			using _InvalidStringError = StringImplement::InvalidStringError<T, Trait>;
			SGE_ASSERT(_InvalidStringError, ptr);
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
			using _InvalidStringError = StringImplement::InvalidStringError<T, Trait>;
			SGE_ASSERT(_InvalidStringError, ptr);
			auto size = GetCStringNormalSize(ptr) + 1;
			if (m_Storage.GetSize() != size)
				return false;
			else
				return memcmp(m_Storage.GetData(), ptr, m_Storage.GetSize() * sizeof(T)) == 0;
		}

		inline bool operator!=(const T* ptr) const
		{
			SGE_ASSERT(NullPointerError, ptr);
			using _InvalidStringError = StringImplement::InvalidStringError<T, Trait>;
			SGE_ASSERT(_InvalidStringError, ptr);
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
			SGE_ASSERT(InvalidValueError, size, GetNormalSize(), sm_MaxSize);
			m_Storage.SetRealSize(size + 1);
		}

		template<typename IteratorType>
		struct IsStringCoreIterator;

		template<typename _T>
		class ReverseIteratorImpl;

		template<typename _T>
		class IteratorImpl
		{
		public:
			struct OutOfRangeError
			{
				inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The iterator is out of range.");
				inline static bool Judge(const IteratorImpl& iter, const std::remove_const_t<_T>* begin, const std::remove_const_t<_T>* end)
				{
					SGE_ASSERT(NullPointerError, begin);
					SGE_ASSERT(NullPointerError, end);
					return !(iter.m_pContent >= begin && iter.m_pContent <= end);
				}
			};

			using ValueType = std::conditional_t<Trait::IsMultipleByte, _T*, _T>;
			using NonConstValueType = std::conditional_t<Trait::IsMultipleByte, std::remove_const_t<_T>*, std::remove_const_t<_T>>;

		public:
			friend OutOfRangeError;
			friend StringCore;
			friend ReverseIteratorImpl<_T>;

			inline static IteratorImpl GetBegin(std::conditional_t<std::is_const_v<_T>, const StringCore&, StringCore&> str)
			{
				return IteratorImpl(const_cast<_T*>(str.GetData()));
			}

			inline static IteratorImpl GetEnd(std::conditional_t<std::is_const_v<_T>, const StringCore&, StringCore&> str)
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

			template<typename IteratorType, typename = std::enable_if_t<IsStringCoreIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, NonConstValueType>), void>>
			inline IteratorImpl(const IteratorType& iter)
			{
				m_pContent = iter.m_pContent;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsStringCoreIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, NonConstValueType>), void>>
			inline IteratorImpl& operator=(const IteratorType& iter)
			{
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

			template<typename IteratorType, typename = std::enable_if_t<IsStringCoreIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::NonConstValueType, NonConstValueType>), void>>
			inline bool operator==(const IteratorType& iter) const
			{
				return m_pContent == iter.m_pContent;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsStringCoreIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::NonConstValueType, NonConstValueType>), void>>
			inline bool operator!=(const IteratorType& iter) const
			{
				return m_pContent != iter.m_pContent;
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
				inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The iterator is out of range.");
				inline static bool Judge(const ReverseIteratorImpl& iter, const std::remove_const_t<_T>* begin, const std::remove_const_t<_T>* end)
				{
					SGE_ASSERT(NullPointerError, begin);
					SGE_ASSERT(NullPointerError, end);
					return !(iter.m_pContent >= begin && iter.m_pContent <= end);
				}
			};

			using ValueType = std::conditional_t<Trait::IsMultipleByte, _T*, _T>;
			using NonConstValueType = std::conditional_t<Trait::IsMultipleByte, std::remove_const_t<_T>*, std::remove_const_t<_T>>;

		public:
			friend OutOfRangeError;
			friend StringCore;

			inline static ReverseIteratorImpl GetBegin(std::conditional_t<std::is_const_v<_T>, const StringCore&, StringCore&> str)
			{
				if constexpr (!Trait::IsMultipleByte)
					return ReverseIteratorImpl(const_cast<_T*>(str.GetData()) + str.GetNormalSize() - 1);
				else
					return ReverseIteratorImpl(const_cast<_T*>(StringImplement::GetPreviousMultipleByteChar<std::remove_const_t<_T>, Trait>(str.GetData() + str.GetNormalSize())));
			}

			inline static ReverseIteratorImpl GetEnd(std::conditional_t<std::is_const_v<_T>, const StringCore&, StringCore&> str)
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

			template<typename IteratorType, typename = std::enable_if_t<IsStringCoreIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, NonConstValueType>), void>>
			inline ReverseIteratorImpl(const IteratorType& iter)
			{
				m_pContent = iter.m_pContent;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsStringCoreIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, NonConstValueType>), void>>
			inline ReverseIteratorImpl& operator=(const IteratorType& iter)
			{
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

			template<typename IteratorType, typename = std::enable_if_t<IsStringCoreIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::NonConstValueType, NonConstValueType>), void>>
			inline bool operator==(const IteratorType& iter) const
			{
				return m_pContent == iter.m_pContent;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsStringCoreIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::NonConstValueType, NonConstValueType>), void>>
			inline bool operator!=(const IteratorType& iter) const
			{
				return m_pContent != iter.m_pContent;
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
			// normal size can be changed by setrealsize when the string is small string.
			SizeType osize = GetNormalSize();
			if (GetRealSize() < nsize)
			{
				SetRealSize(2 * nsize);
			}
			memcpy(m_Storage.GetData() + osize, str.GetData(), (str.GetNormalSize() + 1) * sizeof(T));
			m_Size += str.GetSize();
			m_Storage.SetSize(nsize + 1);
			return *this;
		}

		template<typename OtherAllocator>
		inline StringCore& operator+=(const StringCore<T, Trait, OtherAllocator>& str)
		{
			SizeType nsize = GetNormalSize() + str.GetNormalSize();
			// normal size can be changed by setrealsize when the string is small string.
			SizeType osize = GetNormalSize();
			if (GetRealSize() < nsize)
			{
				SetRealSize(2 * nsize);
			}
			memcpy(m_Storage.GetData() + osize, str.GetData(), (str.GetNormalSize() + 1) * sizeof(T));
			m_Size += str.GetSize();
			m_Storage.SetSize(nsize + 1);
			return *this;
		}

		struct InvalidCharAppendError
		{
			inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The char appending is invalid.");
			inline static bool Judge(const T c)
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					return false;
				}
				else
				{
					return StringImplement::GetMultipleByteCharSize<T, Trait>(&c) != 1;
				}
			}
		};

		inline StringCore& operator+=(const T c)
		{
			SGE_ASSERT(InvalidCharAppendError, c);
			SizeType nsize = GetNormalSize() + 1;
			SizeType osize = GetNormalSize();
			if (GetRealSize() < nsize)
			{
				SetRealSize(2 * nsize);
			}
			*(m_Storage.GetData() + osize) = c;
			*(m_Storage.GetData() + nsize) = 0;
			m_Size += 1;
			m_Storage.SetSize(nsize + 1);
			return *this;
		}

		/*!
		@warning the string which pstr pointing to can not be a part of this StringCore's content.
		*/
		inline StringCore& operator+=(const T* pstr)
		{
			SGE_ASSERT(NullPointerError, pstr);
			using _InvalidStringError = StringImplement::InvalidStringError<T, Trait>;
			SGE_ASSERT(_InvalidStringError, pstr);
			SizeType size = GetCStringNormalSize(pstr);
			SizeType nsize = GetNormalSize() + size;
			SizeType osize = GetNormalSize();
			if (GetRealSize() < nsize)
			{
				SetRealSize(2 * nsize);
			}
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
			SGE_ASSERT(InvalidCharAppendError, c);
			StringCore re(*this);
			re += c;
			return re;
		}

		inline StringCore operator+(const T* pstr) const
		{
			SGE_ASSERT(NullPointerError, pstr);
			using _InvalidStringError = StringImplement::InvalidStringError<T, Trait>;
			SGE_ASSERT(_InvalidStringError, pstr);
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
			using _InvalidStringError = StringImplement::InvalidStringError<T, Trait>;
			SGE_ASSERT(_InvalidStringError, pstr);

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
			using _InvalidStringError = StringImplement::InvalidStringError<T, Trait>;
			SGE_ASSERT(_InvalidStringError, pstr);

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
			SGE_ASSERT(NullPointerError, pstr);
			using _InvalidStringError = StringImplement::InvalidStringError<T, Trait>;
			SGE_ASSERT(_InvalidStringError, pstr);
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
			SGE_ASSERT(NullPointerError, pstr);
			using _InvalidStringError = StringImplement::InvalidStringError<T, Trait>;
			SGE_ASSERT(_InvalidStringError, pstr);
			return StringCore::operator>(pstr) || StringCore::operator==(pstr);
		}

		/*!
		@brief insert a StringCore to the StringCore before the iterator.
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
				memmove(GetData() + index + str.GetNormalSize(), GetData() + index, (osize - index + 1) * sizeof(T));	 // include '\0'
				memcpy(GetData() + index, str.GetData(), str.GetNormalSize() * sizeof(T));
				m_Size += str.GetSize();
				m_Storage.SetSize(nsize + 1);
				return IteratorType(GetData() + index);
			}
			else	// reverse
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
				if (index != osize)
					memmove(GetData() + index + str.GetNormalSize(), GetData() + index, (osize - index + 1) * sizeof(T));	 // include '\0'

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
				memmove(GetData() + index + str.GetNormalSize(), GetData() + index, (osize - index + 1) * sizeof(T));	 // include '\0'
				memcpy(GetData() + index, str.GetData(), str.GetNormalSize() * sizeof(T));
				m_Size += str.GetSize();
				m_Storage.SetSize(nsize + 1);
				return IteratorType(GetData() + index);
			}
			else	// reverse
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
				if (index != osize)
					memmove(GetData() + index + str.GetNormalSize(), GetData() + index, (osize - index + 1) * sizeof(T));	 // include '\0'

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
		@warning only support sequential iterator.
		@warning the string which pstr pointing to can not be a part of this StringCore's content.
		@return Iterator pointing to the inserted CString's first char.
		*/
		template<typename IteratorType, typename = std::enable_if_t<IsStringCoreIterator<IteratorType>::Value, bool>>
		inline IteratorType Insert(const IteratorType& iter, const T* pstr)
		{
			SGE_ASSERT(NullPointerError, pstr);
			using _InvalidStringError = StringImplement::InvalidStringError<T, Trait>;
			SGE_ASSERT(_InvalidStringError, pstr);
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
				memmove(GetData() + index + snsize, GetData() + index, (osize - index + 1) * sizeof(T));	// include '\0'
				memcpy(GetData() + index, pstr, snsize * sizeof(T));
				m_Size += GetCStringSize(pstr);
				m_Storage.SetSize(nsize + 1);
				return IteratorType(GetData() + index);
			}
			else	// reverse
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
				if (index != osize)
					memmove(GetData() + index + snsize, GetData() + index, (osize - index + 1) * sizeof(T));	// include '\0'

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
		@warning only support sequential iterator.
		@warning the begin&end's Container can not be the current Vector.
		@return Iterator pointing to the inserted content's first char.
		*/
		template<typename IteratorType, typename OtherIteratorType, typename = std::enable_if_t<IsStringCoreIterator<IteratorType>::Value, bool>, typename = std::enable_if_t<IsSequentialIterator<OtherIteratorType>, void>>
		inline IteratorType Insert(const IteratorType& iter, const OtherIteratorType& begin, const OtherIteratorType& end)
		{
			SizeType snsize = 0;
			if constexpr (!Trait::IsMultipleByte)
			{
				snsize = end - begin;
			}
			else
			{
				using _InvalidMultipleByteCharError = StringImplement::InvalidMultipleByteCharError<T, Trait>;
				if constexpr (!std::is_pointer_v<OtherIteratorType>)
				{
					for (auto pbuf = begin; pbuf != end; ++pbuf)
					{
						SGE_ASSERT(_InvalidMultipleByteCharError, *pbuf);
						snsize += StringImplement::GetMultipleByteCharSize<T, Trait>(*pbuf);
					}
				}
				else
				{
					for (auto pbuf = begin; pbuf != end; pbuf = StringImplement::GetNextMultipleByteChar<T, Trait>(pbuf))
					{
						SGE_ASSERT(_InvalidMultipleByteCharError, pbuf);
						snsize += StringImplement::GetMultipleByteCharSize<T, Trait>(pbuf);
					}
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
				memmove(GetData() + index + snsize, GetData() + index, (osize - index + 1) * sizeof(T));	// include '\0'
				SizeType cnt = 0;
				SizeType mc_cnt = 0;
				for (auto i = begin; i != end; ++i)
				{
					if constexpr (!Trait::IsMultipleByte)
					{
						*(GetData() + index + cnt) = *i;
						++cnt;
					}
					else
					{
						if constexpr (!std::is_pointer_v<OtherIteratorType>)
						{
							memcpy(GetData() + index + cnt, *i, StringImplement::GetMultipleByteCharSize<T, Trait>(*i) * sizeof(T));
							cnt += StringImplement::GetMultipleByteCharSize<T, Trait>(*i);
						}
						else
						{
							memcpy(GetData() + index + cnt, i, StringImplement::GetMultipleByteCharSize<T, Trait>(i) * sizeof(T));
							SizeType mc_size = StringImplement::GetMultipleByteCharSize<T, Trait>(i);
							cnt += mc_size;
							i += mc_size - 1;
							mc_cnt += 1;
						}
					}
				}
				if constexpr (Trait::IsMultipleByte && std::is_pointer_v<OtherIteratorType>)
					m_Size = mc_cnt;
				else
					m_Size += (end - begin);
				m_Storage.SetSize(nsize + 1);
				return IteratorType(GetData() + index);
			}
			else	// reverse
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
				if (index != osize)
					memmove(GetData() + index + snsize, GetData() + index, (osize - index + 1) * sizeof(T));	// include '\0'

				ReverseIterator i1(GetData() + index + snsize);
				auto i2 = begin;
				if constexpr (!Trait::IsMultipleByte)
				{
					i1 += 1;
				}
				else
				{
					if constexpr (!std::is_pointer_v<OtherIteratorType>)
						i1.m_pContent -= StringImplement::GetMultipleByteCharSize<T, Trait>(*i2);
					else
						i1.m_pContent -= StringImplement::GetMultipleByteCharSize<T, Trait>(i2);
				}
				SizeType mc_cnt = 0;
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
						if constexpr (!std::is_pointer_v<OtherIteratorType>)
						{
							memcpy(*i1, *i2, StringImplement::GetMultipleByteCharSize<T, Trait>(*i2) * sizeof(T));
							++i2;
							i1.m_pContent -= StringImplement::GetMultipleByteCharSize<T, Trait>(*i2);
						}
						else
						{
							memcpy(*i1, i2, StringImplement::GetMultipleByteCharSize<T, Trait>(i2) * sizeof(T));
							i2 = StringImplement::GetNextMultipleByteChar<T, Trait>(i2);
							i1.m_pContent -= StringImplement::GetMultipleByteCharSize<T, Trait>(i2);
							mc_cnt += 1;
						}
					}
				}
				if constexpr (Trait::IsMultipleByte && std::is_pointer_v<OtherIteratorType>)
					m_Size = mc_cnt;
				else
					m_Size += (end - begin);
				m_Storage.SetSize(nsize + 1);
				IteratorType re(GetData() + index + snsize);
				re += 1;
				return re;
			}
		}

		/*!
		@brief remove a element in StringCore by giving a iterator.
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
			memmove((void*)iter.GetData(), ni.GetData(), (GetData() + GetNormalSize() - ni.GetData() + 1) * sizeof(T));
			m_Size -= 1;
			m_Storage.SetSize(nsize + 1);
			if constexpr (std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator>)
			{
				return iter;
			}
			else	// reverse
			{
				return iter + 1;
			}
		}

		template<typename IteratorType, typename = std::enable_if_t<IsStringCoreIterator<IteratorType>::Value, bool>>
		inline StringCore Substring(const IteratorType& iter, SizeType size) const
		{
			SGE_ASSERT(EmptyStringCoreError, m_Size);
			SGE_ASSERT(InvalidValueError, size, 1, m_Size);
			IteratorType eiter = iter + size;
			if constexpr (!Trait::IsMultipleByte)
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, GetData(), GetData() + GetNormalSize() - 1);
				SGE_ASSERT(typename IteratorType::OutOfRangeError, eiter, GetData() - 1, GetData() + GetNormalSize());
			}
			else
			{
				SGE_ASSERT(typename IteratorType::OutOfRangeError, iter, GetData(), StringImplement::GetPreviousMultipleByteChar<T, Trait>(GetData() + GetNormalSize()));
				SGE_ASSERT(typename IteratorType::OutOfRangeError, eiter, StringImplement::GetPreviousMultipleByteChar<T, Trait>(GetData()), GetData() + GetNormalSize());
			}

			return StringCore(iter, eiter);
		}

		inline StringCore Reverse() const
		{
			if (m_Size > 0)
				return StringCore(GetConstReverseBegin(), GetConstReverseEnd());
			else
				return StringCore();
		}

		/*!
		@brief remove some elements in StringCore by giving the iterators.
		@warning only support sequential iterator.
		@warning the begin&end's Container must be current StringCore.
		@return the iterator which points to the next element after the removing has been done.
		*/
		template<typename IteratorType, typename = std::enable_if_t<IsStringCoreIterator<IteratorType>::Value, bool>>
		inline IteratorType Remove(const IteratorType& begin, const IteratorType& end)
		{
			SGE_ASSERT(EmptyStringCoreError, m_Size);
			SizeType size = end - begin;
			SGE_ASSERT(InvalidValueError, size, 1, m_Size);
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
				memcpy((void*)begin.GetData(), end.GetData(), (GetData() + GetNormalSize() - end.GetData() + 1) * sizeof(T));
				m_Size -= size;
				m_Storage.SetSize(nsize + 1);
				return begin;
			}
			else	// reverse
			{
				if constexpr (!Trait::IsMultipleByte)
				{
					SGE_ASSERT(typename IteratorType::OutOfRangeError, end, (GetData() - 1), (GetData()) + GetNormalSize() - 1);
				}
				else
				{
					SGE_ASSERT(typename IteratorType::OutOfRangeError, end, (StringImplement::GetPreviousMultipleByteChar<T, Trait>(GetData())), (StringImplement::GetPreviousMultipleByteChar<T, Trait>(GetData() + GetNormalSize())));
				}
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
			SGE_ASSERT(InvalidValueError, i, 0, m_Size - 1);
			if constexpr (!Trait::IsMultipleByte)
			{
				return *(GetData() + i);
			}
			else
			{
				return *(GetBegin() + i);
			}
		}

		inline std::conditional_t<!Trait::IsMultipleByte, const T, const T*> operator[](const SizeType i) const
		{
			SGE_ASSERT(InvalidValueError, i, 0, m_Size - 1);
			if constexpr (!Trait::IsMultipleByte)
			{
				return *(GetData() + i);
			}
			else
			{
				return *(GetConstBegin() + i);
			}
		}

		template<typename SearchStrategy = StringImplement::SimpleSearchStrategy>
		inline Iterator Find(const StringCore& str, const Iterator& begin, const Iterator& end) const
		{
			SGE_ASSERT(typename Iterator::OutOfRangeError, begin, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(typename Iterator::OutOfRangeError, end, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(InvalidValueError, end - begin, 0, m_Size);

			SizeType nsize = str.GetNormalSize();
			SGE_ASSERT(InvalidValueError, nsize, 1, GetNormalSize());
			const T* pstr = str.GetData();
			if constexpr (std::is_same_v<SearchStrategy, StringImplement::BoyerMooreSearchStrategy>)
			{
				SizeType* pbct = (SizeType*)Allocator::RawNew((Trait::MaxValue + 1) * sizeof(SizeType), alignof(SizeType));
				SizeType* psuff = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));
				SizeType* pgst = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));

				StringImplement::BoyerMooreSearchImplement::MakeBadCharTable<T, Trait>(pbct, pstr, nsize);
				StringImplement::BoyerMooreSearchImplement::MakeSuffix(psuff, pstr, nsize);
				StringImplement::BoyerMooreSearchImplement::MakeGoodSuffixTable(pgst, psuff, pstr, nsize);
				T* res = (T*)(StringImplement::BoyerMooreSearchImplement::BoyerMooreSearch(begin.GetData(), end.GetData(), pstr, pbct, pgst, nsize));

				Allocator::RawDelete(pbct);
				Allocator::RawDelete(psuff);
				Allocator::RawDelete(pgst);

				return Iterator(res);
			}
			else if constexpr (std::is_same_v<SearchStrategy, StringImplement::SimpleSearchStrategy>)
			{
				T* res = (T*)(StringImplement::SimpleSearchImplement::SimpleSearch(begin.GetData(), end.GetData(), pstr, nsize));
				return Iterator(res);
			}
		}

		template<typename SearchStrategy = StringImplement::SimpleSearchStrategy>
		inline ConstIterator Find(const StringCore& str, const ConstIterator& begin, const ConstIterator& end) const
		{
			SGE_ASSERT(typename ConstIterator::OutOfRangeError, begin, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(typename ConstIterator::OutOfRangeError, end, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(InvalidValueError, end - begin, 0, m_Size);

			SizeType nsize = str.GetNormalSize();
			SGE_ASSERT(InvalidValueError, nsize, 1, GetNormalSize());
			const T* pstr = str.GetData();
			if constexpr (std::is_same_v<SearchStrategy, StringImplement::BoyerMooreSearchStrategy>)
			{
				SizeType* pbct = (SizeType*)Allocator::RawNew((Trait::MaxValue + 1) * sizeof(SizeType), alignof(SizeType));
				SizeType* psuff = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));
				SizeType* pgst = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));

				StringImplement::BoyerMooreSearchImplement::MakeBadCharTable<T, Trait>(pbct, pstr, nsize);
				StringImplement::BoyerMooreSearchImplement::MakeSuffix(psuff, pstr, nsize);
				StringImplement::BoyerMooreSearchImplement::MakeGoodSuffixTable(pgst, psuff, pstr, nsize);
				const T* res = StringImplement::BoyerMooreSearchImplement::BoyerMooreSearch(begin.GetData(), end.GetData(), pstr, pbct, pgst, nsize);

				Allocator::RawDelete(pbct);
				Allocator::RawDelete(psuff);
				Allocator::RawDelete(pgst);

				return ConstIterator(res);
			}
			else if constexpr (std::is_same_v<SearchStrategy, StringImplement::SimpleSearchStrategy>)
			{
				const T* res = StringImplement::SimpleSearchImplement::SimpleSearch(begin.GetData(), end.GetData(), pstr, nsize);
				return ConstIterator(res);
			}
		}

		template<typename OtherAllocator, typename SearchStrategy = StringImplement::SimpleSearchStrategy>
		inline Iterator Find(const StringCore<T, Trait, OtherAllocator>& str, const Iterator& begin, const Iterator& end) const
		{
			SGE_ASSERT(typename Iterator::OutOfRangeError, begin, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(typename Iterator::OutOfRangeError, end, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(InvalidValueError, end - begin, 0, m_Size);

			SizeType nsize = str.GetNormalSize();
			SGE_ASSERT(InvalidValueError, nsize, 1, GetNormalSize());
			const T* pstr = str.GetData();
			if constexpr (std::is_same_v<SearchStrategy, StringImplement::BoyerMooreSearchStrategy>)
			{
				SizeType* pbct = (SizeType*)Allocator::RawNew((Trait::MaxValue + 1) * sizeof(SizeType), alignof(SizeType));
				SizeType* psuff = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));
				SizeType* pgst = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));

				StringImplement::BoyerMooreSearchImplement::MakeBadCharTable<T, Trait>(pbct, pstr, nsize);
				StringImplement::BoyerMooreSearchImplement::MakeSuffix(psuff, pstr, nsize);
				StringImplement::BoyerMooreSearchImplement::MakeGoodSuffixTable(pgst, psuff, pstr, nsize);
				T* res = (T*)(StringImplement::BoyerMooreSearchImplement::BoyerMooreSearch(begin.GetData(), end.GetData(), pstr, pbct, pgst, nsize));

				Allocator::RawDelete(pbct);
				Allocator::RawDelete(psuff);
				Allocator::RawDelete(pgst);

				return Iterator(res);
			}
			else if constexpr (std::is_same_v<SearchStrategy, StringImplement::SimpleSearchStrategy>)
			{
				T* res = (T*)(StringImplement::SimpleSearchImplement::SimpleSearch(begin.GetData(), end.GetData(), pstr, nsize));
				return Iterator(res);
			}
		}

		template<typename OtherAllocator, typename SearchStrategy = StringImplement::SimpleSearchStrategy>
		inline ConstIterator Find(const StringCore<T, Trait, OtherAllocator>& str, const ConstIterator& begin, const ConstIterator& end) const
		{
			SGE_ASSERT(typename ConstIterator::OutOfRangeError, begin, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(typename ConstIterator::OutOfRangeError, end, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(InvalidValueError, end - begin, 0, m_Size);

			SizeType nsize = str.GetNormalSize();
			SGE_ASSERT(InvalidValueError, nsize, 1, GetNormalSize());
			const T* pstr = str.GetData();
			if constexpr (std::is_same_v<SearchStrategy, StringImplement::BoyerMooreSearchStrategy>)
			{
				SizeType* pbct = (SizeType*)Allocator::RawNew((Trait::MaxValue + 1) * sizeof(SizeType), alignof(SizeType));
				SizeType* psuff = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));
				SizeType* pgst = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));

				StringImplement::BoyerMooreSearchImplement::MakeBadCharTable<T, Trait>(pbct, pstr, nsize);
				StringImplement::BoyerMooreSearchImplement::MakeSuffix(psuff, pstr, nsize);
				StringImplement::BoyerMooreSearchImplement::MakeGoodSuffixTable(pgst, psuff, pstr, nsize);
				const T* res = StringImplement::BoyerMooreSearchImplement::BoyerMooreSearch(begin.GetData(), end.GetData(), pstr, pbct, pgst, nsize);

				Allocator::RawDelete(pbct);
				Allocator::RawDelete(psuff);
				Allocator::RawDelete(pgst);

				return ConstIterator(res);
			}
			else if constexpr (std::is_same_v<SearchStrategy, StringImplement::SimpleSearchStrategy>)
			{
				const T* res = StringImplement::SimpleSearchImplement::SimpleSearch(begin.GetData(), end.GetData(), pstr, nsize);
				return ConstIterator(res);
			}
		}

		template<typename SearchStrategy = StringImplement::SimpleSearchStrategy>
		inline Iterator Find(const T* pstr, const Iterator& begin, const Iterator& end) const
		{
			SGE_ASSERT(NullPointerError, pstr);
			using _InvalidStringError = StringImplement::InvalidStringError<T, Trait>;
			SGE_ASSERT(_InvalidStringError, pstr);
			SGE_ASSERT(typename Iterator::OutOfRangeError, begin, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(typename Iterator::OutOfRangeError, end, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(InvalidValueError, end - begin, 0, m_Size);

			SizeType nsize = GetCStringNormalSize(pstr);
			SGE_ASSERT(InvalidValueError, nsize, 1, GetNormalSize());
			if constexpr (std::is_same_v<SearchStrategy, StringImplement::BoyerMooreSearchStrategy>)
			{
				SizeType* pbct = (SizeType*)Allocator::RawNew((Trait::MaxValue + 1) * sizeof(SizeType), alignof(SizeType));
				SizeType* psuff = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));
				SizeType* pgst = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));

				StringImplement::BoyerMooreSearchImplement::MakeBadCharTable<T, Trait>(pbct, pstr, nsize);
				StringImplement::BoyerMooreSearchImplement::MakeSuffix(psuff, pstr, nsize);
				StringImplement::BoyerMooreSearchImplement::MakeGoodSuffixTable(pgst, psuff, pstr, nsize);
				T* res = (T*)(StringImplement::BoyerMooreSearchImplement::BoyerMooreSearch(begin.GetData(), end.GetData(), pstr, pbct, pgst, nsize));

				Allocator::RawDelete(pbct);
				Allocator::RawDelete(psuff);
				Allocator::RawDelete(pgst);

				return Iterator(res);
			}
			else if constexpr (std::is_same_v<SearchStrategy, StringImplement::SimpleSearchStrategy>)
			{
				T* res = (T*)(StringImplement::SimpleSearchImplement::SimpleSearch(begin.GetData(), end.GetData(), pstr, nsize));
				return Iterator(res);
			}
		}

		template<typename SearchStrategy = StringImplement::SimpleSearchStrategy>
		inline ConstIterator Find(const T* pstr, const ConstIterator& begin, const ConstIterator& end) const
		{
			SGE_ASSERT(NullPointerError, pstr);
			using _InvalidStringError = StringImplement::InvalidStringError<T, Trait>;
			SGE_ASSERT(_InvalidStringError, pstr);
			SGE_ASSERT(typename ConstIterator::OutOfRangeError, begin, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(typename ConstIterator::OutOfRangeError, end, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(InvalidValueError, end - begin, 0, m_Size);

			SizeType nsize = GetCStringNormalSize(pstr);
			SGE_ASSERT(InvalidValueError, nsize, 1, GetNormalSize());
			if constexpr (std::is_same_v<SearchStrategy, StringImplement::BoyerMooreSearchStrategy>)
			{
				SizeType* pbct = (SizeType*)Allocator::RawNew((Trait::MaxValue + 1) * sizeof(SizeType), alignof(SizeType));
				SizeType* psuff = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));
				SizeType* pgst = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));

				StringImplement::BoyerMooreSearchImplement::MakeBadCharTable<T, Trait>(pbct, pstr, nsize);
				StringImplement::BoyerMooreSearchImplement::MakeSuffix(psuff, pstr, nsize);
				StringImplement::BoyerMooreSearchImplement::MakeGoodSuffixTable(pgst, psuff, pstr, nsize);
				const T* res = StringImplement::BoyerMooreSearchImplement::BoyerMooreSearch(begin.GetData(), end.GetData(), pstr, pbct, pgst, nsize);

				Allocator::RawDelete(pbct);
				Allocator::RawDelete(psuff);
				Allocator::RawDelete(pgst);

				return ConstIterator(res);
			}
			else if constexpr (std::is_same_v<SearchStrategy, StringImplement::SimpleSearchStrategy>)
			{
				const T* res = StringImplement::SimpleSearchImplement::SimpleSearch(begin.GetData(), end.GetData(), pstr, nsize);
				return ConstIterator(res);
			}
		}

		template<typename SearchStrategy = StringImplement::SimpleSearchStrategy>
		inline Iterator ReverseFind(const StringCore& str, const Iterator& begin, const Iterator& end) const
		{
			SGE_ASSERT(typename Iterator::OutOfRangeError, begin, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(typename Iterator::OutOfRangeError, end, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(InvalidValueError, end - begin, 0, m_Size);

			SizeType nsize = str.GetNormalSize();
			SGE_ASSERT(InvalidValueError, nsize, 1, GetNormalSize());
			const T* pstr = str.GetData();
			if constexpr (std::is_same_v<SearchStrategy, StringImplement::BoyerMooreSearchStrategy>)
			{
				SizeType* pbct = (SizeType*)Allocator::RawNew((Trait::MaxValue + 1) * sizeof(SizeType), alignof(SizeType));
				SizeType* psuff = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));
				SizeType* pgpt = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));

				StringImplement::ReverseBoyerMooreSearchImplement::MakeBadCharTable<T, Trait>(pbct, pstr, nsize);
				StringImplement::ReverseBoyerMooreSearchImplement::MakePrefix(psuff, pstr, nsize);
				StringImplement::ReverseBoyerMooreSearchImplement::MakeGoodPrefixTable(pgpt, psuff, pstr, nsize);
				T* res = (T*)(StringImplement::ReverseBoyerMooreSearchImplement::ReverseBoyerMooreSearch(begin.GetData(), end.GetData(), pstr, pbct, pgpt, nsize));

				Allocator::RawDelete(pbct);
				Allocator::RawDelete(psuff);
				Allocator::RawDelete(pgpt);

				return Iterator(res);
			}
			else if constexpr (std::is_same_v<SearchStrategy, StringImplement::SimpleSearchStrategy>)
			{
				T* res = (T*)(StringImplement::SimpleSearchImplement::ReverseSimpleSearch(begin.GetData(), end.GetData(), pstr, nsize));
				return Iterator(res);
			}
		}

		template<typename SearchStrategy = StringImplement::SimpleSearchStrategy>
		inline ConstIterator ReverseFind(const StringCore& str, const ConstIterator& begin, const ConstIterator& end) const
		{
			SGE_ASSERT(typename ConstIterator::OutOfRangeError, begin, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(typename ConstIterator::OutOfRangeError, end, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(InvalidValueError, end - begin, 0, m_Size);

			SizeType nsize = str.GetNormalSize();
			SGE_ASSERT(InvalidValueError, nsize, 1, GetNormalSize());
			const T* pstr = str.GetData();
			if constexpr (std::is_same_v<SearchStrategy, StringImplement::BoyerMooreSearchStrategy>)
			{
				SizeType* pbct = (SizeType*)Allocator::RawNew((Trait::MaxValue + 1) * sizeof(SizeType), alignof(SizeType));
				SizeType* psuff = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));
				SizeType* pgpt = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));

				StringImplement::ReverseBoyerMooreSearchImplement::MakeBadCharTable<T, Trait>(pbct, pstr, nsize);
				StringImplement::ReverseBoyerMooreSearchImplement::MakePrefix(psuff, pstr, nsize);
				StringImplement::ReverseBoyerMooreSearchImplement::MakeGoodPrefixTable(pgpt, psuff, pstr, nsize);
				const T* res = StringImplement::ReverseBoyerMooreSearchImplement::ReverseBoyerMooreSearch(begin.GetData(), end.GetData(), pstr, pbct, pgpt, nsize);

				Allocator::RawDelete(pbct);
				Allocator::RawDelete(psuff);
				Allocator::RawDelete(pgpt);

				return ConstIterator(res);
			}
			else if constexpr (std::is_same_v<SearchStrategy, StringImplement::SimpleSearchStrategy>)
			{
				const T* res = StringImplement::SimpleSearchImplement::ReverseSimpleSearch(begin.GetData(), end.GetData(), pstr, nsize);
				return ConstIterator(res);
			}
		}

		template<typename OtherAllocator, typename SearchStrategy = StringImplement::SimpleSearchStrategy>
		inline Iterator ReverseFind(const StringCore<T, Trait, OtherAllocator>& str, const Iterator& begin, const Iterator& end) const
		{
			SGE_ASSERT(typename Iterator::OutOfRangeError, begin, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(typename Iterator::OutOfRangeError, end, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(InvalidValueError, end - begin, 0, m_Size);

			SizeType nsize = str.GetNormalSize();
			SGE_ASSERT(InvalidValueError, nsize, 1, GetNormalSize());
			const T* pstr = str.GetData();
			if constexpr (std::is_same_v<SearchStrategy, StringImplement::BoyerMooreSearchStrategy>)
			{
				SizeType* pbct = (SizeType*)Allocator::RawNew((Trait::MaxValue + 1) * sizeof(SizeType), alignof(SizeType));
				SizeType* psuff = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));
				SizeType* pgpt = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));

				StringImplement::ReverseBoyerMooreSearchImplement::MakeBadCharTable<T, Trait>(pbct, pstr, nsize);
				StringImplement::ReverseBoyerMooreSearchImplement::MakePrefix(psuff, pstr, nsize);
				StringImplement::ReverseBoyerMooreSearchImplement::MakeGoodPrefixTable(pgpt, psuff, pstr, nsize);
				T* res = (T*)(StringImplement::ReverseBoyerMooreSearchImplement::ReverseBoyerMooreSearch(begin.GetData(), end.GetData(), pstr, pbct, pgpt, nsize));

				Allocator::RawDelete(pbct);
				Allocator::RawDelete(psuff);
				Allocator::RawDelete(pgpt);

				return Iterator(res);
			}
			else if constexpr (std::is_same_v<SearchStrategy, StringImplement::SimpleSearchStrategy>)
			{
				T* res = (T*)(StringImplement::SimpleSearchImplement::ReverseSimpleSearch(begin.GetData(), end.GetData(), pstr, nsize));
				return Iterator(res);
			}
		}

		template<typename OtherAllocator, typename SearchStrategy = StringImplement::SimpleSearchStrategy>
		inline ConstIterator ReverseFind(const StringCore<T, Trait, OtherAllocator>& str, const ConstIterator& begin, const ConstIterator& end) const
		{
			SGE_ASSERT(typename ConstIterator::OutOfRangeError, begin, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(typename ConstIterator::OutOfRangeError, end, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(InvalidValueError, end - begin, 0, m_Size);

			SizeType nsize = str.GetNormalSize();
			SGE_ASSERT(InvalidValueError, nsize, 1, GetNormalSize());
			const T* pstr = str.GetData();
			if constexpr (std::is_same_v<SearchStrategy, StringImplement::BoyerMooreSearchStrategy>)
			{
				SizeType* pbct = (SizeType*)Allocator::RawNew((Trait::MaxValue + 1) * sizeof(SizeType), alignof(SizeType));
				SizeType* psuff = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));
				SizeType* pgpt = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));

				StringImplement::ReverseBoyerMooreSearchImplement::MakeBadCharTable<T, Trait>(pbct, pstr, nsize);
				StringImplement::ReverseBoyerMooreSearchImplement::MakePrefix(psuff, pstr, nsize);
				StringImplement::ReverseBoyerMooreSearchImplement::MakeGoodPrefixTable(pgpt, psuff, pstr, nsize);
				const T* res = StringImplement::ReverseBoyerMooreSearchImplement::ReverseBoyerMooreSearch(begin.GetData(), end.GetData(), pstr, pbct, pgpt, nsize);

				Allocator::RawDelete(pbct);
				Allocator::RawDelete(psuff);
				Allocator::RawDelete(pgpt);

				return ConstIterator(res);
			}
			else if constexpr (std::is_same_v<SearchStrategy, StringImplement::SimpleSearchStrategy>)
			{
				const T* res = StringImplement::SimpleSearchImplement::ReverseSimpleSearch(begin.GetData(), end.GetData(), pstr, nsize);
				return ConstIterator(res);
			}
		}

		template<typename SearchStrategy = StringImplement::SimpleSearchStrategy>
		inline Iterator ReverseFind(const T* pstr, const Iterator& begin, const Iterator& end) const
		{
			SGE_ASSERT(NullPointerError, pstr);
			using _InvalidStringError = StringImplement::InvalidStringError<T, Trait>;
			SGE_ASSERT(_InvalidStringError, pstr);
			SGE_ASSERT(typename Iterator::OutOfRangeError, begin, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(typename Iterator::OutOfRangeError, end, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(InvalidValueError, end - begin, 0, m_Size);

			SizeType nsize = GetCStringNormalSize(pstr);
			SGE_ASSERT(InvalidValueError, nsize, 1, GetNormalSize());
			if constexpr (std::is_same_v<SearchStrategy, StringImplement::BoyerMooreSearchStrategy>)
			{
				SizeType* pbct = (SizeType*)Allocator::RawNew((Trait::MaxValue + 1) * sizeof(SizeType), alignof(SizeType));
				SizeType* psuff = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));
				SizeType* pgpt = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));

				StringImplement::ReverseBoyerMooreSearchImplement::MakeBadCharTable<T, Trait>(pbct, pstr, nsize);
				StringImplement::ReverseBoyerMooreSearchImplement::MakePrefix(psuff, pstr, nsize);
				StringImplement::ReverseBoyerMooreSearchImplement::MakeGoodPrefixTable(pgpt, psuff, pstr, nsize);
				T* res = (T*)(StringImplement::ReverseBoyerMooreSearchImplement::ReverseBoyerMooreSearch(begin.GetData(), end.GetData(), pstr, pbct, pgpt, nsize));

				Allocator::RawDelete(pbct);
				Allocator::RawDelete(psuff);
				Allocator::RawDelete(pgpt);

				return Iterator(res);
			}
			else if constexpr (std::is_same_v<SearchStrategy, StringImplement::SimpleSearchStrategy>)
			{
				T* res = (T*)(StringImplement::SimpleSearchImplement::ReverseSimpleSearch(begin.GetData(), end.GetData(), pstr, nsize));
				return Iterator(res);
			}
		}

		template<typename SearchStrategy = StringImplement::SimpleSearchStrategy>
		inline ConstIterator ReverseFind(const T* pstr, const ConstIterator& begin, const ConstIterator& end) const
		{
			SGE_ASSERT(NullPointerError, pstr);
			using _InvalidStringError = StringImplement::InvalidStringError<T, Trait>;
			SGE_ASSERT(_InvalidStringError, pstr);
			SGE_ASSERT(typename ConstIterator::OutOfRangeError, begin, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(typename ConstIterator::OutOfRangeError, end, GetData(), GetData() + GetNormalSize());
			SGE_ASSERT(InvalidValueError, end - begin, 0, m_Size);

			SizeType nsize = GetCStringNormalSize(pstr);
			SGE_ASSERT(InvalidValueError, nsize, 1, GetNormalSize());
			if constexpr (std::is_same_v<SearchStrategy, StringImplement::BoyerMooreSearchStrategy>)
			{
				SizeType* pbct = (SizeType*)Allocator::RawNew((Trait::MaxValue + 1) * sizeof(SizeType), alignof(SizeType));
				SizeType* psuff = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));
				SizeType* pgpt = (SizeType*)Allocator::RawNew(nsize * sizeof(SizeType), alignof(SizeType));

				StringImplement::ReverseBoyerMooreSearchImplement::MakeBadCharTable<T, Trait>(pbct, pstr, nsize);
				StringImplement::ReverseBoyerMooreSearchImplement::MakePrefix(psuff, pstr, nsize);
				StringImplement::ReverseBoyerMooreSearchImplement::MakeGoodPrefixTable(pgpt, psuff, pstr, nsize);
				const T* res = StringImplement::ReverseBoyerMooreSearchImplement::ReverseBoyerMooreSearch(begin.GetData(), end.GetData(), pstr, pbct, pgpt, nsize);

				Allocator::RawDelete(pbct);
				Allocator::RawDelete(psuff);
				Allocator::RawDelete(pgpt);

				return ConstIterator(res);
			}
			else if constexpr (std::is_same_v<SearchStrategy, StringImplement::SimpleSearchStrategy>)
			{
				const T* res = StringImplement::SimpleSearchImplement::ReverseSimpleSearch(begin.GetData(), end.GetData(), pstr, nsize);
				return ConstIterator(res);
			}
		}

	private:
		StringImplement::Storage<T, Allocator> m_Storage;
		SizeType m_Size;
	};

	using UCS2String = StringCore<Char16, UCS2Trait, DefaultAllocator>;
	using UTF8String = StringCore<Char8, UTF8Trait, DefaultAllocator>;
	using String = UCS2String;

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL) && (!defined(COMMON_EXPORTS))
	extern template class SGE_DLL_IMPORT StringImplement::Storage<Char16, DefaultAllocator>;
	extern template class SGE_DLL_IMPORT StringImplement::Storage<Char8, DefaultAllocator>;
	extern template class SGE_DLL_IMPORT StringCore<Char16, UCS2Trait, DefaultAllocator>;
	extern template class SGE_DLL_IMPORT StringCore<Char8, UTF8Trait, DefaultAllocator>;
#endif

	template<typename Allocator>
	inline StringCore<Char16, UCS2Trait, Allocator> UTF8StringToUCS2String(const StringCore<Char8, UTF8Trait, Allocator>& str)
	{
		SizeType size = str.GetSize();
		StringCore<Char16, UCS2Trait, Allocator> re(size, 0);
		auto pdst = re.GetData();
		auto psrc = str.GetData();
		for (SizeType i = 0; i < size; i++)
		{
			*pdst = StringImplement::UTF8CharToUCS2Char(psrc);
			pdst += 1;
			psrc = StringImplement::GetNextMultipleByteChar<Char8, UTF8Trait>(psrc);
		}
		return re;
	}

	template<typename Allocator = DefaultAllocator>
	inline StringCore<Char16, UCS2Trait, Allocator> UTF8StringToUCS2String(const Char8* pstr)
	{
		SGE_ASSERT(NullPointerError, pstr);
		using _InvalidUTF8StringForUCS2StringError = StringImplement::InvalidUTF8StringForUCS2StringError;
		SGE_ASSERT(_InvalidUTF8StringForUCS2StringError, pstr);
		SizeType size = StringCore<Char8, UTF8Trait, Allocator>::GetCStringSize(pstr);
		StringCore<Char16, UCS2Trait, Allocator> re(size, 0);
		auto pdst = re.GetData();
		auto psrc = pstr;
		for (SizeType i = 0; i < size; i++)
		{
			*pdst = StringImplement::UTF8CharToUCS2Char(psrc);
			pdst += 1;
			psrc = StringImplement::GetNextMultipleByteChar<Char8, UTF8Trait>(psrc);
		}
		return re;
	}

	template<typename Allocator>
	inline StringCore<Char8, UTF8Trait, Allocator> UCS2StringToUTF8String(const StringCore<Char16, UCS2Trait, Allocator>& str)
	{
		SizeType nsize = 0;
		SizeType size = str.GetSize();
		auto psrc = str.GetData();
		for (SizeType i = 0; i < size; i++)
		{
			nsize += StringImplement::GetUCS2CharToUTF8CharSize(*psrc);
			psrc += 1;
		}
		StringCore<Char8, UTF8Trait, Allocator> re(nsize, SGE_U8STR(" "));
		auto pdst = re.GetData();
		psrc = str.GetData();
		for (SizeType i = 0; i < size; i++)
		{
			pdst = StringImplement::UCS2CharToUTF8Char(*psrc, pdst);
			psrc += 1;
		}
		re.m_Size = size;
		return re;
	}

	template<typename Allocator>
	inline StringCore<Char8, UTF8Trait, Allocator> UCS2StringToUTF8String(const Char16* pstr)
	{
		SGE_ASSERT(NullPointerError, pstr);
		SizeType nsize = 0;
		SizeType size = StringCore<Char16, UCS2Trait, Allocator>::GetCStringSize(pstr);
		auto psrc = pstr;
		for (SizeType i = 0; i < size; i++)
		{
			nsize += StringImplement::GetUCS2CharToUTF8CharSize(*psrc);
			psrc += 1;
		}
		StringCore<Char8, UTF8Trait, Allocator> re(nsize, SGE_U8STR(" "));
		auto pdst = re.GetData();
		psrc = pstr;
		for (SizeType i = 0; i < size; i++)
		{
			pdst = StringImplement::UCS2CharToUTF8Char(*psrc, pdst);
			psrc += 1;
		}
		re.m_Size = size;
		return re;
	}

#ifdef SGE_USE_WIDE_CHAR
	template<typename Allocator = DefaultAllocator>
	using TString = StringCore<Char16, UCS2Trait, Allocator>;

#define SGE_UCS2_TO_TSTR(str) str
#define SGE_UTF8_TO_TSTR(str) SpaceGameEngine::UTF8StringToUCS2String(str)

#define SGE_TSTR_TO_UCS2(str) str
#define SGE_TSTR_TO_UTF8(str) SpaceGameEngine::UCS2StringToUTF8String(str)
#else
	template<typename Allocator = DefaultAllocator>
	using TString = StringCore<Char8, UTF8Trait, Allocator>;

#define SGE_UCS2_TO_TSTR(str) SpaceGameEngine::UCS2StringToUTF8String(str)
#define SGE_UTF8_TO_TSTR(str) str

#define SGE_TSTR_TO_UCS2(str) SpaceGameEngine::UTF8StringToUCS2String(str)
#define SGE_TSTR_TO_UTF8(str) str
#endif

#define SGE_STR_TO_TSTR(str) SGE_UCS2_TO_TSTR(str)
#define SGE_TSTR_TO_STR(str) SGE_TSTR_TO_UCS2(str)
#define SGE_STR_TO_UCS2(str) str
#define SGE_UCS2_TO_STR(str) str
#define SGE_STR_TO_UTF8(str) SpaceGameEngine::UCS2StringToUTF8String(str)
#define SGE_UTF8_TO_STR(str) SpaceGameEngine::UTF8StringToUCS2String(str)

	template<typename StringType, typename T>
	struct ToStringCore
	{
		inline static StringType Get(const T& value)
		{
			return StringType(value);
		}
	};

	template<typename StringType, typename T, typename... Args>
	inline StringType ToString(const T& value, Args&&... args)
	{
		return ToStringCore<StringType, T>::Get(value, std::forward<Args>(args)...);
	}

	enum class NumberBase : UInt8
	{
		Decimal = 10,
		Binary = 2,
		Hex = 16
	};

	struct InvalidNumberBaseError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The number base is invalid.");
		inline static bool Judge(NumberBase base)
		{
			return base != NumberBase::Decimal && base != NumberBase::Binary && base != NumberBase::Hex;
		}
	};

	template<typename Allocator, std::integral IntegerType>
	struct ToStringCore<StringCore<Char16, UCS2Trait, Allocator>, IntegerType>
	{
		using StringType = StringCore<Char16, UCS2Trait, Allocator>;

		inline static constexpr const Char16 digits[201] =
			SGE_WSTR("00010203040506070809")
				SGE_WSTR("10111213141516171819")
					SGE_WSTR("20212223242526272829")
						SGE_WSTR("30313233343536373839")
							SGE_WSTR("40414243444546474849")
								SGE_WSTR("50515253545556575859")
									SGE_WSTR("60616263646566676869")
										SGE_WSTR("70717273747576777879")
											SGE_WSTR("80818283848586878889")
												SGE_WSTR("90919293949596979899");

		inline static constexpr const Char16 digits16[17] = SGE_WSTR("0123456789abcdef");

		inline static StringType Get(IntegerType value, SizeType min_length = 0, NumberBase base = NumberBase::Decimal)
		{
			SGE_ASSERT(InvalidNumberBaseError, base);
			if constexpr (std::is_unsigned_v<IntegerType>)
			{
				if (base == NumberBase::Decimal)
				{
					const SizeType length = std::max(Digits<10>(value), min_length);
					SizeType next = length - 1;
					StringType re(length, SGE_WSTR('0'));
					Char16* dst = re.GetData();
					while (value >= 100)
					{
						UInt64 i = (value % 100) * 2;
						value /= 100;
						dst[next] = digits[i + 1];
						dst[next - 1] = digits[i];
						next -= 2;
					}
					if (value < 10)
					{
						dst[next] = SGE_WSTR('0') + value;
					}
					else
					{
						auto i = SizeType(value) * 2;
						dst[next] = digits[i + 1];
						dst[next - 1] = digits[i];
					}

					return re;
				}
				else if (base == NumberBase::Binary)
				{
					const SizeType length = std::max(Digits<2>(value), min_length);
					SizeType next = length - 1;
					StringType re(length, SGE_WSTR('0'));
					Char16* dst = re.GetData();
					while (value >= 2)
					{
						dst[next] = digits16[value % 2];
						value /= 2;
						next -= 1;
					}
					dst[next] = digits16[value];

					return re;
				}
				else if (base == NumberBase::Hex)
				{
					const SizeType length = std::max(Digits<16>(value), min_length);
					SizeType next = length - 1;
					StringType re(length, SGE_WSTR('0'));
					Char16* dst = re.GetData();
					while (value >= 16)
					{
						dst[next] = digits16[value % 16];
						value /= 16;
						next -= 1;
					}
					dst[next] = digits16[value];

					return re;
				}
			}
			else
			{
				if (value < 0)
				{
					value *= -1;
					if (base == NumberBase::Decimal)
					{
						const SizeType length = std::max(Digits<10>(value) + 1, min_length);
						SizeType next = length - 1;
						StringType re(length, SGE_WSTR('0'));
						Char16* dst = re.GetData();
						dst[0] = SGE_WSTR('-');
						while (value >= 100)
						{
							UInt64 i = (value % 100) * 2;
							value /= 100;
							dst[next] = digits[i + 1];
							dst[next - 1] = digits[i];
							next -= 2;
						}
						if (value < 10)
						{
							dst[next] = SGE_WSTR('0') + value;
						}
						else
						{
							auto i = SizeType(value) * 2;
							dst[next] = digits[i + 1];
							dst[next - 1] = digits[i];
						}

						return re;
					}
					else if (base == NumberBase::Binary)
					{
						const SizeType length = std::max(Digits<2>(value) + 1, min_length);
						SizeType next = length - 1;
						StringType re(length, SGE_WSTR('0'));
						Char16* dst = re.GetData();
						dst[0] = SGE_WSTR('-');
						while (value >= 2)
						{
							dst[next] = digits16[value % 2];
							value /= 2;
							next -= 1;
						}
						dst[next] = digits16[value];

						return re;
					}
					else if (base == NumberBase::Hex)
					{
						const SizeType length = std::max(Digits<16>(value) + 1, min_length);
						SizeType next = length - 1;
						StringType re(length, SGE_WSTR('0'));
						Char16* dst = re.GetData();
						dst[0] = SGE_WSTR('-');
						while (value >= 16)
						{
							dst[next] = digits16[value % 16];
							value /= 16;
							next -= 1;
						}
						dst[next] = digits16[value];

						return re;
					}
				}
				else
				{
					if (base == NumberBase::Decimal)
					{
						const SizeType length = std::max(Digits<10>(value), min_length);
						SizeType next = length - 1;
						StringType re(length, SGE_WSTR('0'));
						Char16* dst = re.GetData();
						while (value >= 100)
						{
							UInt64 i = (value % 100) * 2;
							value /= 100;
							dst[next] = digits[i + 1];
							dst[next - 1] = digits[i];
							next -= 2;
						}
						if (value < 10)
						{
							dst[next] = SGE_WSTR('0') + value;
						}
						else
						{
							auto i = SizeType(value) * 2;
							dst[next] = digits[i + 1];
							dst[next - 1] = digits[i];
						}

						return re;
					}
					else if (base == NumberBase::Binary)
					{
						const SizeType length = std::max(Digits<2>(value), min_length);
						SizeType next = length - 1;
						StringType re(length, SGE_WSTR('0'));
						Char16* dst = re.GetData();
						while (value >= 2)
						{
							dst[next] = digits16[value % 2];
							value /= 2;
							next -= 1;
						}
						dst[next] = digits16[value];

						return re;
					}
					else if (base == NumberBase::Hex)
					{
						const SizeType length = std::max(Digits<16>(value), min_length);
						SizeType next = length - 1;
						StringType re(length, SGE_WSTR('0'));
						Char16* dst = re.GetData();
						while (value >= 16)
						{
							dst[next] = digits16[value % 16];
							value /= 16;
							next -= 1;
						}
						dst[next] = digits16[value];

						return re;
					}
				}
			}
		}
	};

	template<typename Allocator>
	struct ToStringCore<StringCore<Char16, UCS2Trait, Allocator>, float>
	{
		using StringType = StringCore<Char16, UCS2Trait, Allocator>;

		inline static StringType Get(float value, SizeType precision = 4)
		{
			if (precision != 0)
			{
				Int64 integer = (Int64)value;
				StringType re = ToString<StringType, Int64>(integer);
				re += SGE_WSTR('.');
				value -= (float)integer;
				if (value < 0.0f)
					value *= -1.0f;
				for (SizeType i = 0; i < precision; ++i)
				{
					value *= 10.0f;
				}
				UInt64 decimal = round(value);
				StringType decimal_str = ToString<StringType, UInt64>(decimal);
				if (decimal_str.GetSize() < precision)
					re += StringType(precision - decimal_str.GetSize(), SGE_WSTR('0'));
				re += decimal_str;
				return re;
			}
			else
				return ToString<StringType, Int64>((Int64)round(value));
		}
	};

	template<typename Allocator>
	struct ToStringCore<StringCore<Char16, UCS2Trait, Allocator>, double>
	{
		using StringType = StringCore<Char16, UCS2Trait, Allocator>;

		inline static StringType Get(double value, SizeType precision = 6)
		{
			if (precision != 0)
			{
				Int64 integer = (Int64)value;
				StringType re = ToString<StringType, Int64>(integer);
				re += SGE_WSTR('.');
				value -= (double)integer;
				if (value < 0.0)
					value *= -1.0;
				for (SizeType i = 0; i < precision; ++i)
				{
					value *= 10.0;
				}
				UInt64 decimal = round(value);
				StringType decimal_str = ToString<StringType, UInt64>(decimal);
				if (decimal_str.GetSize() < precision)
					re += StringType(precision - decimal_str.GetSize(), SGE_WSTR('0'));
				re += decimal_str;
				return re;
			}
			else
				return ToString<StringType, Int64>((Int64)round(value));
		}
	};

	//------------------------------------------------------------------

	template<typename Allocator, std::integral IntegerType>
	struct ToStringCore<StringCore<Char8, UTF8Trait, Allocator>, IntegerType>
	{
		using StringType = StringCore<Char8, UTF8Trait, Allocator>;

		inline static constexpr const Char8 digits[201] =
			SGE_U8STR("00010203040506070809")
				SGE_U8STR("10111213141516171819")
					SGE_U8STR("20212223242526272829")
						SGE_U8STR("30313233343536373839")
							SGE_U8STR("40414243444546474849")
								SGE_U8STR("50515253545556575859")
									SGE_U8STR("60616263646566676869")
										SGE_U8STR("70717273747576777879")
											SGE_U8STR("80818283848586878889")
												SGE_U8STR("90919293949596979899");

		inline static constexpr const Char8 digits16[17] = SGE_U8STR("0123456789abcdef");

		inline static StringType Get(IntegerType value, SizeType min_length = 0, NumberBase base = NumberBase::Decimal)
		{
			SGE_ASSERT(InvalidNumberBaseError, base);
			if constexpr (std::is_unsigned_v<IntegerType>)
			{
				if (base == NumberBase::Decimal)
				{
					const SizeType length = std::max(Digits<10>(value), min_length);
					SizeType next = length - 1;
					StringType re(length, SGE_U8STR("0"));
					Char8* dst = re.GetData();
					while (value >= 100)
					{
						UInt64 i = (value % 100) * 2;
						value /= 100;
						dst[next] = digits[i + 1];
						dst[next - 1] = digits[i];
						next -= 2;
					}
					if (value < 10)
					{
						dst[next] = SGE_U8STR('0') + value;
					}
					else
					{
						auto i = SizeType(value) * 2;
						dst[next] = digits[i + 1];
						dst[next - 1] = digits[i];
					}

					return re;
				}
				else if (base == NumberBase::Binary)
				{
					const SizeType length = std::max(Digits<2>(value), min_length);
					SizeType next = length - 1;
					StringType re(length, SGE_U8STR("0"));
					Char8* dst = re.GetData();
					while (value >= 2)
					{
						dst[next] = digits16[value % 2];
						value /= 2;
						next -= 1;
					}
					dst[next] = digits16[value];

					return re;
				}
				else if (base == NumberBase::Hex)
				{
					const SizeType length = std::max(Digits<16>(value), min_length);
					SizeType next = length - 1;
					StringType re(length, SGE_U8STR("0"));
					Char8* dst = re.GetData();
					while (value >= 16)
					{
						dst[next] = digits16[value % 16];
						value /= 16;
						next -= 1;
					}
					dst[next] = digits16[value];

					return re;
				}
			}
			else
			{
				if (value < 0)
				{
					value *= -1;
					if (base == NumberBase::Decimal)
					{
						const SizeType length = std::max(Digits<10>(value) + 1, min_length);
						SizeType next = length - 1;
						StringType re(length, SGE_U8STR("0"));
						Char8* dst = re.GetData();
						dst[0] = SGE_U8STR('-');
						while (value >= 100)
						{
							UInt64 i = (value % 100) * 2;
							value /= 100;
							dst[next] = digits[i + 1];
							dst[next - 1] = digits[i];
							next -= 2;
						}
						if (value < 10)
						{
							dst[next] = SGE_U8STR('0') + value;
						}
						else
						{
							auto i = SizeType(value) * 2;
							dst[next] = digits[i + 1];
							dst[next - 1] = digits[i];
						}

						return re;
					}
					else if (base == NumberBase::Binary)
					{
						const SizeType length = std::max(Digits<2>(value) + 1, min_length);
						SizeType next = length - 1;
						StringType re(length, SGE_U8STR("0"));
						Char8* dst = re.GetData();
						dst[0] = SGE_U8STR('-');
						while (value >= 2)
						{
							dst[next] = digits16[value % 2];
							value /= 2;
							next -= 1;
						}
						dst[next] = digits16[value];

						return re;
					}
					else if (base == NumberBase::Hex)
					{
						const SizeType length = std::max(Digits<16>(value) + 1, min_length);
						SizeType next = length - 1;
						StringType re(length, SGE_U8STR("0"));
						Char8* dst = re.GetData();
						dst[0] = SGE_U8STR('-');
						while (value >= 16)
						{
							dst[next] = digits16[value % 16];
							value /= 16;
							next -= 1;
						}
						dst[next] = digits16[value];

						return re;
					}
				}
				else
				{
					if (base == NumberBase::Decimal)
					{
						const SizeType length = std::max(Digits<10>(value), min_length);
						SizeType next = length - 1;
						StringType re(length, SGE_U8STR("0"));
						Char8* dst = re.GetData();
						while (value >= 100)
						{
							UInt64 i = (value % 100) * 2;
							value /= 100;
							dst[next] = digits[i + 1];
							dst[next - 1] = digits[i];
							next -= 2;
						}
						if (value < 10)
						{
							dst[next] = SGE_U8STR('0') + value;
						}
						else
						{
							auto i = SizeType(value) * 2;
							dst[next] = digits[i + 1];
							dst[next - 1] = digits[i];
						}

						return re;
					}
					else if (base == NumberBase::Binary)
					{
						const SizeType length = std::max(Digits<2>(value), min_length);
						SizeType next = length - 1;
						StringType re(length, SGE_U8STR("0"));
						Char8* dst = re.GetData();
						while (value >= 2)
						{
							dst[next] = digits16[value % 2];
							value /= 2;
							next -= 1;
						}
						dst[next] = digits16[value];

						return re;
					}
					else if (base == NumberBase::Hex)
					{
						const SizeType length = std::max(Digits<16>(value), min_length);
						SizeType next = length - 1;
						StringType re(length, SGE_U8STR("0"));
						Char8* dst = re.GetData();
						while (value >= 16)
						{
							dst[next] = digits16[value % 16];
							value /= 16;
							next -= 1;
						}
						dst[next] = digits16[value];

						return re;
					}
				}
			}
		}
	};

	template<typename Allocator>
	struct ToStringCore<StringCore<Char8, UTF8Trait, Allocator>, float>
	{
		using StringType = StringCore<Char8, UTF8Trait, Allocator>;

		inline static StringType Get(float value, SizeType precision = 4)
		{
			if (precision != 0)
			{
				Int64 integer = (Int64)value;
				StringType re = ToString<StringType, Int64>(integer);
				re += SGE_U8STR('.');
				value -= (float)integer;
				if (value < 0.0f)
					value *= -1.0f;
				for (SizeType i = 0; i < precision; ++i)
				{
					value *= 10.0f;
				}
				UInt64 decimal = round(value);
				StringType decimal_str = ToString<StringType, UInt64>(decimal);
				if (decimal_str.GetSize() < precision)
					re += StringType(precision - decimal_str.GetSize(), SGE_U8STR("0"));
				re += decimal_str;
				return re;
			}
			else
				return ToString<StringType, Int64>((Int64)round(value));
		}
	};

	template<typename Allocator>
	struct ToStringCore<StringCore<Char8, UTF8Trait, Allocator>, double>
	{
		using StringType = StringCore<Char8, UTF8Trait, Allocator>;

		inline static StringType Get(double value, SizeType precision = 6)
		{
			if (precision != 0)
			{
				Int64 integer = (Int64)value;
				StringType re = ToString<StringType, Int64>(integer);
				re += SGE_U8STR('.');
				value -= (double)integer;
				if (value < 0.0)
					value *= -1.0;
				for (SizeType i = 0; i < precision; ++i)
				{
					value *= 10.0;
				}
				UInt64 decimal = round(value);
				StringType decimal_str = ToString<StringType, UInt64>(decimal);
				if (decimal_str.GetSize() < precision)
					re += StringType(precision - decimal_str.GetSize(), SGE_U8STR("0"));
				re += decimal_str;
				return re;
			}
			else
				return ToString<StringType, Int64>((Int64)round(value));
		}
	};

	//------------------------------------------------------------------

	template<NumberBase Base, typename T, typename Trait = CharTrait<T>>
	struct IsNumericalCharacterCore
	{
	};

	template<NumberBase Base, typename T, typename Trait = CharTrait<T>, typename ArgType = std::enable_if_t<std::is_same_v<T, typename Trait::ValueType>, std::conditional_t<Trait::IsMultipleByte, const T*, T>>>
	inline bool IsNumericalCharacter(ArgType c)
	{
		return IsNumericalCharacterCore<Base, T, Trait>::Get(c);
	}

	template<>
	struct IsNumericalCharacterCore<NumberBase::Decimal, Char16, UCS2Trait>
	{
		inline static bool Get(Char16 c)
		{
			return c >= SGE_WSTR('0') && c <= SGE_WSTR('9');
		}
	};

	template<>
	struct IsNumericalCharacterCore<NumberBase::Binary, Char16, UCS2Trait>
	{
		inline static bool Get(Char16 c)
		{
			return c == SGE_WSTR('0') || c == SGE_WSTR('1');
		}
	};

	template<>
	struct IsNumericalCharacterCore<NumberBase::Hex, Char16, UCS2Trait>
	{
		inline static bool Get(Char16 c)
		{
			return (c >= SGE_WSTR('0') && c <= SGE_WSTR('9')) || (c >= SGE_WSTR('a') && c <= SGE_WSTR('f')) || (c >= SGE_WSTR('A') && c <= SGE_WSTR('F'));
		}
	};

	template<>
	struct IsNumericalCharacterCore<NumberBase::Decimal, Char8, UTF8Trait>
	{
		inline static bool Get(const Char8* pc)
		{
			SGE_ASSERT(NullPointerError, pc);
			SGE_ASSERT(StringImplement::InvalidUTF8CharError, pc);
			return (*pc) >= SGE_U8STR('0') && (*pc) <= SGE_U8STR('9');
		}
	};

	template<>
	struct IsNumericalCharacterCore<NumberBase::Binary, Char8, UTF8Trait>
	{
		inline static bool Get(const Char8* pc)
		{
			SGE_ASSERT(NullPointerError, pc);
			SGE_ASSERT(StringImplement::InvalidUTF8CharError, pc);
			return (*pc) == SGE_U8STR('0') || (*pc) == SGE_U8STR('1');
		}
	};

	template<>
	struct IsNumericalCharacterCore<NumberBase::Hex, Char8, UTF8Trait>
	{
		inline static bool Get(const Char8* pc)
		{
			SGE_ASSERT(NullPointerError, pc);
			SGE_ASSERT(StringImplement::InvalidUTF8CharError, pc);
			return ((*pc) >= SGE_U8STR('0') && (*pc) <= SGE_U8STR('9')) || ((*pc) >= SGE_U8STR('a') && (*pc) <= SGE_U8STR('f')) || ((*pc) >= SGE_U8STR('A') && (*pc) <= SGE_U8STR('F'));
		}
	};

	template<typename T, typename Trait, typename Allocator>
	inline bool IsSignedNumericalString(const StringCore<T, Trait, Allocator>& str)
	{
		static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

		if (str.GetSize() == 0)
			return false;

		bool is_nega = false;
		NumberBase base = NumberBase::Decimal;
		auto iter = str.GetConstBegin();

		if constexpr (std::is_same_v<Trait, UCS2Trait>)
		{
			if (!IsNumericalCharacter<NumberBase::Decimal, T, Trait>(*iter))
			{
				if (str.GetSize() == 1)
					return false;
				else if (*iter != SGE_WSTR('-'))
					return false;
				else
				{
					is_nega = true;
					++iter;
				}
			}
			if (str.GetSize() > (is_nega ? 3 : 2))
			{
				if (*iter == SGE_WSTR('0'))
				{
					++iter;
					if (*iter == SGE_WSTR('b') || *iter == SGE_WSTR('B'))
					{
						base = NumberBase::Binary;
						++iter;
					}
					else if (*iter == SGE_WSTR('x') || *iter == SGE_WSTR('X'))
					{
						base = NumberBase::Hex;
						++iter;
					}
				}
			}
		}
		else	// UTF8Trait
		{
			static_assert(std::is_same_v<Trait, UTF8Trait>, "unsupported CharTrait");
			if (!IsNumericalCharacter<NumberBase::Decimal, T, Trait>(*iter))
			{
				if (str.GetSize() == 1)
					return false;
				else if (**iter != SGE_U8STR('-'))
					return false;
				else
				{
					is_nega = true;
					++iter;
				}
			}
			if (str.GetSize() > (is_nega ? 3 : 2))
			{
				if (**iter == SGE_U8STR('0'))
				{
					++iter;
					if (**iter == SGE_U8STR('b') || **iter == SGE_U8STR('B'))
					{
						base = NumberBase::Binary;
						++iter;
					}
					else if (**iter == SGE_U8STR('x') || **iter == SGE_U8STR('X'))
					{
						base = NumberBase::Hex;
						++iter;
					}
				}
			}
		}

		if (base == NumberBase::Decimal)
		{
			for (; iter != str.GetConstEnd(); ++iter)
			{
				if (!IsNumericalCharacter<NumberBase::Decimal, T, Trait>(*iter))
					return false;
			}
		}
		else if (base == NumberBase::Binary)
		{
			for (; iter != str.GetConstEnd(); ++iter)
			{
				if (!IsNumericalCharacter<NumberBase::Binary, T, Trait>(*iter))
					return false;
			}
		}
		else if (base == NumberBase::Hex)
		{
			for (; iter != str.GetConstEnd(); ++iter)
			{
				if (!IsNumericalCharacter<NumberBase::Hex, T, Trait>(*iter))
					return false;
			}
		}

		return true;
	}

	template<typename T, typename Trait, typename Allocator>
	inline bool IsUnsignedNumericalString(const StringCore<T, Trait, Allocator>& str)
	{
		static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

		if (str.GetSize() == 0)
			return false;

		NumberBase base = NumberBase::Decimal;
		auto iter = str.GetConstBegin();

		if (str.GetSize() > 2)
		{
			if constexpr (std::is_same_v<Trait, UCS2Trait>)
			{
				if (*iter == SGE_WSTR('0'))
				{
					++iter;
					if (*iter == SGE_WSTR('b') || *iter == SGE_WSTR('B'))
					{
						base = NumberBase::Binary;
						++iter;
					}
					else if (*iter == SGE_WSTR('x') || *iter == SGE_WSTR('X'))
					{
						base = NumberBase::Hex;
						++iter;
					}
				}
			}
			else	// UTF8Trait
			{
				static_assert(std::is_same_v<Trait, UTF8Trait>, "unsupported CharTrait");
				if (**iter == SGE_U8STR('0'))
				{
					++iter;
					if (**iter == SGE_U8STR('b') || **iter == SGE_U8STR('B'))
					{
						base = NumberBase::Binary;
						++iter;
					}
					else if (**iter == SGE_U8STR('x') || **iter == SGE_U8STR('X'))
					{
						base = NumberBase::Hex;
						++iter;
					}
				}
			}
		}

		if (base == NumberBase::Decimal)
		{
			for (; iter != str.GetConstEnd(); ++iter)
			{
				if (!IsNumericalCharacter<NumberBase::Decimal, T, Trait>(*iter))
					return false;
			}
		}
		else if (base == NumberBase::Binary)
		{
			for (; iter != str.GetConstEnd(); ++iter)
			{
				if (!IsNumericalCharacter<NumberBase::Binary, T, Trait>(*iter))
					return false;
			}
		}
		else if (base == NumberBase::Hex)
		{
			for (; iter != str.GetConstEnd(); ++iter)
			{
				if (!IsNumericalCharacter<NumberBase::Hex, T, Trait>(*iter))
					return false;
			}
		}

		return true;
	}

	template<typename T, typename Trait, typename Allocator>
	inline bool IsDecimalString(const StringCore<T, Trait, Allocator>& str)
	{
		static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

		if (str.GetSize() == 0)
			return false;

		bool has_meet_point = false;

		if constexpr (std::is_same_v<Trait, UCS2Trait>)
		{
			if (!IsNumericalCharacter<NumberBase::Decimal, T, Trait>(str[0]))
			{
				if (str.GetSize() == 1)
					return false;
				else if (str[0] != SGE_WSTR('-'))
					return false;
			}

			for (auto iter = str.GetConstBegin() + 1; iter != str.GetConstEnd(); ++iter)
			{
				if (!IsNumericalCharacter<NumberBase::Decimal, T, Trait>(*iter))
				{
					if (*iter != SGE_WSTR('.'))
						return false;
					else
					{
						if (has_meet_point)
							return false;
						else if (iter + 1 == str.GetConstEnd())
							return false;
						else if (!IsNumericalCharacter<NumberBase::Decimal, T, Trait>(*(iter - 1)))
							return false;
						else
							has_meet_point = true;
					}
				}
			}
		}
		else	// UTF8Trait
		{
			static_assert(std::is_same_v<Trait, UTF8Trait>, "unsupported CharTrait");
			if (!IsNumericalCharacter<NumberBase::Decimal, T, Trait>(str[0]))
			{
				if (str.GetSize() == 1)
					return false;
				else if ((*str[0]) != SGE_U8STR('-'))
					return false;
			}

			for (auto iter = str.GetConstBegin() + 1; iter != str.GetConstEnd(); ++iter)
			{
				if (!IsNumericalCharacter<NumberBase::Decimal, T, Trait>(*iter))
				{
					if ((**iter) != SGE_U8STR('.'))
						return false;
					else
					{
						if (has_meet_point)
							return false;
						else if (iter + 1 == str.GetConstEnd())
							return false;
						else if (!IsNumericalCharacter<NumberBase::Decimal, T, Trait>(*(iter - 1)))
							return false;
						else
							has_meet_point = true;
					}
				}
			}
		}

		return true;
	}

	struct NonSignedNumericalStringError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The string is not numerical string.");
		template<typename T, typename Trait, typename Allocator>
		inline static bool Judge(const StringCore<T, Trait, Allocator>& str)
		{
			static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

			return !IsSignedNumericalString(str);
		}
	};

	struct NonUnsignedNumericalStringError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The string is not unsigned numerical string.");
		template<typename T, typename Trait, typename Allocator>
		inline static bool Judge(const StringCore<T, Trait, Allocator>& str)
		{
			static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

			return !IsUnsignedNumericalString(str);
		}
	};

	struct NonDecimalStringError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The string is not decimal string.");
		template<typename T, typename Trait, typename Allocator>
		inline static bool Judge(const StringCore<T, Trait, Allocator>& str)
		{
			static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

			return !IsDecimalString(str);
		}
	};

	template<typename StringType, typename T>
	struct StringToCore
	{
		inline static T Get(const StringType& str)
		{
			return T(str);
		}
	};

	template<typename StringType, typename T>
	inline T StringTo(const StringType& str)
	{
		return StringToCore<StringType, T>::Get(str);
	}

	template<typename Allocator, std::integral IntegerType>
	struct StringToCore<StringCore<Char16, UCS2Trait, Allocator>, IntegerType>
	{
		using StringType = StringCore<Char16, UCS2Trait, Allocator>;

		inline static IntegerType Get(const StringType& str)
		{
			if constexpr (std::is_signed_v<IntegerType>)
			{
				SGE_ASSERT(NonSignedNumericalStringError, str);
				bool is_negative = false;
				NumberBase base = NumberBase::Decimal;
				typename StringType::ConstIterator iter = str.GetConstBegin();
				if (*iter == SGE_WSTR('-'))
				{
					is_negative = true;
					++iter;
				}
				if (str.GetSize() > (is_negative ? 3 : 2))
				{
					if (*iter == SGE_WSTR('0'))
					{
						++iter;
						if (*iter == SGE_WSTR('b') || *iter == SGE_WSTR('B'))
						{
							base = NumberBase::Binary;
							++iter;
						}
						else if (*iter == SGE_WSTR('x') || *iter == SGE_WSTR('X'))
						{
							base = NumberBase::Hex;
							++iter;
						}
					}
				}
				IntegerType re = 0;
				if (base == NumberBase::Decimal)
				{
					for (; iter != str.GetConstEnd(); ++iter)
					{
						re *= 10;
						re += (*iter) - SGE_WSTR('0');
					}
				}
				else if (base == NumberBase::Binary)
				{
					for (; iter != str.GetConstEnd(); ++iter)
					{
						re *= 2;
						re += (*iter) - SGE_WSTR('0');
					}
				}
				else if (base == NumberBase::Hex)
				{
					for (; iter != str.GetConstEnd(); ++iter)
					{
						re *= 16;
						if (*iter >= SGE_WSTR('0') && *iter <= SGE_WSTR('9'))
							re += (*iter) - SGE_WSTR('0');
						else if (*iter >= SGE_WSTR('a') && *iter <= SGE_WSTR('f'))
							re += (*iter) - SGE_WSTR('a') + 10;
						else if (*iter >= SGE_WSTR('A') && *iter <= SGE_WSTR('F'))
							re += (*iter) - SGE_WSTR('A') + 10;
					}
				}
				return (is_negative ? -1 * re : re);
			}
			else
			{
				SGE_ASSERT(NonUnsignedNumericalStringError, str);
				NumberBase base = NumberBase::Decimal;
				typename StringType::ConstIterator iter = str.GetConstBegin();
				if (str.GetSize() > 2)
				{
					if (*iter == SGE_WSTR('0'))
					{
						++iter;
						if (*iter == SGE_WSTR('b') || *iter == SGE_WSTR('B'))
						{
							base = NumberBase::Binary;
							++iter;
						}
						else if (*iter == SGE_WSTR('x') || *iter == SGE_WSTR('X'))
						{
							base = NumberBase::Hex;
							++iter;
						}
					}
				}
				IntegerType re = 0;
				if (base == NumberBase::Decimal)
				{
					for (; iter != str.GetConstEnd(); ++iter)
					{
						re *= 10;
						re += (*iter) - SGE_WSTR('0');
					}
				}
				else if (base == NumberBase::Binary)
				{
					for (; iter != str.GetConstEnd(); ++iter)
					{
						re *= 2;
						re += (*iter) - SGE_WSTR('0');
					}
				}
				else if (base == NumberBase::Hex)
				{
					for (; iter != str.GetConstEnd(); ++iter)
					{
						re *= 16;
						if (*iter >= SGE_WSTR('0') && *iter <= SGE_WSTR('9'))
							re += (*iter) - SGE_WSTR('0');
						else if (*iter >= SGE_WSTR('a') && *iter <= SGE_WSTR('f'))
							re += (*iter) - SGE_WSTR('a') + 10;
						else if (*iter >= SGE_WSTR('A') && *iter <= SGE_WSTR('F'))
							re += (*iter) - SGE_WSTR('A') + 10;
					}
				}
				return re;
			}
		}
	};

	template<typename Allocator>
	struct StringToCore<StringCore<Char16, UCS2Trait, Allocator>, float>
	{
		using StringType = StringCore<Char16, UCS2Trait, Allocator>;

		inline static float Get(const StringType& str)
		{
			SGE_ASSERT(NonDecimalStringError, str);
			bool is_negative = false;
			bool is_after_point = false;
			typename StringType::ConstIterator iter = str.GetConstBegin();
			if (*iter == SGE_WSTR('-'))
			{
				is_negative = true;
				++iter;
			}
			float re = 0.0f;
			float decimal = 1.0f;
			for (; iter != str.GetConstEnd(); ++iter)
			{
				if ((!is_after_point) && (*iter) == SGE_WSTR('.'))
				{
					is_after_point = true;
					continue;
				}
				if (is_after_point)
				{
					decimal *= 0.1f;
					re += decimal * ((float)((*iter) - SGE_WSTR('0')));
				}
				else
				{
					re *= 10.0f;
					re += (float)((*iter) - SGE_WSTR('0'));
				}
			}
			return (is_negative ? -1.0f * re : re);
		}
	};

	template<typename Allocator>
	struct StringToCore<StringCore<Char16, UCS2Trait, Allocator>, double>
	{
		using StringType = StringCore<Char16, UCS2Trait, Allocator>;

		inline static double Get(const StringType& str)
		{
			SGE_ASSERT(NonDecimalStringError, str);
			bool is_negative = false;
			bool is_after_point = false;
			typename StringType::ConstIterator iter = str.GetConstBegin();
			if (*iter == SGE_WSTR('-'))
			{
				is_negative = true;
				++iter;
			}
			double re = 0.0;
			double decimal = 1.0;
			for (; iter != str.GetConstEnd(); ++iter)
			{
				if ((!is_after_point) && (*iter) == SGE_WSTR('.'))
				{
					is_after_point = true;
					continue;
				}
				if (is_after_point)
				{
					decimal *= 0.1;
					re += decimal * ((double)((*iter) - SGE_WSTR('0')));
				}
				else
				{
					re *= 10.0;
					re += (double)((*iter) - SGE_WSTR('0'));
				}
			}
			return (is_negative ? -1.0 * re : re);
		}
	};

	//------------------------------------------------------------------

	template<typename Allocator, std::integral IntegerType>
	struct StringToCore<StringCore<Char8, UTF8Trait, Allocator>, IntegerType>
	{
		using StringType = StringCore<Char8, UTF8Trait, Allocator>;

		inline static IntegerType Get(const StringType& str)
		{
			if constexpr (std::is_signed_v<IntegerType>)
			{
				SGE_ASSERT(NonSignedNumericalStringError, str);
				bool is_negative = false;
				NumberBase base = NumberBase::Decimal;
				typename StringType::ConstIterator iter = str.GetConstBegin();
				if (**iter == SGE_U8STR('-'))
				{
					is_negative = true;
					++iter;
				}
				if (str.GetSize() > (is_negative ? 3 : 2))
				{
					if (**iter == SGE_U8STR('0'))
					{
						++iter;
						if (**iter == SGE_U8STR('b') || **iter == SGE_U8STR('B'))
						{
							base = NumberBase::Binary;
							++iter;
						}
						else if (**iter == SGE_U8STR('x') || **iter == SGE_U8STR('X'))
						{
							base = NumberBase::Hex;
							++iter;
						}
					}
				}
				IntegerType re = 0;
				if (base == NumberBase::Decimal)
				{
					for (; iter != str.GetConstEnd(); ++iter)
					{
						re *= 10;
						re += (**iter) - SGE_U8STR('0');
					}
				}
				else if (base == NumberBase::Binary)
				{
					for (; iter != str.GetConstEnd(); ++iter)
					{
						re *= 2;
						re += (**iter) - SGE_U8STR('0');
					}
				}
				else if (base == NumberBase::Hex)
				{
					for (; iter != str.GetConstEnd(); ++iter)
					{
						re *= 16;
						if ((**iter) >= SGE_U8STR('0') && (**iter) <= SGE_U8STR('9'))
							re += (**iter) - SGE_U8STR('0');
						else if ((**iter) >= SGE_U8STR('a') && (**iter) <= SGE_U8STR('f'))
							re += (**iter) - SGE_U8STR('a') + 10;
						else if ((**iter) >= SGE_U8STR('A') && (**iter) <= SGE_U8STR('F'))
							re += (**iter) - SGE_U8STR('A') + 10;
					}
				}
				return (is_negative ? -1 * re : re);
			}
			else
			{
				SGE_ASSERT(NonUnsignedNumericalStringError, str);
				NumberBase base = NumberBase::Decimal;
				typename StringType::ConstIterator iter = str.GetConstBegin();
				if (str.GetSize() > 2)
				{
					if (**iter == SGE_U8STR('0'))
					{
						++iter;
						if (**iter == SGE_U8STR('b') || **iter == SGE_U8STR('B'))
						{
							base = NumberBase::Binary;
							++iter;
						}
						else if (**iter == SGE_U8STR('x') || **iter == SGE_U8STR('X'))
						{
							base = NumberBase::Hex;
							++iter;
						}
					}
				}
				IntegerType re = 0;
				if (base == NumberBase::Decimal)
				{
					for (; iter != str.GetConstEnd(); ++iter)
					{
						re *= 10;
						re += (**iter) - SGE_U8STR('0');
					}
				}
				else if (base == NumberBase::Binary)
				{
					for (; iter != str.GetConstEnd(); ++iter)
					{
						re *= 2;
						re += (**iter) - SGE_U8STR('0');
					}
				}
				else if (base == NumberBase::Hex)
				{
					for (; iter != str.GetConstEnd(); ++iter)
					{
						re *= 16;
						if ((**iter) >= SGE_U8STR('0') && (**iter) <= SGE_U8STR('9'))
							re += (**iter) - SGE_U8STR('0');
						else if ((**iter) >= SGE_U8STR('a') && (**iter) <= SGE_U8STR('f'))
							re += (**iter) - SGE_U8STR('a') + 10;
						else if ((**iter) >= SGE_U8STR('A') && (**iter) <= SGE_U8STR('F'))
							re += (**iter) - SGE_U8STR('A') + 10;
					}
				}
				return re;
			}
		}
	};

	template<typename Allocator>
	struct StringToCore<StringCore<Char8, UTF8Trait, Allocator>, float>
	{
		using StringType = StringCore<Char8, UTF8Trait, Allocator>;

		inline static float Get(const StringType& str)
		{
			SGE_ASSERT(NonDecimalStringError, str);
			bool is_negative = false;
			bool is_after_point = false;
			typename StringType::ConstIterator iter = str.GetConstBegin();
			if (**iter == SGE_U8STR('-'))
			{
				is_negative = true;
				++iter;
			}
			float re = 0.0f;
			float decimal = 1.0f;
			for (; iter != str.GetConstEnd(); ++iter)
			{
				if ((!is_after_point) && (**iter) == SGE_U8STR('.'))
				{
					is_after_point = true;
					continue;
				}
				if (is_after_point)
				{
					decimal *= 0.1f;
					re += decimal * ((float)((**iter) - SGE_U8STR('0')));
				}
				else
				{
					re *= 10.0f;
					re += (float)((**iter) - SGE_U8STR('0'));
				}
			}
			return (is_negative ? -1.0f * re : re);
		}
	};

	template<typename Allocator>
	struct StringToCore<StringCore<Char8, UTF8Trait, Allocator>, double>
	{
		using StringType = StringCore<Char8, UTF8Trait, Allocator>;

		inline static double Get(const StringType& str)
		{
			SGE_ASSERT(NonDecimalStringError, str);
			bool is_negative = false;
			bool is_after_point = false;
			typename StringType::ConstIterator iter = str.GetConstBegin();
			if (**iter == SGE_U8STR('-'))
			{
				is_negative = true;
				++iter;
			}
			double re = 0.0;
			double decimal = 1.0;
			for (; iter != str.GetConstEnd(); ++iter)
			{
				if ((!is_after_point) && (**iter) == SGE_U8STR('.'))
				{
					is_after_point = true;
					continue;
				}
				if (is_after_point)
				{
					decimal *= 0.1;
					re += decimal * ((double)((**iter) - SGE_U8STR('0')));
				}
				else
				{
					re *= 10.0;
					re += (double)((**iter) - SGE_U8STR('0'));
				}
			}
			return (is_negative ? -1.0 * re : re);
		}
	};

	template<typename T, typename Trait, typename Allocator>
	inline Vector<StringCore<T, Trait, Allocator>, Allocator> Split(const StringCore<T, Trait, Allocator>& str, const StringCore<T, Trait, Allocator>& separ)
	{
		static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

		Vector<StringCore<T, Trait, Allocator>, Allocator> re;
		auto siter = str.GetConstBegin();
		auto eiter = str.Find(separ, siter, str.GetConstEnd());
		while (eiter != str.GetConstEnd())
		{
			re.EmplaceBack(siter, eiter);
			siter = eiter + separ.GetSize();
			eiter = str.Find(separ, siter, str.GetConstEnd());
		}
		re.EmplaceBack(siter, eiter);
		return re;
	}

	template<typename T, typename Trait = CharTrait<T>>
	struct IsLineSeparatorCharacterCore
	{
	};

	template<typename T, typename Trait = CharTrait<T>, typename ArgType = std::enable_if_t<std::is_same_v<T, typename Trait::ValueType>, std::conditional_t<Trait::IsMultipleByte, const T*, T>>>
	inline bool IsLineSeparatorCharacter(ArgType c)
	{
		return IsLineSeparatorCharacterCore<T, Trait>::Get(c);
	}

	template<>
	struct IsLineSeparatorCharacterCore<Char16, UCS2Trait>
	{
		inline static bool Get(Char16 c)
		{
			return c == SGE_WSTR('\n') || c == SGE_WSTR('\r');
		}
	};

	template<>
	struct IsLineSeparatorCharacterCore<Char8, UTF8Trait>
	{
		inline static bool Get(const Char8* pc)
		{
			SGE_ASSERT(NullPointerError, pc);
			SGE_ASSERT(StringImplement::InvalidUTF8CharError, pc);
			return (*pc) == SGE_U8STR('\n') || (*pc) == SGE_U8STR('\r');
		}
	};

	template<typename T, typename Trait = CharTrait<T>>
	struct IsWordSeparatorCharacterCore
	{
	};

	template<typename T, typename Trait = CharTrait<T>, typename ArgType = std::enable_if_t<std::is_same_v<T, typename Trait::ValueType>, std::conditional_t<Trait::IsMultipleByte, const T*, T>>>
	inline bool IsWordSeparatorCharacter(ArgType c)
	{
		return IsWordSeparatorCharacterCore<T, Trait>::Get(c);
	}

	template<>
	struct IsWordSeparatorCharacterCore<Char16, UCS2Trait>
	{
		inline static bool Get(Char16 c)
		{
			return c == SGE_WSTR(' ') || c == SGE_WSTR('\t') || c == SGE_WSTR('\n') || c == SGE_WSTR('\r');
		}
	};

	template<>
	struct IsWordSeparatorCharacterCore<Char8, UTF8Trait>
	{
		inline static bool Get(const Char8* pc)
		{
			SGE_ASSERT(NullPointerError, pc);
			SGE_ASSERT(StringImplement::InvalidUTF8CharError, pc);
			return (*pc) == SGE_U8STR(' ') || (*pc) == SGE_U8STR('\t') || (*pc) == SGE_U8STR('\n') || (*pc) == SGE_U8STR('\r');
		}
	};

}

/*!
@}
*/