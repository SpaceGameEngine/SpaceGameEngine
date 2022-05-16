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

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template struct SPACE_LANGUAGE_API Pair<SizeType, String>;
	template class SPACE_LANGUAGE_API HashMap<BaseType, Pair<SizeType, String>>;
#endif

	class SPACE_LANGUAGE_API BaseTypeSet : public UncopyableAndUnmovable, public Singleton<BaseTypeSet>
	{
	private:
		BaseTypeSet();

	public:
		friend DefaultAllocator;

		SizeType GetSize(BaseType bt) const;
		const String& GetName(BaseType bt) const;

	private:
		HashMap<BaseType, Pair<SizeType, String>> m_Content;
	};

	struct InvalidBaseTypeError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The BaseType is invalid.");
		static SPACE_LANGUAGE_API bool Judge(BaseType bt);
	};

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API Vector<BaseType>;
#endif

	class SPACE_LANGUAGE_API Type
	{
	public:
		Type();
		Type(BaseType bt);
		Type(std::initializer_list<BaseType> bts);

		const Vector<BaseType>& GetContent() const;
		SizeType GetSize() const;

		Type operator+(const Type& t) const;
		Type& operator+=(const Type& t);

		bool operator==(const Type& t) const;
		bool operator!=(const Type& t) const;

	private:
		Vector<BaseType> m_Content;
		SizeType m_Size;
	};

	enum class StorageType
	{
		Global = 0,
		Local = 1,
		Register = 2
	};

	class SPACE_LANGUAGE_API Variable
	{
	public:
		Variable(const Type& type, StorageType st, SizeType idx);

		const Type& GetType() const;
		StorageType GetStorageType() const;
		SizeType GetIndex() const;

		bool operator==(const Variable& v) const;
		bool operator!=(const Variable& v) const;

	private:
		const Type& m_Type;
		StorageType m_StorageType;
		SizeType m_Index;
	};
}

/*!
@}
*/