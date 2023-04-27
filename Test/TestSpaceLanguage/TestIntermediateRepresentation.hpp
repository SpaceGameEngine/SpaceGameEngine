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
#include "gtest/gtest.h"
#include "VirtualMachine/Register.h"
#include "IntermediateRepresentation/Operation.h"
#include "IntermediateRepresentation/RegisterAllocator.h"
#include "IntermediateRepresentation/Translator.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;

TEST(IntermediateRepresentation_BaseTypeSet, Test)
{
	using IntermediateRepresentation::BaseTypeSet;
	using IntermediateRepresentation::BaseType;

	BaseTypeSet& bts = BaseTypeSet::GetSingleton();
	ASSERT_EQ(bts.GetName(BaseType::Void), SGE_STR("Void"));
	ASSERT_EQ(bts.GetSize(BaseType::Void), 0);
	ASSERT_EQ(bts.GetAlignment(BaseType::Void), 0);
	ASSERT_EQ(bts.GetName(BaseType::Int8), SGE_STR("Int8"));
	ASSERT_EQ(bts.GetSize(BaseType::Int8), sizeof(Int8));
	ASSERT_EQ(bts.GetAlignment(BaseType::Int8), alignof(Int8));
	ASSERT_EQ(bts.GetName(BaseType::UInt8), SGE_STR("UInt8"));
	ASSERT_EQ(bts.GetSize(BaseType::UInt8), sizeof(UInt8));
	ASSERT_EQ(bts.GetAlignment(BaseType::UInt8), alignof(UInt8));
	ASSERT_EQ(bts.GetName(BaseType::Int16), SGE_STR("Int16"));
	ASSERT_EQ(bts.GetSize(BaseType::Int16), sizeof(Int16));
	ASSERT_EQ(bts.GetAlignment(BaseType::Int16), alignof(Int16));
	ASSERT_EQ(bts.GetName(BaseType::UInt16), SGE_STR("UInt16"));
	ASSERT_EQ(bts.GetSize(BaseType::UInt16), sizeof(UInt16));
	ASSERT_EQ(bts.GetAlignment(BaseType::UInt16), alignof(UInt16));
	ASSERT_EQ(bts.GetName(BaseType::Int32), SGE_STR("Int32"));
	ASSERT_EQ(bts.GetSize(BaseType::Int32), sizeof(Int32));
	ASSERT_EQ(bts.GetAlignment(BaseType::Int32), alignof(Int32));
	ASSERT_EQ(bts.GetName(BaseType::UInt32), SGE_STR("UInt32"));
	ASSERT_EQ(bts.GetSize(BaseType::UInt32), sizeof(UInt32));
	ASSERT_EQ(bts.GetAlignment(BaseType::UInt32), alignof(UInt32));
	ASSERT_EQ(bts.GetName(BaseType::Int64), SGE_STR("Int64"));
	ASSERT_EQ(bts.GetSize(BaseType::Int64), sizeof(Int64));
	ASSERT_EQ(bts.GetAlignment(BaseType::Int64), alignof(Int64));
	ASSERT_EQ(bts.GetName(BaseType::UInt64), SGE_STR("UInt64"));
	ASSERT_EQ(bts.GetSize(BaseType::UInt64), sizeof(UInt64));
	ASSERT_EQ(bts.GetAlignment(BaseType::UInt64), alignof(UInt64));
	ASSERT_EQ(bts.GetName(BaseType::Float), SGE_STR("Float"));
	ASSERT_EQ(bts.GetSize(BaseType::Float), sizeof(float));
	ASSERT_EQ(bts.GetAlignment(BaseType::Float), alignof(float));
	ASSERT_EQ(bts.GetName(BaseType::Double), SGE_STR("Double"));
	ASSERT_EQ(bts.GetSize(BaseType::Double), sizeof(double));
	ASSERT_EQ(bts.GetAlignment(BaseType::Double), alignof(double));
	ASSERT_EQ(IntermediateRepresentation::BaseTypeSize, 11);
}

TEST(IntermediateRepresentation_Type, Test)
{
	using namespace IntermediateRepresentation;

	Type t_void = BaseTypes::GetVoidType();
	ASSERT_EQ(t_void.GetContent().GetSize(), 1);
	ASSERT_EQ(t_void.GetContent()[0], BaseType::Void);
	ASSERT_EQ(t_void.GetSize(), 0);
	ASSERT_EQ(t_void.GetAlignment(), 0);

	Type t_int16(BaseType::Int16);
	ASSERT_EQ(t_int16.GetContent().GetSize(), 1);
	ASSERT_EQ(t_int16.GetContent()[0], BaseType::Int16);
	ASSERT_EQ(t_int16.GetSize(), 2);
	ASSERT_EQ(t_int16.GetAlignment(), alignof(Int16));

	Type t_compose({BaseType::Int16, BaseType::Int32});
	ASSERT_EQ(t_compose.GetContent().GetSize(), 2);
	ASSERT_EQ(t_compose.GetContent()[0], BaseType::Int16);
	ASSERT_EQ(t_compose.GetContent()[1], BaseType::Int32);
	ASSERT_EQ(t_compose.GetSize(), 6);
	ASSERT_EQ(t_compose.GetAlignment(), Max(alignof(Int16), alignof(Int32)));

	Type t_compose2(BaseType::Int8);
	Type t_re = t_compose2 + t_compose;

	ASSERT_EQ(t_compose2.GetContent().GetSize(), 1);
	ASSERT_EQ(t_compose2.GetContent()[0], BaseType::Int8);
	ASSERT_EQ(t_compose2.GetSize(), 1);
	ASSERT_EQ(t_compose2.GetAlignment(), alignof(Int8));

	ASSERT_EQ(t_re.GetContent().GetSize(), 3);
	ASSERT_EQ(t_re.GetContent()[0], BaseType::Int8);
	ASSERT_EQ(t_re.GetContent()[1], BaseType::Int16);
	ASSERT_EQ(t_re.GetContent()[2], BaseType::Int32);
	ASSERT_EQ(t_re.GetSize(), 7);
	ASSERT_EQ(t_re.GetAlignment(), Max(t_compose2.GetAlignment(), t_compose.GetAlignment()));

	ASSERT_EQ(t_compose2.GetContent().GetSize(), 1);
	ASSERT_EQ(t_compose2.GetContent()[0], BaseType::Int8);
	ASSERT_EQ(t_compose2.GetSize(), 1);
	ASSERT_EQ(t_compose2.GetAlignment(), alignof(Int8));

	t_compose2 += t_compose;

	ASSERT_EQ(t_compose2.GetContent().GetSize(), 3);
	ASSERT_EQ(t_compose2.GetContent()[0], BaseType::Int8);
	ASSERT_EQ(t_compose2.GetContent()[1], BaseType::Int16);
	ASSERT_EQ(t_compose2.GetContent()[2], BaseType::Int32);
	ASSERT_EQ(t_compose2.GetSize(), 7);
	ASSERT_EQ(t_compose2.GetAlignment(), Max<SizeType>(alignof(Int8), t_compose.GetAlignment()));

	ASSERT_EQ(t_compose2, t_re);
	ASSERT_NE(t_compose2, t_compose);
}

