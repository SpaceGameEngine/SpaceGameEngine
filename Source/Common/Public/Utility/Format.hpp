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
#include "SGEString.hpp"
#include "Container/Vector.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	template<typename T, typename Trait, typename Allocator>
	inline bool IsFormatString(const StringCore<T, Trait, Allocator>& str)
	{
		static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

		if (str.GetSize() == 0)
			return true;

		bool is_in_arg = false;
		bool has_opt = false;
		UInt8 idx_status = 0;	 //0 : unknown ; 1 : no idx ; 2 : has idx
		SizeType num_cnt = 0;

		if constexpr (std::is_same_v<Trait, UCS2Trait>)
		{
			for (auto iter = str.GetConstBegin(); iter != str.GetConstEnd(); ++iter)
			{
				if (!is_in_arg)
				{
					if (*iter == SGE_WSTR('{'))
					{
						auto next = iter + 1;
						if (next == str.GetConstEnd())
							return false;
						if (*next != SGE_WSTR('{'))	   //{{
							is_in_arg = true;
						else
							++iter;
					}
					else if (*iter == SGE_WSTR('}'))
					{
						auto next = iter + 1;
						if (next == str.GetConstEnd())
							return false;
						if (*next != SGE_WSTR('}'))	   //}}
							return false;
						else
							++iter;
					}
				}
				else
				{
					if (!has_opt)
					{
						if (*iter == SGE_WSTR(':'))
							has_opt = true;
						else if (*iter == SGE_WSTR('}'))
						{
							if (num_cnt)
							{
								if (idx_status == 1)	//1 mean no idx
									return false;
								else
									idx_status = 2;
							}
							else
							{
								if (idx_status == 2)	//2 mean has idx
									return false;
								else
									idx_status = 1;
							}
							num_cnt = 0;
							is_in_arg = false;
						}
						else if (!IsNumericalCharacter<T, Trait>(*iter))
							return false;
						else
							num_cnt += 1;
					}
					else
					{
						if (*iter == SGE_WSTR('}'))
						{
							if (num_cnt)
							{
								if (idx_status == 1)	//1 mean no idx
									return false;
								else
									idx_status = 2;
							}
							else
							{
								if (idx_status == 2)	//2 mean has idx
									return false;
								else
									idx_status = 1;
							}
							num_cnt = 0;
							is_in_arg = false;
							has_opt = false;
						}
					}
				}
			}
			if (is_in_arg)
				return false;
		}
		else	//UTF8Trait
		{
			static_assert(std::is_same_v<Trait, UTF8Trait>, "unsupported CharTrait");
			for (auto iter = str.GetConstBegin(); iter != str.GetConstEnd(); ++iter)
			{
				if (!is_in_arg)
				{
					if ((**iter) == SGE_U8STR('{'))
					{
						auto next = iter + 1;
						if (next == str.GetConstEnd())
							return false;
						if ((**next) != SGE_U8STR('{'))	   //{{
							is_in_arg = true;
						else
							++iter;
					}
					else if ((**iter) == SGE_U8STR('}'))
					{
						auto next = iter + 1;
						if (next == str.GetConstEnd())
							return false;
						if ((**next) != SGE_U8STR('}'))	   //}}
							return false;
						else
							++iter;
					}
				}
				else
				{
					if (!has_opt)
					{
						if ((**iter) == SGE_U8STR(':'))
							has_opt = true;
						else if ((**iter) == SGE_U8STR('}'))
						{
							if (num_cnt)
							{
								if (idx_status == 1)	//1 mean no idx
									return false;
								else
									idx_status = 2;
							}
							else
							{
								if (idx_status == 2)	//2 mean has idx
									return false;
								else
									idx_status = 1;
							}
							num_cnt = 0;
							is_in_arg = false;
							has_opt = false;
						}
						else if (!IsNumericalCharacter<T, Trait>(*iter))
							return false;
						else
							num_cnt += 1;
					}
					else
					{
						if ((**iter) == SGE_U8STR('}'))
						{
							if (num_cnt)
							{
								if (idx_status == 1)	//1 mean no idx
									return false;
								else
									idx_status = 2;
							}
							else
							{
								if (idx_status == 2)	//2 mean has idx
									return false;
								else
									idx_status = 1;
							}
							num_cnt = 0;
							is_in_arg = false;
							has_opt = false;
						}
					}
				}
			}
			if (is_in_arg)
				return false;
		}

		return true;
	}

	struct NonFormatStringError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The string is not format string.");
		template<typename T, typename Trait, typename Allocator>
		inline static bool Judge(const StringCore<T, Trait, Allocator>& str)
		{
			static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

			return !IsFormatString(str);
		}
	};

	struct InvalidNumberBaseOptionError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The option for the number base is invalid, only B/D/H can be chosen.");
		template<typename T, typename Trait, typename Allocator>
		inline static bool Judge(const StringCore<T, Trait, Allocator>& str)
		{
			static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

			if (str.GetSize() == 0)
				return false;
			else if (str.GetSize() > 1)
				return true;

			if constexpr (std::is_same_v<Trait, UCS2Trait>)
			{
				return str[0] != SGE_WSTR('B') && str[0] != SGE_WSTR('D') && str[0] != SGE_WSTR('H');
			}
			else	//UTF8Trait
			{
				static_assert(std::is_same_v<Trait, UTF8Trait>, "unsupported CharTrait");

				return *(str[0]) != SGE_U8STR('B') && *(str[0]) != SGE_U8STR('D') && *(str[0]) != SGE_U8STR('H');
			}
		}
	};

	template<typename T, typename Trait, typename Allocator, typename ValueType>
	struct Formatter
	{
		static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

		inline static StringCore<T, Trait, Allocator> Get(const ValueType& val, const StringCore<T, Trait, Allocator>& opt)
		{
			if constexpr (std::is_integral_v<ValueType>)
			{
				SGE_ASSERT(InvalidNumberBaseOptionError, opt);
				NumberBase base = NumberBase::Decimal;
				if (opt.GetSize())
				{
					if constexpr (std::is_same_v<Trait, UCS2Trait>)
					{
						if (opt[0] == SGE_WSTR('B'))
							base = NumberBase::Binary;
						else if (opt[0] == SGE_WSTR('D'))
							base = NumberBase::Decimal;
						else if (opt[0] == SGE_WSTR('H'))
							base = NumberBase::Hex;
					}
					else	//UTF8Trait
					{
						static_assert(std::is_same_v<Trait, UTF8Trait>, "unsupported CharTrait");

						if (*(opt[0]) == SGE_U8STR('B'))
							base = NumberBase::Binary;
						else if (*(opt[0]) == SGE_U8STR('D'))
							base = NumberBase::Decimal;
						else if (*(opt[0]) == SGE_U8STR('H'))
							base = NumberBase::Hex;
					}
				}
				return ToString<StringCore<T, Trait, Allocator>, ValueType>(val, base);
			}
			else
				return ToString<StringCore<T, Trait, Allocator>, ValueType>(val);
		}
	};

	template<typename T, typename Trait, typename Allocator>
	struct Formatter<T, Trait, Allocator, T*>
	{
		static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

		inline static StringCore<T, Trait, Allocator> Get(const T* pstr, const StringCore<T, Trait, Allocator>& opt)
		{
			SGE_ASSERT(NullPointerError, pstr);
			return StringCore<T, Trait, Allocator>(pstr);
		}
	};

	template<typename T, typename Trait, typename Allocator>
	struct Formatter<T, Trait, Allocator, float>
	{
		static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

		inline static StringCore<T, Trait, Allocator> Get(float f, const StringCore<T, Trait, Allocator>& opt)
		{
			if (opt.GetSize())
			{
				SGE_ASSERT(NonUnsignedNumericalStringError, opt);
				return ToString<StringCore<T, Trait, Allocator>, float>(f, StringTo<StringCore<T, Trait, Allocator>, SizeType>(opt));
			}
			else
				return ToString<StringCore<T, Trait, Allocator>, float>(f);
		}
	};

	template<typename T, typename Trait, typename Allocator>
	struct Formatter<T, Trait, Allocator, double>
	{
		static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

		inline static StringCore<T, Trait, Allocator> Get(double d, const StringCore<T, Trait, Allocator>& opt)
		{
			if (opt.GetSize())
			{
				SGE_ASSERT(NonUnsignedNumericalStringError, opt);
				return ToString<StringCore<T, Trait, Allocator>, double>(d, StringTo<StringCore<T, Trait, Allocator>, SizeType>(opt));
			}
			else
				return ToString<StringCore<T, Trait, Allocator>, double>(d);
		}
	};

	namespace FormatCore
	{
		template<typename T, typename Trait, typename Allocator>
		inline void DoFormat(Vector<Pair<SizeType, StringCore<T, Trait, Allocator>>>& arg_strs, SizeType less, SizeType idx)
		{
			static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

			if (less == 0)
				return;
			for (auto iter = arg_strs.GetBegin(); iter != arg_strs.GetEnd(); ++iter)
			{
				if (iter->m_First >= idx)
					iter->m_Second.Clear();
			}
		}

		template<typename T, typename Trait, typename Allocator, typename Arg0, typename... Args>
		inline void DoFormat(Vector<Pair<SizeType, StringCore<T, Trait, Allocator>>>& arg_strs, SizeType less, SizeType idx, const Arg0& arg0, const Args&... args)
		{
			static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");

			if (less == 0)
				return;
			for (auto iter = arg_strs.GetBegin(); iter != arg_strs.GetEnd(); ++iter)
			{
				if (iter->m_First == idx)
				{
					iter->m_Second = Formatter<T, Trait, Allocator, Arg0>::Get(arg0, iter->m_Second);
					less -= 1;
				}
			}
			DoFormat<T, Trait, Allocator>(arg_strs, less, idx + 1, args...);
		}
	}

	template<typename T, typename Trait, typename Allocator, typename... Args>
	inline StringCore<T, Trait, Allocator> Format(const StringCore<T, Trait, Allocator>& str, const Args&... args)
	{
		static_assert(std::is_same_v<T, typename Trait::ValueType>, "invalid trait : the value type is different");
		SGE_ASSERT(NonFormatStringError, str);

		StringCore<T, Trait, Allocator> re;
		Vector<StringCore<T, Trait, Allocator>> normal_strs;
		Vector<Pair<SizeType, StringCore<T, Trait, Allocator>>> arg_strs;
		bool is_in_arg = false;
		bool has_opt = false;
		StringCore<T, Trait, Allocator> str_buf;

		if constexpr (std::is_same_v<Trait, UCS2Trait>)
		{
			for (auto iter = str.GetConstBegin(); iter != str.GetConstEnd(); ++iter)
			{
				if (!is_in_arg)
				{
					if (*iter == SGE_WSTR('{'))
					{
						auto next = iter + 1;
						if (*next == SGE_WSTR('{'))	   //{{
						{
							str_buf += SGE_WSTR('{');
							++iter;
						}
						else
						{
							normal_strs.PushBack(str_buf);
							if (str_buf.GetSize())
								str_buf.Clear();
							is_in_arg = true;
							arg_strs.EmplaceBack();
						}
					}
					else if (*iter == SGE_WSTR('}'))	//}}	after the NonFormatString checking, the '}' outside the argument must be "}}"
					{
						str_buf += SGE_WSTR('}');
						++iter;
					}
					else
					{
						str_buf += *iter;
					}
				}
				else
				{
					if (!has_opt)
					{
						if (*iter == SGE_WSTR(':'))
						{
							if (str_buf.GetSize())
							{
								arg_strs[arg_strs.GetSize() - 1].m_First = StringTo<StringCore<T, Trait, Allocator>, SizeType>(str_buf);
								str_buf.Clear();
							}
							else
							{
								arg_strs[arg_strs.GetSize() - 1].m_First = arg_strs.GetSize() - 1;
							}
							has_opt = true;
						}
						else if (*iter == SGE_WSTR('}'))
						{
							if (str_buf.GetSize())
							{
								arg_strs[arg_strs.GetSize() - 1].m_First = StringTo<StringCore<T, Trait, Allocator>, SizeType>(str_buf);
								str_buf.Clear();
							}
							else
							{
								arg_strs[arg_strs.GetSize() - 1].m_First = arg_strs.GetSize() - 1;
							}
							is_in_arg = false;
						}
						else
						{
							str_buf += *iter;
						}
					}
					else
					{
						if (*iter == SGE_WSTR('}'))
						{
							if (str_buf.GetSize())
							{
								arg_strs[arg_strs.GetSize() - 1].m_Second = str_buf;
								str_buf.Clear();
							}
							is_in_arg = false;
							has_opt = false;
						}
						else
						{
							str_buf += *iter;
						}
					}
				}
			}
		}
		else	//UTF8Trait
		{
			static_assert(std::is_same_v<Trait, UTF8Trait>, "unsupported CharTrait");

			for (auto iter = str.GetConstBegin(); iter != str.GetConstEnd(); ++iter)
			{
				if (!is_in_arg)
				{
					if ((**iter) == SGE_U8STR('{'))
					{
						auto next = iter + 1;
						if ((**next) == SGE_U8STR('{'))	   //{{
						{
							str_buf += SGE_U8STR('{');
							++iter;
						}
						else
						{
							normal_strs.PushBack(str_buf);
							if (str_buf.GetSize())
								str_buf.Clear();
							is_in_arg = true;
							arg_strs.EmplaceBack();
						}
					}
					else if ((**iter) == SGE_U8STR('}'))	//}}	after the NonFormatString checking, the '}' outside the argument must be "}}"
					{
						str_buf += SGE_U8STR('}');
						++iter;
					}
					else
					{
						str_buf += StringCore<T, Trait, Allocator>(1, *iter);
					}
				}
				else
				{
					if (!has_opt)
					{
						if ((**iter) == SGE_U8STR(':'))
						{
							if (str_buf.GetSize())
							{
								arg_strs[arg_strs.GetSize() - 1].m_First = StringTo<StringCore<T, Trait, Allocator>, SizeType>(str_buf);
								str_buf.Clear();
							}
							else
							{
								arg_strs[arg_strs.GetSize() - 1].m_First = arg_strs.GetSize() - 1;
							}
							has_opt = true;
						}
						else if ((**iter) == SGE_U8STR('}'))
						{
							if (str_buf.GetSize())
							{
								arg_strs[arg_strs.GetSize() - 1].m_First = StringTo<StringCore<T, Trait, Allocator>, SizeType>(str_buf);
								str_buf.Clear();
							}
							else
							{
								arg_strs[arg_strs.GetSize() - 1].m_First = arg_strs.GetSize() - 1;
							}
							is_in_arg = false;
						}
						else
						{
							str_buf += StringCore<T, Trait, Allocator>(1, *iter);
						}
					}
					else
					{
						if ((**iter) == SGE_U8STR('}'))
						{
							if (str_buf.GetSize())
							{
								arg_strs[arg_strs.GetSize() - 1].m_Second = str_buf;
								str_buf.Clear();
							}
							is_in_arg = false;
							has_opt = false;
						}
						else
						{
							str_buf += StringCore<T, Trait, Allocator>(1, *iter);
						}
					}
				}
			}
		}
		if (str_buf.GetSize())
			normal_strs.PushBack(std::move(str_buf));

		FormatCore::DoFormat(arg_strs, arg_strs.GetSize(), 0, (std::decay_t<Args>)args...);

		auto iter_a = arg_strs.GetConstBegin();
		auto iter_n = normal_strs.GetConstBegin();
		for (; iter_a != arg_strs.GetConstEnd(); ++iter_a, ++iter_n)
		{
			re += *iter_n;
			re += iter_a->m_Second;
		}
		if (iter_n != normal_strs.GetConstEnd())
			re += *iter_n;

		return re;
	}

	/*!
	@}
	*/
}