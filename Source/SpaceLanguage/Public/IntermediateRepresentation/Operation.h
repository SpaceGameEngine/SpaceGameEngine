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

	enum class StorageType : UInt8
	{
		Constant = 1,
		Global = 6,
		Local = 10,
		Function = 16
	};

	namespace StorageTypeMasks
	{
		inline constexpr const UInt8 Constant = 1;
		inline constexpr const UInt8 Variable = 2;
		inline constexpr const UInt8 Global = 6;
		inline constexpr const UInt8 Local = 10;
		inline constexpr const UInt8 Function = 16;
	}

	struct InvalidStorageTypeError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The StorageType is invalid.");
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

	enum class OperationType : UInt8
	{
		Set = 0,
		NewLocal = 1,
		DeleteLocal = 2,
		Push = 3,
		Pop = 4,
		Copy = 5,
		Label = 6,
		Goto = 7,
		If = 8,
		Call = 9,
		Return = 10,
		ExternalCallArgument = 11,
		ExternalCall = 12,
		GetReturnValue = 13,
		//todo
	};

	inline constexpr const SizeType OperationTypeSetSize = 14;

	struct InvalidOperationTypeError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The OperationType is invalid.");
		static SPACE_LANGUAGE_API bool Judge(OperationType ot);
	};

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API Vector<UInt8>;
	template struct SPACE_LANGUAGE_API Pair<Vector<UInt8>, String>;
	template class SPACE_LANGUAGE_API HashMap<OperationType, Pair<Vector<UInt8>, String>>;
#endif

	class SPACE_LANGUAGE_API OperationTypeSet : public UncopyableAndUnmovable, public Singleton<OperationTypeSet>
	{
	private:
		OperationTypeSet();

	public:
		friend DefaultAllocator;

		const Vector<UInt8>& GetArguments(OperationType type) const;
		const String& GetName(OperationType type) const;

	private:
		HashMap<OperationType, Pair<Vector<UInt8>, String>> m_Content;
	};

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API Vector<Variable>;
#endif

	class SPACE_LANGUAGE_API Operation
	{
	public:
		Operation() = delete;
		Operation(OperationType type, const Vector<Variable>& arguments);

		OperationType GetType() const;
		const Vector<Variable>& GetArguments() const;

		bool operator==(const Operation& o) const;
		bool operator!=(const Operation& o) const;

	private:
		OperationType m_Type;
		Vector<Variable> m_Arguments;
	};

	struct InvalidOperationError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The Operation is invalid.");
		static SPACE_LANGUAGE_API bool Judge(const Operation& o);
	};

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API Vector<const Type*>;
	template class SPACE_LANGUAGE_API Vector<Operation>;
#endif

	class SPACE_LANGUAGE_API Function
	{
	public:
		Function() = delete;
		Function(const Vector<const Type*>& parameter_types, const Type& result_type, SizeType idx, const Vector<Operation>& operations);

		const Vector<const Type*>& GetParameterTypes() const;
		const Type& GetResultType() const;
		UInt64 GetIndex() const;
		const Vector<Operation>& GetOperations() const;

		Variable ToVariable() const;
		explicit operator Variable() const;

		bool operator==(const IntermediateRepresentation::Function& func) const;
		bool operator!=(const IntermediateRepresentation::Function& func) const;

	private:
		Vector<const Type*> m_ParameterTypes;
		const Type* m_pResultType;
		UInt64 m_Index;
		Vector<Operation> m_Operations;
	};
}

/*!
@}
*/