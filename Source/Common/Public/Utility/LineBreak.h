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
#include "SGEString.hpp"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	enum class LineBreak : UInt8
	{
		Unknown = 0,
		LF = 1,
		CR = 2,
		CRLF = 3
	};

	struct UnknownLineBreakError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("The LineBreak is unknown.");
		static COMMON_API bool Judge(LineBreak lb);
	};

	template<typename T, typename Trait = CharTrait<T>>
	struct GetLineBreakCore
	{
	};

	template<typename T, typename Trait = CharTrait<T>, typename ArgType = std::enable_if_t<std::is_same_v<T, typename Trait::ValueType>, std::conditional_t<Trait::IsMultipleByte, const T*, T>>>
	inline LineBreak GetLineBreak(ArgType c1, ArgType c2)
	{
		return GetLineBreakCore<T, Trait>::Get(c1, c2);
	}

	template<>
	struct COMMON_API GetLineBreakCore<Char16, UCS2Trait>
	{
		static LineBreak Get(Char16 c1, Char16 c2);
	};

	template<>
	struct COMMON_API GetLineBreakCore<Char8, UTF8Trait>
	{
		static LineBreak Get(const Char8* pc1, const Char8* pc2);
	};

	template<typename T, typename Trait = CharTrait<T>, IsAllocator Allocator = DefaultAllocator>
	struct GetLineBreakStringCore
	{
	};

	template<typename T, typename Trait = CharTrait<T>, IsAllocator Allocator = DefaultAllocator>
	inline StringCore<T, Trait, Allocator> GetLineBreakString(LineBreak lb)
	{
		return GetLineBreakStringCore<T, Trait, Allocator>::Get(lb);
	}

	template<IsAllocator Allocator>
	struct GetLineBreakStringCore<Char16, UCS2Trait, Allocator>
	{
		inline static StringCore<Char16, UCS2Trait, Allocator> Get(LineBreak lb)
		{
			SGE_ASSERT(UnknownLineBreakError, lb);
			if (lb == LineBreak::CR)
				return SGE_WSTR("\r");
			else if (lb == LineBreak::LF)
				return SGE_WSTR("\n");
			else if (lb == LineBreak::CRLF)
				return SGE_WSTR("\r\n");
		}
	};

	template<IsAllocator Allocator>
	struct GetLineBreakStringCore<Char8, UTF8Trait, Allocator>
	{
		inline static StringCore<Char8, UTF8Trait, Allocator> Get(LineBreak lb)
		{
			SGE_ASSERT(UnknownLineBreakError, lb);
			if (lb == LineBreak::CR)
				return SGE_U8STR("\r");
			else if (lb == LineBreak::LF)
				return SGE_U8STR("\n");
			else if (lb == LineBreak::CRLF)
				return SGE_U8STR("\r\n");
		}
	};

	COMMON_API LineBreak GetSystemLineBreak();

	struct EndLineType
	{
	};

	inline constexpr const EndLineType EndLine;
}

/*!
@}
*/