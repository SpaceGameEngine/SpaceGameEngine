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
							has_opt = false;
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

	/*!
	@}
	*/
}