TEST(IntermediateRepresentation_CanConvert, Test)
{
	using namespace IntermediateRepresentation;
	ASSERT_TRUE(CanConvert(BaseTypes::GetUInt64Type(), BaseTypes::GetInt64Type()));
	ASSERT_TRUE(CanConvert(BaseTypes::GetInt64Type(), BaseTypes::GetUInt64Type()));

	ASSERT_FALSE(CanConvert(BaseTypes::GetUInt32Type(), BaseTypes::GetInt64Type()));
	ASSERT_FALSE(CanConvert(BaseTypes::GetInt64Type(), BaseTypes::GetUInt32Type()));
}

TEST(IntermediateRepresentation_Variable, Test)
{
	using IntermediateRepresentation::Variable;
	using IntermediateRepresentation::BaseType;
	using IntermediateRepresentation::Type;
	using IntermediateRepresentation::StorageType;

	Type t(BaseType::UInt64);
	Variable v1(t, StorageType::Global, 1);

	ASSERT_EQ(v1.GetType(), t);
	ASSERT_EQ(v1.GetStorageType(), StorageType::Global);
	ASSERT_EQ(v1.GetIndex(), 1);

	Variable v2(t, StorageType::Global, 1);
	Variable v3(t, StorageType::Local, 0);

	ASSERT_EQ(v1, v2);
	ASSERT_NE(v1, v3);
}

TEST(IntermediateRepresentation_IsTerminatorOperationType, Test)
{
	using namespace IntermediateRepresentation;
	for (UInt8 ot = 0; ot < OperationTypeSetSize; ++ot)
	{
		if ((OperationType)ot == OperationType::Goto || (OperationType)ot == OperationType::If || (OperationType)ot == OperationType::Return)
			ASSERT_TRUE(IsTerminatorOperationType((OperationType)ot));
		else
			ASSERT_FALSE(IsTerminatorOperationType((OperationType)ot));
	}
}

