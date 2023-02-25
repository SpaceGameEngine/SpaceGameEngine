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
#include "ForwardDefinition.hpp"
#include "SGEStringForward.h"
#include "Platform.hpp"
#include "Meta/Trait.hpp"
#include "Utility/Utility.hpp"
#include "CommonAPI.h"
#include "Utility/DebugInformation.h"
#include <type_traits>
#include <string>
#include <concepts>

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	using ErrorMessageChar = Char8;
	using ErrorMessageString = std::u8string;
#define SGE_ESTR(str) SGE_U8STR(str)
#define SGE_ESTR_TO_UTF8(str) str

	COMMON_API void ThrowError(const ErrorMessageChar* error_msg, DebugInformation debug_info);

	/*!
	@brief Can check whether a type is a error type or not.But need to specify the Judge function's arguments' types.
	*/
	template<typename T, typename... Args>
	concept IsError = requires(Args... args) {
						  {
							  T::sm_pContent
							  }
							  -> std::convertible_to<const ErrorMessageChar*>;
						  {
							  T::Judge(args...)
							  }
							  -> std::same_as<bool>;
					  };

	struct NullPointerError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("Pointer can not be null");
		template<typename T>
		inline static bool Judge(const T ptr)
		{
			return ptr == nullptr;
		}
	};

	struct InvalidValueError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The value is invalid");
		template<typename T1, typename T2, typename T3>
		inline static bool Judge(T1&& val, T2&& min_val, T3&& max_val)
		{
			return !(val >= min_val && val <= max_val);
		}
	};

	struct SelfAssignmentError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("a self assignment has occured");
		template<typename T>
		inline static bool Judge(const T* pthis, const T* ptr)
		{
			return pthis == ptr;
		}
	};

	template<typename ErrorType>
	struct Assert : public UncopyableAndUnmovable
	{
	public:
		inline Assert() = delete;

		template<typename... Args>
		explicit inline Assert(const DebugInformation& debug_info, Args&&... args)
			: m_IsDefault(true), m_DebugInformation(debug_info)
		{
#ifdef SGE_DEBUG
			m_Result = ErrorType::Judge(std::forward<Args>(args)...);
#endif
		}

		inline ~Assert()
		{
#ifdef SGE_DEBUG
			if (m_IsDefault && m_Result)
				ThrowError(ErrorType::sm_pContent, m_DebugInformation);
#endif
		}

		template<typename Func, typename... Args>
		inline void Handle(Func&& func, Args&&... args)
		{
#ifdef SGE_DEBUG
			m_IsDefault = false;
			if (m_Result)
				func(std::forward<Args>(args)...);
#endif
		}

		template<typename Func, typename... Args>
		inline Assert& Run(Func&& func, Args&&... args)
		{
#ifdef SGE_DEBUG
			if (m_Result)
				func(std::forward<Args>(args)...);
#endif
			return *this;
		}

	private:
		bool m_IsDefault;
		bool m_Result;
		DebugInformation m_DebugInformation;
	};

	template<typename ErrorType>
	struct Check : public UncopyableAndUnmovable
	{
	public:
		inline Check() = delete;

		template<typename... Args>
		explicit inline Check(const DebugInformation& debug_info, Args&&... args)
			: m_IsDefault(true), m_DebugInformation(debug_info)
		{
			m_Result = ErrorType::Judge(std::forward<Args>(args)...);
		}

		inline ~Check()
		{
			if (m_IsDefault && m_Result)
				ThrowError(ErrorType::sm_pContent, m_DebugInformation);
		}

		template<typename Func, typename... Args>
		inline void Handle(Func&& func, Args&&... args)
		{
			m_IsDefault = false;
			if (m_Result)
				func(std::forward<Args>(args)...);
		}

		template<typename Func, typename... Args>
		inline Check& Run(Func&& func, Args&&... args)
		{
			if (m_Result)
				func(std::forward<Args>(args)...);
			return *this;
		}

	private:
		bool m_IsDefault;
		bool m_Result;
		DebugInformation m_DebugInformation;
	};

	/*!
	@brief assert condition when debug
	*/
#define SGE_ASSERT(error_type, ...) \
	SpaceGameEngine::Assert<error_type>(SGE_DEBUG_INFORMATION, ##__VA_ARGS__)

	/*!
	@brief check condition
	*/
#define SGE_CHECK(error_type, ...) \
	SpaceGameEngine::Check<error_type>(SGE_DEBUG_INFORMATION, ##__VA_ARGS__)

}

/*!
@}
*/