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
#include "IntermediateRepresentation/Type.h"

/*!
@ingroup SpaceLanguage
@{
*/

namespace SpaceGameEngine::SpaceLanguage::IntermediateRepresentation
{
	enum class StorageType : UInt8
	{
		Constant = 1,
		Global = 6,
		Local = 10,
		Reference = 18,
		Function = 32
	};

	namespace StorageTypeMasks
	{
		inline constexpr const UInt8 Constant = 1;
		inline constexpr const UInt8 Variable = 2;
		inline constexpr const UInt8 Value = 3;
		inline constexpr const UInt8 Global = 6;
		inline constexpr const UInt8 Local = 10;
		inline constexpr const UInt8 Reference = 18;
		inline constexpr const UInt8 Function = 32;
	}

	struct InvalidStorageTypeError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The StorageType is invalid.");
		static SPACE_LANGUAGE_API bool Judge(StorageType st);
	};

	class SPACE_LANGUAGE_API Variable
	{
	public:
		Variable() = delete;
		Variable(const Type& type, StorageType st, SizeType idx);

		const Type& GetType() const;
		StorageType GetStorageType() const;
		UInt64 GetIndex() const;

		bool operator==(const Variable& v) const;
		bool operator!=(const Variable& v) const;

	private:
		const Type* m_pType;
		StorageType m_StorageType;
		UInt64 m_Index;
	};
}

/*!
@}
*/