TEST(IntermediateRepresentation_OperationTypeSet, Test)
{
	using namespace IntermediateRepresentation;
	const OperationTypeSet& ots = OperationTypeSet::GetSingleton();

	ASSERT_EQ(ots.Get(OperationType::Set).m_ArgumentStorageTypeMasks, Vector<UInt8>({StorageTypeMasks::Variable, StorageTypeMasks::Constant, StorageTypeMasks::Constant}));
	ASSERT_EQ(ots.Get(OperationType::Set).m_Name, SGE_STR("Set"));
	ASSERT_EQ(ots.Get(OperationType::Set).m_VariableRegistersSize, 1);
	ASSERT_EQ(ots.Get(OperationType::Set).m_AdditionalRegistersSize, 2);
	ASSERT_FALSE(ots.Get(OperationType::Set).m_JudgementFunction({
		Variable(BaseTypes::GetUInt32Type(), StorageType::Local, 0),
		Variable(BaseTypes::GetUInt32Type(), StorageType::Constant, 0),
		Variable(BaseTypes::GetUInt32Type(), StorageType::Constant, 123),
	}));
	ASSERT_TRUE(ots.Get(OperationType::Set).m_JudgementFunction({
		Variable(BaseTypes::GetUInt32Type(), StorageType::Local, 0),
		Variable(BaseTypes::GetUInt32Type(), StorageType::Constant, 1),
		Variable(BaseTypes::GetUInt32Type(), StorageType::Constant, 123),
	}));

	ASSERT_EQ(ots.Get(OperationType::NewLocal).m_ArgumentStorageTypeMasks, Vector<UInt8>({StorageTypeMasks::Local}));
	ASSERT_EQ(ots.Get(OperationType::NewLocal).m_Name, SGE_STR("NewLocal"));
	ASSERT_EQ(ots.Get(OperationType::NewLocal).m_VariableRegistersSize, 1);
	ASSERT_EQ(ots.Get(OperationType::NewLocal).m_AdditionalRegistersSize, 0);

	ASSERT_EQ(ots.Get(OperationType::DeleteLocal).m_ArgumentStorageTypeMasks, Vector<UInt8>({StorageTypeMasks::Local}));
	ASSERT_EQ(ots.Get(OperationType::DeleteLocal).m_Name, SGE_STR("DeleteLocal"));
	ASSERT_EQ(ots.Get(OperationType::DeleteLocal).m_VariableRegistersSize, 1);
	ASSERT_EQ(ots.Get(OperationType::DeleteLocal).m_AdditionalRegistersSize, 0);

	ASSERT_EQ(ots.Get(OperationType::Push).m_ArgumentStorageTypeMasks, Vector<UInt8>({StorageTypeMasks::Value}));
	ASSERT_EQ(ots.Get(OperationType::Push).m_Name, SGE_STR("Push"));
	ASSERT_EQ(ots.Get(OperationType::Push).m_VariableRegistersSize, 1);
	ASSERT_EQ(ots.Get(OperationType::Push).m_AdditionalRegistersSize, 1);

	ASSERT_EQ(ots.Get(OperationType::Pop).m_ArgumentStorageTypeMasks, Vector<UInt8>({StorageTypeMasks::Variable}));
	ASSERT_EQ(ots.Get(OperationType::Pop).m_Name, SGE_STR("Pop"));
	ASSERT_EQ(ots.Get(OperationType::Pop).m_VariableRegistersSize, 1);
	ASSERT_EQ(ots.Get(OperationType::Pop).m_AdditionalRegistersSize, 1);

	ASSERT_EQ(ots.Get(OperationType::Copy).m_ArgumentStorageTypeMasks, Vector<UInt8>({StorageTypeMasks::Variable, StorageTypeMasks::Variable}));
	ASSERT_EQ(ots.Get(OperationType::Copy).m_Name, SGE_STR("Copy"));
	ASSERT_EQ(ots.Get(OperationType::Copy).m_VariableRegistersSize, 2);
	ASSERT_EQ(ots.Get(OperationType::Copy).m_AdditionalRegistersSize, 0);
	ASSERT_FALSE(ots.Get(OperationType::Copy).m_JudgementFunction({
		Variable(BaseTypes::GetUInt32Type(), StorageType::Local, 0),
		Variable(BaseTypes::GetUInt32Type(), StorageType::Local, 1),
	}));
	ASSERT_TRUE(ots.Get(OperationType::Copy).m_JudgementFunction({
		Variable(BaseTypes::GetUInt32Type(), StorageType::Local, 0),
		Variable(BaseTypes::GetUInt32Type(), StorageType::Local, 0),
	}));

	ASSERT_EQ(ots.Get(OperationType::Goto).m_ArgumentStorageTypeMasks, Vector<UInt8>({StorageTypeMasks::Constant}));
	ASSERT_EQ(ots.Get(OperationType::Goto).m_Name, SGE_STR("Goto"));
	ASSERT_EQ(ots.Get(OperationType::Goto).m_VariableRegistersSize, 0);
	ASSERT_EQ(ots.Get(OperationType::Goto).m_AdditionalRegistersSize, 0);

	ASSERT_EQ(ots.Get(OperationType::If).m_ArgumentStorageTypeMasks, Vector<UInt8>({StorageTypeMasks::Variable, StorageTypeMasks::Constant, StorageTypeMasks::Constant}));
	ASSERT_EQ(ots.Get(OperationType::If).m_Name, SGE_STR("If"));
	ASSERT_EQ(ots.Get(OperationType::If).m_VariableRegistersSize, 1);
	ASSERT_EQ(ots.Get(OperationType::If).m_AdditionalRegistersSize, 0);

	ASSERT_EQ(ots.Get(OperationType::Call).m_ArgumentStorageTypeMasks, Vector<UInt8>({StorageTypeMasks::Constant}));
	ASSERT_EQ(ots.Get(OperationType::Call).m_Name, SGE_STR("Call"));
	ASSERT_EQ(ots.Get(OperationType::Call).m_VariableRegistersSize, 0);
	ASSERT_EQ(ots.Get(OperationType::Call).m_AdditionalRegistersSize, 0);

	ASSERT_EQ(ots.Get(OperationType::CallFunctionPointer).m_ArgumentStorageTypeMasks, Vector<UInt8>({StorageTypeMasks::Variable}));
	ASSERT_EQ(ots.Get(OperationType::CallFunctionPointer).m_Name, SGE_STR("CallFunctionPointer"));
	ASSERT_EQ(ots.Get(OperationType::CallFunctionPointer).m_VariableRegistersSize, 1);
	ASSERT_EQ(ots.Get(OperationType::CallFunctionPointer).m_AdditionalRegistersSize, 0);

	ASSERT_EQ(ots.Get(OperationType::Return).m_ArgumentStorageTypeMasks, Vector<UInt8>({StorageTypeMasks::Value}));
	ASSERT_EQ(ots.Get(OperationType::Return).m_Name, SGE_STR("Return"));
	ASSERT_EQ(ots.Get(OperationType::Return).m_VariableRegistersSize, 1);
	ASSERT_EQ(ots.Get(OperationType::Return).m_AdditionalRegistersSize, 0);

	ASSERT_EQ(ots.Get(OperationType::ExternalCallArgument).m_ArgumentStorageTypeMasks, Vector<UInt8>({StorageTypeMasks::Constant, StorageTypeMasks::Value}));
	ASSERT_EQ(ots.Get(OperationType::ExternalCallArgument).m_Name, SGE_STR("ExternalCallArgument"));
	ASSERT_EQ(ots.Get(OperationType::ExternalCallArgument).m_VariableRegistersSize, 1);
	ASSERT_EQ(ots.Get(OperationType::ExternalCallArgument).m_AdditionalRegistersSize, 0);
	ASSERT_FALSE(ots.Get(OperationType::ExternalCallArgument).m_JudgementFunction({
		Variable(BaseTypes::GetUInt32Type(), StorageType::Constant, 0),
		Variable(BaseTypes::GetUInt32Type(), StorageType::Local, 1),
	}));
	ASSERT_TRUE(ots.Get(OperationType::ExternalCallArgument).m_JudgementFunction({
		Variable(BaseTypes::GetUInt32Type(), StorageType::Constant, ArgumentRegistersSize),
		Variable(BaseTypes::GetUInt32Type(), StorageType::Local, 0),
	}));
	ASSERT_TRUE(ots.Get(OperationType::ExternalCallArgument).m_JudgementFunction({
		Variable(BaseTypes::GetUInt32Type(), StorageType::Constant, 0),
		Variable(Type({BaseType::UInt64, BaseType::UInt64}), StorageType::Local, 0),
	}));

	ASSERT_EQ(ots.Get(OperationType::ExternalCall).m_ArgumentStorageTypeMasks, Vector<UInt8>({StorageTypeMasks::Constant, StorageTypeMasks::Constant}));
	ASSERT_EQ(ots.Get(OperationType::ExternalCall).m_Name, SGE_STR("ExternalCall"));
	ASSERT_EQ(ots.Get(OperationType::ExternalCall).m_VariableRegistersSize, 0);
	ASSERT_EQ(ots.Get(OperationType::ExternalCall).m_AdditionalRegistersSize, 0);

	ASSERT_EQ(ots.Get(OperationType::GetReturnValue).m_ArgumentStorageTypeMasks, Vector<UInt8>({StorageTypeMasks::Variable}));
	ASSERT_EQ(ots.Get(OperationType::GetReturnValue).m_Name, SGE_STR("GetReturnValue"));
	ASSERT_EQ(ots.Get(OperationType::GetReturnValue).m_VariableRegistersSize, 1);
	ASSERT_EQ(ots.Get(OperationType::GetReturnValue).m_AdditionalRegistersSize, 0);

	ASSERT_EQ(ots.Get(OperationType::MakeReference).m_ArgumentStorageTypeMasks, Vector<UInt8>({StorageTypeMasks::Reference, StorageTypeMasks::Variable}));
	ASSERT_EQ(ots.Get(OperationType::MakeReference).m_Name, SGE_STR("MakeReference"));
	ASSERT_EQ(ots.Get(OperationType::MakeReference).m_VariableRegistersSize, 2);
	ASSERT_EQ(ots.Get(OperationType::MakeReference).m_AdditionalRegistersSize, 0);
	ASSERT_FALSE(ots.Get(OperationType::MakeReference).m_JudgementFunction({
		Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0),
		Variable(BaseTypes::GetInt64Type(), StorageType::Local, 1),
	}));
	ASSERT_TRUE(ots.Get(OperationType::MakeReference).m_JudgementFunction({
		Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0),
		Variable(BaseTypes::GetInt32Type(), StorageType::Local, 1),
	}));

	ASSERT_EQ(ots.Get(OperationType::GetAddress).m_ArgumentStorageTypeMasks, Vector<UInt8>({StorageTypeMasks::Variable, StorageTypeMasks::Variable}));
	ASSERT_EQ(ots.Get(OperationType::GetAddress).m_Name, SGE_STR("GetAddress"));
	ASSERT_EQ(ots.Get(OperationType::GetAddress).m_VariableRegistersSize, 2);
	ASSERT_EQ(ots.Get(OperationType::GetAddress).m_AdditionalRegistersSize, 0);
	ASSERT_FALSE(ots.Get(OperationType::GetAddress).m_JudgementFunction({
		Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0),
		Variable(BaseTypes::GetUInt32Type(), StorageType::Local, 1),
	}));
	ASSERT_TRUE(ots.Get(OperationType::GetAddress).m_JudgementFunction({
		Variable(BaseTypes::GetUInt32Type(), StorageType::Local, 0),
		Variable(BaseTypes::GetUInt32Type(), StorageType::Local, 1),
	}));

	ASSERT_EQ(ots.Get(OperationType::GetReference).m_ArgumentStorageTypeMasks, Vector<UInt8>({StorageTypeMasks::Reference, StorageTypeMasks::Variable}));
	ASSERT_EQ(ots.Get(OperationType::GetReference).m_Name, SGE_STR("GetReference"));
	ASSERT_EQ(ots.Get(OperationType::GetReference).m_VariableRegistersSize, 2);
	ASSERT_EQ(ots.Get(OperationType::GetReference).m_AdditionalRegistersSize, 0);
	ASSERT_FALSE(ots.Get(OperationType::GetReference).m_JudgementFunction({
		Variable(BaseTypes::GetUInt32Type(), StorageType::Local, 0),
		Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 1),
	}));
	ASSERT_TRUE(ots.Get(OperationType::GetReference).m_JudgementFunction({
		Variable(BaseTypes::GetUInt32Type(), StorageType::Local, 0),
		Variable(BaseTypes::GetUInt32Type(), StorageType::Local, 1),
	}));

	ASSERT_EQ(ots.Get(OperationType::ReleaseReference).m_ArgumentStorageTypeMasks, Vector<UInt8>({StorageTypeMasks::Reference}));
	ASSERT_EQ(ots.Get(OperationType::ReleaseReference).m_Name, SGE_STR("ReleaseReference"));
	ASSERT_EQ(ots.Get(OperationType::ReleaseReference).m_VariableRegistersSize, 1);
	ASSERT_EQ(ots.Get(OperationType::ReleaseReference).m_AdditionalRegistersSize, 0);

	// todo
}

