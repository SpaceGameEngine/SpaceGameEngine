/*
Copyright 2025 creatorlxd

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
#include "Serializer/Serializer.h"
#include "SGEString.hpp"
#include "Stream/StreamReader.hpp"
#include "Utility/LineBreak.h"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	template<IsString StringType>
	class StringSerializer
	{
	public:
		template<typename T>
		inline static MemoryData Serialize(const T& value)
		{
			return MakeMemoryData(ToString<StringType>(value));
		}

		template<typename T>
		inline static T Deserialize(const MemoryData& md)
		{
			using CharType = StringType::CharType;
			return StringTo<StringType, T>(StringType((const CharType*)md.GetData(), (const CharType*)((Byte*)md.GetData() + md.GetSize())));
		}
	};

	template<IsString StringType>
	class StreamReader<StringSerializer<StringType>>
	{
	public:
		inline StreamReader<StringSerializer<StringType>>(InputStream& stream)
			: m_Stream(stream)
		{
		}

		inline StringType ReadWord()
		{
			StringType result;
			// read word, similar to File::ReadWord
			if constexpr (!CharTrait::IsMultipleByte)
			{
				CharType char_buf = 0;
				ReadChar(&char_buf);

				while (m_IsValid && IsWordSeparatorCharacter<CharType, CharTrait>(char_buf))
					ReadChar(&char_buf);
				while (m_IsValid && !IsWordSeparatorCharacter<CharType, CharTrait>(char_buf))
				{
					result += char_buf;
					ReadChar(&char_buf);
				}
			}
			else
			{
				CharType char_buf[CharTrait::MaxMultipleByteSize];

				ReadChar(char_buf);
				while (m_IsValid && IsWordSeparatorCharacter<CharType, CharTrait>(char_buf))
					ReadChar(char_buf);
				while (m_IsValid && !IsWordSeparatorCharacter<CharType, CharTrait>(char_buf))
				{
					result.Insert(result.GetConstEnd(), (CharType*)char_buf, (CharType*)char_buf + StringImplement::GetMultipleByteCharSize<CharType, CharTrait>(char_buf));
					ReadChar(char_buf);
				}
			}
			return result;
		}

		inline StringType ReadLine(LineBreak lb = LineBreak::LF)
		{
			SGE_ASSERT(UnknownLineBreakError, lb);

			StringType result;
			// read line, similar to File::ReadLine
			if constexpr (!CharTrait::IsMultipleByte)
			{
				CharType char_buf = 0;
				ReadChar(&char_buf);
				if (lb != LineBreak::CRLF)
				{
					while (m_IsValid && GetLineBreak<CharType, CharTrait>(char_buf, char_buf) != lb)
					{
						result += char_buf;
						ReadChar(&char_buf);
					}
				}
				else
				{
					while (m_IsValid)
					{
						if (GetLineBreak<CharType, CharTrait>(char_buf, char_buf) == LineBreak::CR)
						{
							CharType pre_char_buf = char_buf;
							ReadChar(&char_buf);
							if (m_IsValid && GetLineBreak<CharType, CharTrait>(pre_char_buf, char_buf) == LineBreak::CRLF)
								break;
							else
								result += pre_char_buf;
						}
						else
						{
							result += char_buf;
							ReadChar(&char_buf);
						}
					}
				}
			}
			else
			{
				CharType char_buf[CharTrait::MaxMultipleByteSize];
				ReadChar(char_buf);

				if (lb != LineBreak::CRLF)
				{
					while (m_IsValid && GetLineBreak<CharType, CharTrait>(char_buf, char_buf) != lb)
					{
						result.Insert(result.GetConstEnd(), (CharType*)char_buf, (CharType*)char_buf + StringImplement::GetMultipleByteCharSize<CharType, CharTrait>(char_buf));
						ReadChar(char_buf);
					}
				}
				else
				{
					CharType pre_char_buf[CharTrait::MaxMultipleByteSize];
					while (m_IsValid)
					{
						if (GetLineBreak<CharType, CharTrait>(char_buf, char_buf) == LineBreak::CR)
						{
							memcpy(pre_char_buf, char_buf, sizeof(char_buf));
							ReadChar(char_buf);
							if (m_IsValid && GetLineBreak<CharType, CharTrait>(pre_char_buf, char_buf) == LineBreak::CRLF)
								break;
							else
								result.Insert(result.GetConstEnd(), (CharType*)pre_char_buf, (CharType*)pre_char_buf + StringImplement::GetMultipleByteCharSize<CharType, CharTrait>(pre_char_buf));
						}
						else
						{
							result.Insert(result.GetConstEnd(), (CharType*)char_buf, (CharType*)char_buf + StringImplement::GetMultipleByteCharSize<CharType, CharTrait>(char_buf));
							ReadChar(char_buf);
						}
					}
				}
			}
			return result;
		}

		inline StringType ReadAll()
		{
			StringType result;
			if constexpr (!CharTrait::IsMultipleByte)
			{
				CharType char_buf = 0;
				ReadChar(&char_buf);
				while (m_IsValid)
				{
					result += char_buf;
					ReadChar(&char_buf);
				}
			}
			else
			{
				CharType char_buf[CharTrait::MaxMultipleByteSize];
				ReadChar(char_buf);
				while (m_IsValid)
				{
					result.Insert(result.GetConstEnd(), (CharType*)char_buf, (CharType*)char_buf + StringImplement::GetMultipleByteCharSize<CharType, CharTrait>(char_buf));
					ReadChar(char_buf);
				}
			}
			return result;
		}

		template<typename T>
		inline StreamReader<StringSerializer<StringType>>& operator>>(T& value)
		{
			StringType word = ReadWord();
			value = StringSerializer<StringType>::template Deserialize<T>(ReferenceMemoryData(word));
			return *this;
		}

		inline operator bool() const
		{
			return m_IsValid;
		}

	private:
		using CharType = typename StringType::CharType;
		using CharTrait = typename StringType::ValueTrait;

		inline void ReadChar(CharType* pc)
		{
			SGE_ASSERT(NullPointerError, pc);

			if constexpr (!CharTrait::IsMultipleByte)
			{
				MemoryData memoryData = ReferenceMemoryData(pc, sizeof(CharType));
				m_IsValid = m_Stream.Read(memoryData);
			}
			else
			{
				// read multiple byte character, similar to File::ReadChar
				memset(pc, 0, sizeof(CharType) * CharTrait::MaxMultipleByteSize);
				auto char_head = ReferenceMemoryData(pc, sizeof(CharType));
				m_IsValid = m_Stream.Read(char_head);
				SizeType left_size = StringImplement::GetMultipleByteCharSize<CharType, CharTrait>(pc) - 1;
				if (left_size)
				{
					auto char_left = ReferenceMemoryData(pc + 1, left_size * sizeof(CharType));
					m_IsValid = m_Stream.Read(char_left);
				}
				using _InvalidMultipleByteCharError = StringImplement::InvalidMultipleByteCharError<CharType, CharTrait>;
				SGE_CHECK(_InvalidMultipleByteCharError, pc);
			}
		}

	private:
		InputStream& m_Stream;
		bool m_IsValid = true;
	};

	extern template class COMMON_API_TEMPLATE_DECLARE StreamReader<StringSerializer<UCS2String>>;
	extern template class COMMON_API_TEMPLATE_DECLARE StreamReader<StringSerializer<UTF8String>>;
}

/*!
@}
*/