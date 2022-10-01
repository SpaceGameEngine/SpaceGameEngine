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
#include "SpaceLanguageAPI.h"
#include "Container/Vector.hpp"
#include "Container/HashMap.hpp"
#include "Utility/Singleton.hpp"
#include "SGEString.hpp"

/*!
@ingroup SpaceLanguage
@{
*/

namespace SpaceGameEngine::SpaceLanguage::IntermediateRepresentation
{
	enum class BaseType : UInt8
	{
		Void = 0,
		Int8 = 1,
		UInt8 = 2,
		Int16 = 3,
		UInt16 = 4,
		Int32 = 5,
		UInt32 = 6,
		Int64 = 7,
		UInt64 = 8,
		Float = 9,
		Double = 10
	};

	inline constexpr const SizeType BaseTypeSize = 11;

	struct InvalidBaseTypeError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The BaseType is invalid.");
		static SPACE_LANGUAGE_API bool Judge(BaseType bt);
	};

	struct SPACE_LANGUAGE_API BaseTypeInformation
	{
		BaseTypeInformation() = delete;
		BaseTypeInformation(SizeType size, SizeType alignment, const String& name);
		bool operator==(const BaseTypeInformation& bti) const;
		bool operator!=(const BaseTypeInformation& bti) const;

		SizeType m_Size;
		SizeType m_Alignment;
		String m_Name;
	};

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API HashMap<BaseType, BaseTypeInformation>;
#endif

	class SPACE_LANGUAGE_API BaseTypeSet : public UncopyableAndUnmovable, public Singleton<BaseTypeSet>
	{
	private:
		BaseTypeSet();

	public:
		friend DefaultAllocator;

		SizeType GetSize(BaseType bt) const;
		SizeType GetAlignment(BaseType bt) const;
		const String& GetName(BaseType bt) const;

	private:
		HashMap<BaseType, BaseTypeInformation> m_Content;
	};

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API Vector<BaseType>;
#endif

	class SPACE_LANGUAGE_API Type
	{
	public:
		Type() = delete;
		Type(BaseType bt);
		Type(std::initializer_list<BaseType> bts);
		Type(const Vector<BaseType>& bts);

		const Vector<BaseType>& GetContent() const;
		SizeType GetSize() const;
		SizeType GetAlignment() const;

		Type operator+(const Type& t) const;
		Type& operator+=(const Type& t);

		bool operator==(const Type& t) const;
		bool operator!=(const Type& t) const;

	private:
		Vector<BaseType> m_Content;
		SizeType m_Size;
		SizeType m_Alignment;
	};

	SPACE_LANGUAGE_API bool CanConvert(const Type& from, const Type& to);

	namespace BaseTypes
	{
		SPACE_LANGUAGE_API const Type& GetVoidType();
		SPACE_LANGUAGE_API const Type& GetInt8Type();
		SPACE_LANGUAGE_API const Type& GetUInt8Type();
		SPACE_LANGUAGE_API const Type& GetInt16Type();
		SPACE_LANGUAGE_API const Type& GetUInt16Type();
		SPACE_LANGUAGE_API const Type& GetInt32Type();
		SPACE_LANGUAGE_API const Type& GetUInt32Type();
		SPACE_LANGUAGE_API const Type& GetInt64Type();
		SPACE_LANGUAGE_API const Type& GetUInt64Type();
		SPACE_LANGUAGE_API const Type& GetFloatType();
		SPACE_LANGUAGE_API const Type& GetDoubleType();
	}
}

/*!
@}
*/