TEST(IntermediateRepresentation_Operation, Test)
{
	using namespace IntermediateRepresentation;

	Operation o1(OperationType::Push, {Variable(BaseTypes::GetInt32Type(), StorageType::Local, 0)});

	ASSERT_EQ(o1.GetType(), OperationType::Push);
	ASSERT_EQ(o1.GetArguments().GetSize(), 1);
	ASSERT_EQ(o1.GetArguments()[0], Variable(BaseTypes::GetInt32Type(), StorageType::Local, 0));

	Operation o2(OperationType::Push, {Variable(BaseTypes::GetInt32Type(), StorageType::Local, 0)});

	ASSERT_EQ(o1, o2);

	Operation o3(OperationType::Push, {Variable(BaseTypes::GetInt32Type(), StorageType::Global, 0)});

	ASSERT_NE(o1, o3);
}

TEST(IntermediateRepresentation_BasicBlock, Test)
{
	using namespace IntermediateRepresentation;
	Operation o1(OperationType::Return, {Variable(BaseTypes::GetVoidType(), StorageType::Constant, 0)});
	Operation o2(OperationType::Return, {Variable(BaseTypes::GetUInt32Type(), StorageType::Constant, 0)});
	Operation o3(OperationType::Goto, {Variable(BaseTypes::GetUInt64Type(), StorageType::Constant, 1)});
	Operation o4(OperationType::If, {Variable(BaseTypes::GetUInt64Type(), StorageType::Global, 1),
									 Variable(BaseTypes::GetUInt64Type(), StorageType::Constant, 2),
									 Variable(BaseTypes::GetUInt64Type(), StorageType::Constant, 3)});

	BasicBlock bb1(0, {o1});

	ASSERT_EQ(bb1.GetIndex(), 0);
	ASSERT_EQ(bb1.GetToIndices().GetSize(), 0);
	ASSERT_EQ(bb1.GetContent().GetSize(), 1);
	ASSERT_EQ(*bb1.GetContent().GetConstBegin(), o1);

	BasicBlock bb2(0, {o1});

	ASSERT_EQ(bb2.GetIndex(), 0);
	ASSERT_EQ(bb2.GetToIndices().GetSize(), 0);
	ASSERT_EQ(bb2.GetContent().GetSize(), 1);
	ASSERT_EQ(*bb2.GetContent().GetConstBegin(), o1);

	ASSERT_EQ(bb1, bb2);

	BasicBlock bb3(0, {o2});

	ASSERT_EQ(bb3.GetIndex(), 0);
	ASSERT_EQ(bb3.GetToIndices().GetSize(), 0);
	ASSERT_EQ(bb3.GetContent().GetSize(), 1);
	ASSERT_EQ(*bb3.GetContent().GetConstBegin(), o2);

	ASSERT_NE(bb1, bb3);
	ASSERT_NE(bb2, bb3);

	BasicBlock bb4(1, {o3});

	ASSERT_EQ(bb4.GetIndex(), 1);
	ASSERT_EQ(bb4.GetToIndices().GetSize(), 1);
	ASSERT_EQ(bb4.GetToIndices()[0], 1);
	ASSERT_EQ(bb4.GetContent().GetSize(), 1);
	ASSERT_EQ(*bb4.GetContent().GetConstBegin(), o3);

	BasicBlock bb5(2, {o4});

	ASSERT_EQ(bb5.GetIndex(), 2);
	ASSERT_EQ(bb5.GetToIndices().GetSize(), 2);
	ASSERT_EQ(bb5.GetToIndices()[0], 2);
	ASSERT_EQ(bb5.GetToIndices()[1], 3);
	ASSERT_EQ(bb5.GetContent().GetSize(), 1);
	ASSERT_EQ(*bb5.GetContent().GetConstBegin(), o4);
}

