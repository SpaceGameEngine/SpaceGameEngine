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
#include "TypeDefinition.hpp"
#include <type_traits>
#include <limits>

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	template<typename T, SizeType _Size>
	struct ArrayLiteral
	{
		using Type = T;
		inline static constexpr const SizeType Size = _Size;

		T m_Value[_Size];

		constexpr ArrayLiteral(const T (&value)[_Size])
		{
			for (SizeType i = 0; i < _Size; ++i)
			{
				m_Value[i] = value[i];
			}
		}
	};

	template<typename T, SizeType Size1, SizeType Size2>
	inline constexpr ArrayLiteral<T, Size1 + Size2> ConcatArrayLiteral(const ArrayLiteral<T, Size1>& array_literal1, const ArrayLiteral<T, Size2>& array_literal2)
	{
		T result[Size1 + Size2];
		for (SizeType i = 0; i < Size1; ++i)
			result[i] = array_literal1.m_Value[i];
		for (SizeType i = 0; i < Size2; ++i)
			result[Size1 + i] = array_literal2.m_Value[i];
		return ArrayLiteral<T, Size1 + Size2>(result);
	}

	template<typename T, SizeType Size1, SizeType Size2, SizeType... RestSizes>
	inline constexpr ArrayLiteral<T, Size1 + Size2 + (RestSizes + ...)> ConcatArrayLiteral(const ArrayLiteral<T, Size1>& array_literal1, const ArrayLiteral<T, Size2>& array_literal2, const ArrayLiteral<T, RestSizes>&... rest_array_literals)
	{
		return ConcatArrayLiteral(ConcatArrayLiteral(array_literal1, array_literal2), rest_array_literals...);
	}

	template<typename T, SizeType Size1, SizeType Size2>
	inline constexpr ArrayLiteral<T, Size1 + Size2 - 1> ConcatCStringLiteral(const ArrayLiteral<T, Size1>& array_literal1, const ArrayLiteral<T, Size2>& array_literal2)
	{
		T result[Size1 + Size2 - 1];
		for (SizeType i = 0; i < Size1 - 1; ++i)
			result[i] = array_literal1.m_Value[i];
		for (SizeType i = 0; i < Size2; ++i)
			result[Size1 - 1 + i] = array_literal2.m_Value[i];
		return ArrayLiteral<T, Size1 + Size2 - 1>(result);
	}

	template<typename T, SizeType Size1, SizeType Size2, SizeType... RestSizes>
	inline constexpr ArrayLiteral<T, Size1 + Size2 + (RestSizes + ...) - sizeof...(RestSizes) - 1> ConcatCStringLiteral(const ArrayLiteral<T, Size1>& array_literal1, const ArrayLiteral<T, Size2>& array_literal2, const ArrayLiteral<T, RestSizes>&... rest_array_literals)
	{
		return ConcatCStringLiteral(ConcatCStringLiteral(array_literal1, array_literal2), rest_array_literals...);
	}

	template<typename T, SizeType SepSize, SizeType Size1, SizeType Size2>
	inline constexpr ArrayLiteral<T, Size1 + Size2 + SepSize - 2> JoinCStringLiterals(const ArrayLiteral<T, SepSize>& separator, const ArrayLiteral<T, Size1>& str1, const ArrayLiteral<T, Size2>& str2)
	{
		return ConcatCStringLiteral(str1, separator, str2);
	}

	template<typename T, SizeType SepSize, SizeType Size1, SizeType Size2, SizeType... RestSizes>
	inline constexpr auto JoinCStringLiterals(const ArrayLiteral<T, SepSize>& separator, const ArrayLiteral<T, Size1>& str1, const ArrayLiteral<T, Size2>& str2, const ArrayLiteral<T, RestSizes>&... rest)
	{
		return JoinCStringLiterals(separator, JoinCStringLiterals(separator, str1, str2), rest...);
	}

	template<std::integral auto Value, typename CharType, SizeType Base = 10>
	inline constexpr auto IntegerToCStringLiteral()
	{
		static_assert(Base >= 2 && Base <= 36, "Base must be between 2 and 36");

		using T = decltype(Value);
		using UT = std::make_unsigned_t<T>;

		// compute the number of digits at compile time
		constexpr bool is_negative = std::is_signed_v<T> && (Value < 0);
		constexpr UT abs_value = []() constexpr -> UT {
			if constexpr (std::is_signed_v<T> && (Value < 0))
				return static_cast<UT>(-(Value + 1)) + 1;
			else
				return static_cast<UT>(Value);
		}();

		constexpr SizeType digit_count = []() constexpr -> SizeType {
			if constexpr (abs_value == 0)
				return 1;
			SizeType n = 0;
			UT v = abs_value;
			while (v > 0)
			{
				++n;
				v /= static_cast<UT>(Base);
			}
			return n;
		}();

		constexpr SizeType sign_count = is_negative ? 1 : 0;
		constexpr SizeType total_size = sign_count + digit_count + 1;	 // +1 for null terminator

		constexpr CharType digits[] = {
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
			'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
			'u', 'v', 'w', 'x', 'y', 'z'};

		CharType result[total_size]{};
		SizeType pos = total_size - 1;
		result[pos] = static_cast<CharType>(0);	   // null terminator

		UT v = abs_value;
		if (v == 0)
		{
			result[--pos] = static_cast<CharType>('0');
		}
		else
		{
			while (v > 0)
			{
				result[--pos] = digits[v % static_cast<UT>(Base)];
				v /= static_cast<UT>(Base);
			}
		}

		if constexpr (is_negative)
			result[--pos] = static_cast<CharType>('-');

		return ArrayLiteral<CharType, total_size>(result);
	}

	template<bool Value, typename CharType>
	inline constexpr auto BoolToCStringLiteral()
	{
		if constexpr (Value)
			return ArrayLiteral<CharType, 5>({CharType('t'), CharType('r'), CharType('u'), CharType('e'), CharType(0)});
		else
			return ArrayLiteral<CharType, 6>({CharType('f'), CharType('a'), CharType('l'), CharType('s'), CharType('e'), CharType(0)});
	}
}

/*!
@}
*/