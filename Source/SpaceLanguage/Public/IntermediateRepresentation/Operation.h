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
		Type() = delete;
		Type(BaseType bt);
		Type(std::initializer_list<BaseType> bts);
		Type(const Vector<BaseType>& bts);

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
		Constant = 0,
		Global = 1,
		Local = 2,
		Function = 3
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
		NewLocal = 0,
		DeleteLocal = 1,
		Push = 2,
		Pop = 3,
		Copy = 4,
		//todo
	};

	inline constexpr const SizeType OperationTypeSetSize = 5;

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
	template class SPACE_LANGUAGE_API HashMap<OperationType, Pair<SizeType, String>>;
#endif

	class SPACE_LANGUAGE_API OperationTypeSet : public UncopyableAndUnmovable, public Singleton<OperationTypeSet>
	{
	private:
		OperationTypeSet();

	public:
		friend DefaultAllocator;

		SizeType GetArgumentsSize(OperationType type) const;
		const String& GetName(OperationType type) const;

	private:
		HashMap<OperationType, Pair<SizeType, String>> m_Content;
	};

	struct InvalidOperationTypeError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The OperationType is invalid.");
		static SPACE_LANGUAGE_API bool Judge(OperationType ot);
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