TEST(IntermediateRepresentation_Function, Test)
{
	using namespace IntermediateRepresentation;

	Vector<Operation> operations({Operation(OperationType::Push, {Variable(BaseTypes::GetInt32Type(), StorageType::Local, 0)})});
	IntermediateRepresentation::Function f1({&BaseTypes::GetInt32Type(), &BaseTypes::GetUInt64Type()}, BaseTypes::GetUInt32Type(), 1, operations);

	ASSERT_EQ(f1.GetParameterTypes(), Vector<const Type*>({&BaseTypes::GetInt32Type(), &BaseTypes::GetUInt64Type()}));
	ASSERT_EQ(f1.GetResultType(), BaseTypes::GetUInt32Type());
	ASSERT_EQ(f1.GetIndex(), 1);
	ASSERT_EQ(f1.GetOperations().GetSize(), 1);
	ASSERT_EQ(f1.GetOperations()[0], Operation(OperationType::Push, {Variable(BaseTypes::GetInt32Type(), StorageType::Local, 0)}));
	ASSERT_FALSE(f1.IsExternal());

	Variable v1 = f1.ToVariable();
	ASSERT_EQ(v1.GetType(), BaseTypes::GetUInt64Type());
	ASSERT_EQ(v1.GetStorageType(), StorageType::Function);
	ASSERT_EQ(v1.GetIndex(), 1);

	Variable v2 = (Variable)f1;
	ASSERT_EQ(v2.GetType(), BaseTypes::GetUInt64Type());
	ASSERT_EQ(v2.GetStorageType(), StorageType::Function);
	ASSERT_EQ(v2.GetIndex(), 1);

	ASSERT_EQ(v1, v2);

	const IntermediateRepresentation::Function f2({&BaseTypes::GetInt32Type(), &BaseTypes::GetUInt64Type()}, BaseTypes::GetUInt32Type(), 1, operations);
	const IntermediateRepresentation::Function f3({&BaseTypes::GetInt32Type(), &BaseTypes::GetUInt32Type()}, BaseTypes::GetUInt32Type(), 1, operations);

	ASSERT_EQ(f1, f2);
	ASSERT_FALSE(f2.IsExternal());
	ASSERT_NE(f1, f3);
	ASSERT_FALSE(f3.IsExternal());

	const IntermediateRepresentation::Function f4({&BaseTypes::GetInt32Type(), &BaseTypes::GetUInt64Type()}, BaseTypes::GetUInt32Type(), 1);

	ASSERT_NE(f1, f4);
	ASSERT_TRUE(f4.IsExternal());
}

TEST(IntermediateRepresentation_RegisterAllocationRequests, Test)
{
	using namespace IntermediateRepresentation;
	RegisterAllocationRequests reqs;
	reqs.AddRegisterAllocationRequest(0, 100, 123);
	reqs.AddRegisterAllocationRequest(0, 300, 456);
	reqs.AddRegisterAllocationRequest(0, 500, 123);
	reqs.AddFunctionCall(0, 200, FunctionCallType::Internal, 1);
	reqs.AddFunctionCall(0, 400, FunctionCallType::External, 2);
	reqs.AddFunctionCall(0, 600, FunctionCallType::FunctionPointer, 0);

	const auto& reqs_ref = reqs.GetRequests();
	ASSERT_EQ(reqs_ref.GetSize(), 1);
	ASSERT_EQ(reqs_ref.Find(0)->m_Second.Find(123)->m_Second.GetSize(), 2);
	ASSERT_EQ(reqs_ref.Find(0)->m_Second.Find(123)->m_Second.GetConstBegin()->m_First, 100);
	ASSERT_EQ((reqs_ref.Find(0)->m_Second.Find(123)->m_Second.GetConstBegin() + 1)->m_First, 500);
	ASSERT_EQ(reqs_ref.Find(0)->m_Second.Find(456)->m_Second.GetSize(), 1);
	ASSERT_EQ(reqs_ref.Find(0)->m_Second.Find(456)->m_Second.GetConstBegin()->m_First, 300);
	ASSERT_EQ(reqs_ref.Find(1), reqs_ref.GetConstEnd());

	const auto& func_call_record_ref = reqs.GetFunctionCallRecords();
	ASSERT_EQ(func_call_record_ref.GetSize(), 1);
	ASSERT_EQ(func_call_record_ref.Find(0)->m_Second.GetSize(), 3);
	ASSERT_EQ(func_call_record_ref.Find(0)->m_Second.GetConstBegin()->m_First, 200);
	ASSERT_EQ(func_call_record_ref.Find(0)->m_Second.GetConstBegin()->m_Second.m_First, FunctionCallType::Internal);
	ASSERT_EQ(func_call_record_ref.Find(0)->m_Second.GetConstBegin()->m_Second.m_Second, 1);
	ASSERT_EQ((func_call_record_ref.Find(0)->m_Second.GetConstBegin() + 1)->m_First, 400);
	ASSERT_EQ((func_call_record_ref.Find(0)->m_Second.GetConstBegin() + 1)->m_Second.m_First, FunctionCallType::External);
	ASSERT_EQ((func_call_record_ref.Find(0)->m_Second.GetConstBegin() + 1)->m_Second.m_Second, 2);
	ASSERT_EQ((func_call_record_ref.Find(0)->m_Second.GetConstBegin() + 2)->m_First, 600);
	ASSERT_EQ((func_call_record_ref.Find(0)->m_Second.GetConstBegin() + 2)->m_Second.m_First, FunctionCallType::FunctionPointer);
	ASSERT_EQ((func_call_record_ref.Find(0)->m_Second.GetConstBegin() + 2)->m_Second.m_Second, 0);
	ASSERT_EQ(func_call_record_ref.Find(1), func_call_record_ref.GetConstEnd());

	const auto& internal_func_relation_ref = reqs.GetInternalFunctionCallRelationships();
	ASSERT_EQ(internal_func_relation_ref.GetSize(), 1);
	ASSERT_EQ(internal_func_relation_ref.Find(0)->m_Second.GetSize(), 1);
	ASSERT_EQ(internal_func_relation_ref.Find(0)->m_Second.Find(1)->m_First, 1);
	ASSERT_EQ(internal_func_relation_ref.Find(0)->m_Second.Find(1)->m_Second, true);
	ASSERT_EQ(internal_func_relation_ref.Find(1), internal_func_relation_ref.GetConstEnd());
}

