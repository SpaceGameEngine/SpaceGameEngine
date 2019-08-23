/*
Copyright 2019 creatorlxd

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
#include "SGEString.h"
#include "Platform.hpp"
#include "Utility/Utility.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

#if defined(DEBUG) || defined(_DEBUG)
#define SGE_DEBUG
#endif

	struct DebugInformation
	{
		const TChar* m_pFileName;
		const TChar* m_pFunctionName;
		UInt32 m_LineNumber;

		DebugInformation(const TChar* file_name, const TChar* func_name, UInt32 line_number);
	};

#ifdef SGE_USE_WIDE_CHAR
#define SGE_FILE __FILEW__
#define SGE_FUNCTION __FUNCTIONW__
#else
#define SGE_FILE __FILE__
#define SGE_FUNCTION __FUNCTION__
#endif
#define SGE_LINE __LINE__
#define SGE_DEBUG_INFORMATION SpaceGameEngine::DebugInformation(SGE_FILE,SGE_FUNCTION,SGE_LINE)

	void ThrowError(const TChar* error_msg, DebugInformation debug_info);

	/*!
	@file
	@todo add Error as a concept when c++20 can be used
	*/

	/*!
	@brief a example of error class
	*/
	struct BlankError
	{
		/*!
		@brief error message
		*/
		inline static const TChar sm_pContent[] = SGE_TSTR("Blank Error");
		/*!
		@brief a function for judging the condition
		@retval true error happend
		@retval false no error happend
		*/
		static bool Judge();
	};

	struct NullPointerError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Pointer can not be null");
		static bool Judge(const void* ptr);
	};

	struct InvalidSizeError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The size is invalid");
		static bool Judge(SizeType size, SizeType min_size, SizeType max_size);
	};

	template<typename ErrorType>
	struct Assert :public Uncopyable
	{
	public:
		inline Assert() = delete;

		template<typename... Args>
		explicit inline Assert(const DebugInformation& debug_info, Args&&... args)
			:m_IsDefault(true), m_DebugInformation(debug_info)
		{
#ifdef SGE_DEBUG
			m_Result = ErrorType::Judge(std::forward<Args>(args)...);
#endif
		}

		inline ~Assert()
		{
#ifdef SGE_DEBUG
			if (m_IsDefault&&m_Result)
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
	private:
		bool m_IsDefault;
		bool m_Result;
		DebugInformation m_DebugInformation;
	};

	template<typename ErrorType>
	struct Check :public Uncopyable
	{
	public:
		inline Check() = delete;

		template<typename... Args>
		explicit inline Check(const DebugInformation& debug_info, Args&&... args)
			:m_IsDefault(true), m_DebugInformation(debug_info)
		{
			m_Result = ErrorType::Judge(std::forward<Args>(args)...);
		}

		inline ~Check()
		{
			if (m_IsDefault&&m_Result)
				ThrowError(ErrorType::sm_pContent, m_DebugInformation);
		}

		template<typename Func, typename... Args>
		inline void Handle(Func&& func, Args&&... args)
		{
			m_IsDefault = false;
			if (m_Result)
				func(std::forward<Args>(args)...);
		}
	private:
		bool m_IsDefault;
		bool m_Result;
		DebugInformation m_DebugInformation;
	};

	/*!
	@brief assert condition when debug
	*/
#define SGE_ASSERT(error_type,...)\
SpaceGameEngine::Assert<error_type>(SGE_DEBUG_INFORMATION,##__VA_ARGS__)

	/*!
	@brief check condition
	*/
#define SGE_CHECK(error_type,...)\
SpaceGameEngine::Check<error_type>(SGE_DEBUG_INFORMATION,##__VA_ARGS__)

	/*!
	@}
	*/
}