/*
Copyright 2022 creatorlxd

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
#include "Error.h"
#include "Meta/Trait.hpp"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	struct OptionalIsEmptyError;
	struct OptionalIsNotEmptyError;

	namespace OptionalTag
	{
		struct EmptyOptionalTag
		{
		};

		inline constexpr const EmptyOptionalTag EmptyOptional;

		struct DefaultValueTag
		{
		};

		inline constexpr const DefaultValueTag DefaultValue;
	}

	template<typename T>
	class Optional
	{
	public:
		inline Optional()
			: m_HasValue(false)
		{
		}

		template<typename... Args,
				 typename = std::enable_if_t<
					 !(sizeof...(Args) == 1 &&
					   (std::is_same_v<RemoveCVRefType<Args>, Optional> && ...)),
					 void>>
		inline Optional(Args&&... args)
			: m_HasValue(true)
		{
			new (m_Value) T(std::forward<Args>(args)...);
		}

		inline Optional(OptionalTag::EmptyOptionalTag)
			: m_HasValue(false)
		{
		}

		inline Optional(OptionalTag::DefaultValueTag)
			: m_HasValue(true)
		{
			new (m_Value) T();
		}

		inline ~Optional()
		{
			if (m_HasValue)
				reinterpret_cast<T*>(m_Value)->~T();
		}

		inline Optional(const Optional& opt)
			: m_HasValue(opt.m_HasValue)
		{
			if (m_HasValue)
				new (m_Value) T(opt.Get());
		}

		inline Optional(Optional&& opt)
			: m_HasValue(opt.m_HasValue)
		{
			if (m_HasValue)
			{
				new (m_Value) T(std::move(opt.Get()));
				opt.Clear();
			}
		}

		inline Optional& operator=(const Optional& opt)
		{
			SGE_ASSERT(SelfAssignmentError, this, &opt);

			if (m_HasValue && opt.m_HasValue)
				Get() = opt.Get();
			else if (m_HasValue && (!opt.m_HasValue))
				Clear();
			else if ((!m_HasValue) && opt.m_HasValue)
				Set(opt.Get());

			return *this;
		}

		inline Optional& operator=(Optional&& opt)
		{
			SGE_ASSERT(SelfAssignmentError, this, &opt);

			if (m_HasValue && opt.m_HasValue)
			{
				Get() = std::move(opt.Get());
				opt.Clear();
			}
			else if (m_HasValue && (!opt.m_HasValue))
				Clear();
			else if ((!m_HasValue) && opt.m_HasValue)
			{
				Set(std::move(opt.Get()));
				opt.Clear();
			}

			return *this;
		}

		inline bool HasValue() const
		{
			return m_HasValue;
		}

		inline T& Get()
		{
			SGE_ASSERT(OptionalIsEmptyError, *this);
			return *reinterpret_cast<T*>(m_Value);
		}

		inline const T& Get() const
		{
			SGE_ASSERT(OptionalIsEmptyError, *this);
			return *reinterpret_cast<const T*>(m_Value);
		}

		template<typename... Args>
		inline void Set(Args&&... args)
		{
			SGE_ASSERT(OptionalIsNotEmptyError, *this);
			m_HasValue = true;
			new (m_Value) T(std::forward<Args>(args)...);
		}

		inline void Clear()
		{
			if (m_HasValue)
			{
				reinterpret_cast<T*>(m_Value)->~T();
				m_HasValue = false;
			}
		}

		inline bool operator==(const Optional& opt) const
		{
			if (m_HasValue != opt.m_HasValue)
				return false;
			else if (m_HasValue)
				return Get() == opt.Get();
			else
				return true;
		}

		inline bool operator!=(const Optional& opt) const
		{
			if (m_HasValue != opt.m_HasValue)
				return true;
			else if (m_HasValue)
				return Get() != opt.Get();
			else
				return false;
		}

	private:
		bool m_HasValue;
		Byte m_Value[sizeof(T)];
	};

	struct OptionalIsEmptyError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("Optional is empty");
		template<typename T>
		inline static bool Judge(const Optional<T>& optional)
		{
			return !optional.HasValue();
		}
	};

	struct OptionalIsNotEmptyError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("Optional is not empty");
		template<typename T>
		inline static bool Judge(const Optional<T>& optional)
		{
			return optional.HasValue();
		}
	};
}

/*!
@}
*/