TEST(IntermediateRepresentation_RegisterAllocationResults, Test)
{
	using namespace IntermediateRepresentation;
	RegisterAllocationResults results;
	results.AddRegisterAllocationResult(123, 100, RegisterAllocationResult(RegisterAllocationState::Inactive, 1));
	results.AddRegisterAllocationResult(456, 300, RegisterAllocationResult(RegisterAllocationState::Active, 2));
	results.AddRegisterAllocationResult(123, 500, RegisterAllocationResult(RegisterAllocationState::Active, 1));
	const auto& results_ref = results.GetResults();
	ASSERT_EQ(results_ref.Find(123)->m_Second.GetSize(), 2);
	ASSERT_EQ(results_ref.Find(123)->m_Second.GetConstBegin()->m_First, 100);
	ASSERT_EQ(results_ref.Find(123)->m_Second.GetConstBegin()->m_Second.m_State, RegisterAllocationState::Inactive);
	ASSERT_EQ(results_ref.Find(123)->m_Second.GetConstBegin()->m_Second.m_CommonRegisterIndex, 1);
	ASSERT_EQ((results_ref.Find(123)->m_Second.GetConstBegin() + 1)->m_First, 500);
	ASSERT_EQ((results_ref.Find(123)->m_Second.GetConstBegin() + 1)->m_Second.m_State, RegisterAllocationState::Active);
	ASSERT_EQ((results_ref.Find(123)->m_Second.GetConstBegin() + 1)->m_Second.m_CommonRegisterIndex, 1);
	ASSERT_EQ(results_ref.Find(456)->m_Second.GetSize(), 1);
	ASSERT_EQ(results_ref.Find(456)->m_Second.GetConstBegin()->m_First, 300);
	ASSERT_EQ(results_ref.Find(456)->m_Second.GetConstBegin()->m_Second.m_State, RegisterAllocationState::Active);
	ASSERT_EQ(results_ref.Find(456)->m_Second.GetConstBegin()->m_Second.m_CommonRegisterIndex, 2);
}

TEST(IntermediateRepresentation_TranslateUnit, Test)
{
	using namespace IntermediateRepresentation;
	TranslateUnit tu;
	const Type& t1 = tu.NewType({BaseType::Float, BaseType::Int32});
	const Variable& v1 = tu.NewGlobalVariable(BaseTypes::GetUInt64Type(), 0);
	const IntermediateRepresentation::Function& f1 = tu.NewInternalFunction({&t1, &BaseTypes::GetUInt64Type()}, t1, 0, {Operation(OperationType::Push, {v1})});
	const IntermediateRepresentation::Function& f2 = tu.NewExternalFunction({&t1, &BaseTypes::GetUInt64Type()}, t1, 1);

	ASSERT_EQ(tu.GetGlobalVariable(0), v1);
	ASSERT_EQ(tu.GetFunction(0), f1);
	ASSERT_EQ(tu.GetFunction(1), f2);

	ASSERT_FALSE(f1.IsExternal());
	ASSERT_TRUE(f2.IsExternal());
}

TEST(IntermediateRepresentation_IsValidTranslateUnit, Test)
{
	using namespace IntermediateRepresentation;

	TranslateUnit tu1;
	tu1.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																 Operation(OperationType::Set, {Variable(BaseTypes::GetUInt64Type(), StorageType::Global, 0), Variable(BaseTypes::GetUInt64Type(), StorageType::Constant, 0), Variable(BaseTypes::GetUInt64Type(), StorageType::Constant, 0)}),
															 });
	ASSERT_FALSE(IsValidTranslateUnit(tu1));
	tu1.NewGlobalVariable(BaseTypes::GetUInt64Type(), 0);
	ASSERT_TRUE(IsValidTranslateUnit(tu1));

	TranslateUnit tu2;
	tu2.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																 Operation(OperationType::Push, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)}),
															 });
	ASSERT_FALSE(IsValidTranslateUnit(tu2));

	TranslateUnit tu3;
	tu3.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																 Operation(OperationType::NewLocal, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)}),
																 Operation(OperationType::NewLocal, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)}),
															 });
	ASSERT_FALSE(IsValidTranslateUnit(tu3));

	TranslateUnit tu4;
	tu4.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																 Operation(OperationType::DeleteLocal, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)}),
															 });
	ASSERT_FALSE(IsValidTranslateUnit(tu4));

	TranslateUnit tu5;
	tu5.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																 Operation(OperationType::NewLocal, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)}),
															 });
	ASSERT_FALSE(IsValidTranslateUnit(tu5));

	TranslateUnit tu6;
	tu6.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																 Operation(OperationType::NewLocal, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)}),
																 Operation(OperationType::Push, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)}),
																 Operation(OperationType::DeleteLocal, {Variable(BaseTypes::GetInt64Type(), StorageType::Local, 0)}),
															 });
	ASSERT_FALSE(IsValidTranslateUnit(tu6));

	TranslateUnit tu7;
	tu7.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																 Operation(OperationType::NewLocal, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)}),
																 Operation(OperationType::Push, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)}),
																 Operation(OperationType::DeleteLocal, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)}),
															 });
	ASSERT_TRUE(IsValidTranslateUnit(tu7));

	/*TranslateUnit tu8;
	tu8.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																 Operation(OperationType::Label, {Variable(BaseTypes::GetVoidType(), StorageType::Constant, 0)}),
																 Operation(OperationType::Goto, {Variable(BaseTypes::GetVoidType(), StorageType::Constant, 0)}),
																 Operation(OperationType::Goto, {Variable(BaseTypes::GetVoidType(), StorageType::Constant, 1)}),
																 Operation(OperationType::Label, {Variable(BaseTypes::GetVoidType(), StorageType::Constant, 1)}),
															 });
	ASSERT_TRUE(IsValidTranslateUnit(tu8));

	TranslateUnit tu9;
	tu9.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																 Operation(OperationType::Label, {Variable(BaseTypes::GetVoidType(), StorageType::Constant, 0)}),
																 Operation(OperationType::Goto, {Variable(BaseTypes::GetVoidType(), StorageType::Constant, 1)}),
															 });
	ASSERT_FALSE(IsValidTranslateUnit(tu9));

	TranslateUnit tu10;
	tu10.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::Label, {Variable(BaseTypes::GetVoidType(), StorageType::Constant, 0)}),
																  Operation(OperationType::Label, {Variable(BaseTypes::GetVoidType(), StorageType::Constant, 0)}),
																  Operation(OperationType::Goto, {Variable(BaseTypes::GetVoidType(), StorageType::Constant, 0)}),
															  });
	ASSERT_FALSE(IsValidTranslateUnit(tu10));

	TranslateUnit tu11;
	tu11.NewGlobalVariable(BaseTypes::GetUInt8Type(), 0);
	tu11.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::Label, {Variable(BaseTypes::GetVoidType(), StorageType::Constant, 0)}),
																  Operation(OperationType::If, {Variable(BaseTypes::GetUInt8Type(), StorageType::Global, 0), Variable(BaseTypes::GetVoidType(), StorageType::Constant, 0)}),
																  Operation(OperationType::If, {Variable(BaseTypes::GetUInt8Type(), StorageType::Global, 0), Variable(BaseTypes::GetVoidType(), StorageType::Constant, 1)}),
																  Operation(OperationType::Label, {Variable(BaseTypes::GetVoidType(), StorageType::Constant, 1)}),
															  });
	ASSERT_TRUE(IsValidTranslateUnit(tu11));

	TranslateUnit tu12;
	tu12.NewGlobalVariable(BaseTypes::GetUInt8Type(), 0);
	tu12.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::Label, {Variable(BaseTypes::GetVoidType(), StorageType::Constant, 0)}),
																  Operation(OperationType::If, {Variable(BaseTypes::GetUInt8Type(), StorageType::Global, 0), Variable(BaseTypes::GetVoidType(), StorageType::Constant, 1)}),
															  });
	ASSERT_FALSE(IsValidTranslateUnit(tu12));*/

	TranslateUnit tu13;
	tu13.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::Call, {Variable(BaseTypes::GetVoidType(), StorageType::Constant, 1)}),
															  });
	tu13.NewInternalFunction({}, BaseTypes::GetVoidType(), 1, {});
	ASSERT_TRUE(IsValidTranslateUnit(tu13));

	TranslateUnit tu14;
	tu14.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::Call, {Variable(BaseTypes::GetVoidType(), StorageType::Constant, 1)}),
															  });
	ASSERT_FALSE(IsValidTranslateUnit(tu14));

	TranslateUnit tu15;
	tu15.NewGlobalVariable(BaseTypes::GetUInt8Type(), 0);
	tu15.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::ExternalCallArgument, {
																													 Variable(BaseTypes::GetVoidType(), StorageType::Constant, 0),
																													 Variable(BaseTypes::GetUInt8Type(), StorageType::Global, 0),
																												 }),
															  });
	ASSERT_TRUE(IsValidTranslateUnit(tu15));

	TranslateUnit tu17;
	tu17.NewGlobalVariable(BaseTypes::GetUInt8Type(), 0);
	tu17.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::MakeReference, {
																											  Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0),
																											  Variable(BaseTypes::GetUInt8Type(), StorageType::Global, 0),
																										  }),
																  Operation(OperationType::Push, {Variable(BaseTypes::GetUInt8Type(), StorageType::Reference, 0)}),
																  Operation(OperationType::ReleaseReference, {Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0)}),
															  });
	ASSERT_TRUE(IsValidTranslateUnit(tu17));

	TranslateUnit tu19;
	tu19.NewGlobalVariable(BaseTypes::GetUInt8Type(), 0);
	tu19.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::MakeReference, {
																											  Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0),
																											  Variable(BaseTypes::GetUInt8Type(), StorageType::Global, 0),
																										  }),
																  Operation(OperationType::MakeReference, {
																											  Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0),
																											  Variable(BaseTypes::GetUInt8Type(), StorageType::Global, 0),
																										  }),
																  Operation(OperationType::Push, {Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0)}),
																  Operation(OperationType::ReleaseReference, {Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0)}),
															  });
	ASSERT_FALSE(IsValidTranslateUnit(tu19));

	TranslateUnit tu20;
	tu20.NewGlobalVariable(BaseTypes::GetUInt8Type(), 0);
	tu20.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::MakeReference, {
																											  Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0),
																											  Variable(BaseTypes::GetUInt8Type(), StorageType::Reference, 1),
																										  }),
																  Operation(OperationType::Push, {Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0)}),
																  Operation(OperationType::ReleaseReference, {Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0)}),
															  });
	ASSERT_FALSE(IsValidTranslateUnit(tu20));

	TranslateUnit tu21;
	tu21.NewGlobalVariable(BaseTypes::GetUInt8Type(), 0);
	tu21.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::Push, {Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0)}),
																  Operation(OperationType::ReleaseReference, {Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0)}),
															  });
	ASSERT_FALSE(IsValidTranslateUnit(tu21));

	TranslateUnit tu22;
	tu22.NewGlobalVariable(BaseTypes::GetUInt8Type(), 0);
	tu22.NewGlobalVariable(BaseTypes::GetUInt64Type(), 1);
	tu22.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::GetAddress, {
																										   Variable(BaseTypes::GetInt64Type(), StorageType::Global, 1),
																										   Variable(BaseTypes::GetInt8Type(), StorageType::Global, 0),
																									   }),
															  });
	ASSERT_TRUE(IsValidTranslateUnit(tu22));

	TranslateUnit tu24;
	tu24.NewGlobalVariable(BaseTypes::GetUInt8Type(), 0);
	tu24.NewGlobalVariable(BaseTypes::GetUInt64Type(), 1);
	tu24.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::GetAddress, {
																										   Variable(BaseTypes::GetUInt64Type(), StorageType::Global, 1),
																										   Variable(BaseTypes::GetInt8Type(), StorageType::Global, 0),
																									   }),
																  Operation(OperationType::GetReference, {
																											 Variable(BaseTypes::GetUInt8Type(), StorageType::Reference, 0),
																											 Variable(BaseTypes::GetUInt64Type(), StorageType::Global, 1),
																										 }),
																  Operation(OperationType::ReleaseReference, {Variable(BaseTypes::GetUInt8Type(), StorageType::Reference, 0)}),
															  });
	ASSERT_TRUE(IsValidTranslateUnit(tu24));

	TranslateUnit tu25;
	tu25.NewGlobalVariable(BaseTypes::GetUInt8Type(), 0);
	tu25.NewGlobalVariable(BaseTypes::GetUInt64Type(), 1);
	tu25.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::GetAddress, {
																										   Variable(BaseTypes::GetUInt64Type(), StorageType::Global, 1),
																										   Variable(BaseTypes::GetInt8Type(), StorageType::Global, 0),
																									   }),
																  Operation(OperationType::GetReference, {
																											 Variable(BaseTypes::GetUInt8Type(), StorageType::Reference, 0),
																											 Variable(BaseTypes::GetUInt64Type(), StorageType::Global, 1),
																										 }),
																  Operation(OperationType::GetReference, {
																											 Variable(BaseTypes::GetUInt8Type(), StorageType::Reference, 0),
																											 Variable(BaseTypes::GetUInt64Type(), StorageType::Global, 1),
																										 }),
																  Operation(OperationType::ReleaseReference, {Variable(BaseTypes::GetUInt8Type(), StorageType::Reference, 0)}),
															  });
	ASSERT_FALSE(IsValidTranslateUnit(tu25));

	TranslateUnit tu26;
	tu26.NewGlobalVariable(BaseTypes::GetUInt8Type(), 0);
	tu26.NewGlobalVariable(BaseTypes::GetUInt64Type(), 1);
	tu26.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::GetAddress, {
																										   Variable(BaseTypes::GetUInt64Type(), StorageType::Global, 1),
																										   Variable(BaseTypes::GetInt8Type(), StorageType::Global, 0),
																									   }),
																  Operation(OperationType::GetReference, {
																											 Variable(BaseTypes::GetUInt8Type(), StorageType::Reference, 0),
																											 Variable(BaseTypes::GetUInt64Type(), StorageType::Reference, 1),
																										 }),
																  Operation(OperationType::ReleaseReference, {Variable(BaseTypes::GetUInt8Type(), StorageType::Reference, 0)}),
																  Operation(OperationType::ReleaseReference, {Variable(BaseTypes::GetUInt64Type(), StorageType::Reference, 1)}),
															  });
	ASSERT_FALSE(IsValidTranslateUnit(tu26));

	TranslateUnit tu27;
	tu27.NewGlobalVariable(BaseTypes::GetUInt8Type(), 0);
	tu27.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::MakeReference, {
																											  Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0),
																											  Variable(BaseTypes::GetUInt8Type(), StorageType::Global, 0),
																										  }),
																  Operation(OperationType::Push, {Variable(BaseTypes::GetUInt8Type(), StorageType::Reference, 0)}),
															  });
	ASSERT_FALSE(IsValidTranslateUnit(tu27));

	TranslateUnit tu28;
	tu28.NewGlobalVariable(BaseTypes::GetUInt8Type(), 0);
	tu28.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::MakeReference, {
																											  Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0),
																											  Variable(BaseTypes::GetUInt8Type(), StorageType::Global, 0),
																										  }),
																  Operation(OperationType::Push, {Variable(BaseTypes::GetUInt8Type(), StorageType::Reference, 0)}),
																  Operation(OperationType::ReleaseReference, {Variable(BaseTypes::GetUInt8Type(), StorageType::Reference, 0)}),
															  });
	ASSERT_FALSE(IsValidTranslateUnit(tu28));

	TranslateUnit tu29;
	tu29.NewGlobalVariable(BaseTypes::GetUInt8Type(), 0);
	tu29.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::MakeReference, {
																											  Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0),
																											  Variable(BaseTypes::GetUInt8Type(), StorageType::Global, 0),
																										  }),
																  Operation(OperationType::Push, {Variable(BaseTypes::GetUInt8Type(), StorageType::Reference, 0)}),
																  Operation(OperationType::ReleaseReference, {Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0)}),
																  Operation(OperationType::ReleaseReference, {Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0)}),
															  });
	ASSERT_FALSE(IsValidTranslateUnit(tu29));

	TranslateUnit tu30;
	tu30.NewGlobalVariable(BaseTypes::GetUInt8Type(), 0);
	tu30.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::MakeReference, {
																											  Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0),
																											  Variable(BaseTypes::GetUInt8Type(), StorageType::Global, 0),
																										  }),
																  Operation(OperationType::ReleaseReference, {Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0)}),
																  Operation(OperationType::MakeReference, {
																											  Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0),
																											  Variable(BaseTypes::GetUInt8Type(), StorageType::Global, 0),
																										  }),
																  Operation(OperationType::Push, {Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0)}),
																  Operation(OperationType::ReleaseReference, {Variable(BaseTypes::GetInt8Type(), StorageType::Reference, 0)}),
															  });
	ASSERT_TRUE(IsValidTranslateUnit(tu30));

	TranslateUnit tu31;
	tu31.NewGlobalVariable(BaseTypes::GetUInt8Type(), 0);
	tu31.NewGlobalVariable(BaseTypes::GetUInt64Type(), 1);
	tu31.NewInternalFunction({}, BaseTypes::GetVoidType(), 0, {
																  Operation(OperationType::GetAddress, {
																										   Variable(BaseTypes::GetUInt64Type(), StorageType::Global, 1),
																										   Variable(BaseTypes::GetInt8Type(), StorageType::Global, 0),
																									   }),
																  Operation(OperationType::GetReference, {
																											 Variable(BaseTypes::GetUInt8Type(), StorageType::Reference, 0),
																											 Variable(BaseTypes::GetUInt64Type(), StorageType::Global, 1),
																										 }),
																  Operation(OperationType::ReleaseReference, {Variable(BaseTypes::GetUInt8Type(), StorageType::Reference, 0)}),
																  Operation(OperationType::GetReference, {
																											 Variable(BaseTypes::GetUInt8Type(), StorageType::Reference, 0),
																											 Variable(BaseTypes::GetUInt64Type(), StorageType::Global, 1),
																										 }),
																  Operation(OperationType::ReleaseReference, {Variable(BaseTypes::GetUInt8Type(), StorageType::Reference, 0)}),
															  });
	ASSERT_TRUE(IsValidTranslateUnit(tu31));
}