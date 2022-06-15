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
#include "gtest/gtest.h"
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
	ASSERT_TRUE(CanConvert(t_compose, BaseTypes::GetUInt32Type()));
	ASSERT_FALSE(CanConvert(BaseTypes::GetUInt32Type(), t_compose));

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

TEST(IntermediateRepresentation_OperationTypeSet, Test)
{
	using namespace IntermediateRepresentation;
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArguments(OperationType::Set), Vector<UInt8>({StorageTypeMasks::Variable, StorageTypeMasks::Constant, StorageTypeMasks::Constant}));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArguments(OperationType::NewLocal), Vector<UInt8>({StorageTypeMasks::Local}));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArguments(OperationType::DeleteLocal), Vector<UInt8>({StorageTypeMasks::Local}));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArguments(OperationType::Push), Vector<UInt8>({StorageTypeMasks::Variable}));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArguments(OperationType::Pop), Vector<UInt8>({StorageTypeMasks::Variable}));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArguments(OperationType::Copy), Vector<UInt8>({StorageTypeMasks::Variable, StorageTypeMasks::Variable}));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArguments(OperationType::Label), Vector<UInt8>({StorageTypeMasks::Constant}));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArguments(OperationType::Goto), Vector<UInt8>({StorageTypeMasks::Constant}));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArguments(OperationType::If), Vector<UInt8>({StorageTypeMasks::Variable, StorageTypeMasks::Constant}));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArguments(OperationType::Call), Vector<UInt8>({StorageTypeMasks::Constant}));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArguments(OperationType::Return), Vector<UInt8>({StorageTypeMasks::Variable}));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArguments(OperationType::ExternalCallArgument), Vector<UInt8>({StorageTypeMasks::Constant, StorageTypeMasks::Variable}));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArguments(OperationType::ExternalCall), Vector<UInt8>({StorageTypeMasks::Constant, StorageTypeMasks::Constant}));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArguments(OperationType::GetReturnValue), Vector<UInt8>({StorageTypeMasks::Variable}));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArguments(OperationType::MakeReference), Vector<UInt8>({StorageTypeMasks::Reference, StorageTypeMasks::Variable}));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArguments(OperationType::GetAddress), Vector<UInt8>({StorageTypeMasks::Variable, StorageTypeMasks::Variable}));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArguments(OperationType::GetReference), Vector<UInt8>({StorageTypeMasks::Reference, StorageTypeMasks::Variable}));
	//todo

	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::Set), SGE_STR("Set"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::NewLocal), SGE_STR("NewLocal"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::DeleteLocal), SGE_STR("DeleteLocal"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::Push), SGE_STR("Push"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::Pop), SGE_STR("Pop"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::Copy), SGE_STR("Copy"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::Label), SGE_STR("Label"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::Goto), SGE_STR("Goto"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::If), SGE_STR("If"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::Call), SGE_STR("Call"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::Return), SGE_STR("Return"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::ExternalCallArgument), SGE_STR("ExternalCallArgument"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::ExternalCall), SGE_STR("ExternalCall"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::GetReturnValue), SGE_STR("GetReturnValue"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::MakeReference), SGE_STR("MakeReference"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::GetAddress), SGE_STR("GetAddress"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::GetReference), SGE_STR("GetReference"));
	//todo
}

TEST(IntermediateRepresentation_OperationSet, Test)
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
	ASSERT_NE(f1, f3);
}

TEST(IntermediateRepresentation_RegisterAllocationRequests, Test)
{
	using namespace IntermediateRepresentation;
	RegisterAllocationRequests reqs;
	reqs.AddRegisterAllocationRequest(123, 100);
	reqs.AddRegisterAllocationRequest(456, 300);
	reqs.AddRegisterAllocationRequest(123, 500);
	const auto& reqs_ref = reqs.GetRequests();
	ASSERT_EQ(reqs_ref.Find(123)->m_Second.GetSize(), 2);
	ASSERT_EQ(reqs_ref.Find(123)->m_Second.GetConstBegin()->m_First, 100);
	ASSERT_EQ((reqs_ref.Find(123)->m_Second.GetConstBegin() + 1)->m_First, 500);
	ASSERT_EQ(reqs_ref.Find(456)->m_Second.GetSize(), 1);
	ASSERT_EQ(reqs_ref.Find(456)->m_Second.GetConstBegin()->m_First, 300);
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
	const IntermediateRepresentation::Function& f1 = tu.NewFunction({&t1, &BaseTypes::GetUInt64Type()}, t1, 0, {Operation(OperationType::Push, {v1})});

	ASSERT_EQ(tu.GetGlobalVariable(0), v1);
	ASSERT_EQ(tu.GetFunction(0), f1);
}

TEST(IntermediateRepresentation_IsValidTranslateUnit, Test)
{
	using namespace IntermediateRepresentation;

	TranslateUnit tu1;
	tu1.NewFunction({}, BaseTypes::GetVoidType(), 0, {Operation(OperationType::Set, {Variable(BaseTypes::GetUInt64Type(), StorageType::Global, 0), Variable(BaseTypes::GetUInt64Type(), StorageType::Constant, 0), Variable(BaseTypes::GetUInt64Type(), StorageType::Constant, 0)})});
	ASSERT_FALSE(IsValidTranslateUnit(tu1));
	tu1.NewGlobalVariable(BaseTypes::GetUInt64Type(), 0);
	ASSERT_TRUE(IsValidTranslateUnit(tu1));

	TranslateUnit tu2;
	tu2.NewFunction({}, BaseTypes::GetVoidType(), 0, {Operation(OperationType::Push, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)})});
	ASSERT_FALSE(IsValidTranslateUnit(tu2));

	TranslateUnit tu3;
	tu3.NewFunction({}, BaseTypes::GetVoidType(), 0, {Operation(OperationType::NewLocal, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)}), Operation(OperationType::NewLocal, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)})});
	ASSERT_FALSE(IsValidTranslateUnit(tu3));

	TranslateUnit tu4;
	tu4.NewFunction({}, BaseTypes::GetVoidType(), 0, {Operation(OperationType::DeleteLocal, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)})});
	ASSERT_FALSE(IsValidTranslateUnit(tu4));

	TranslateUnit tu5;
	tu5.NewFunction({}, BaseTypes::GetVoidType(), 0, {Operation(OperationType::NewLocal, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)})});
	ASSERT_FALSE(IsValidTranslateUnit(tu5));

	TranslateUnit tu6;
	tu6.NewFunction({}, BaseTypes::GetVoidType(), 0, {Operation(OperationType::NewLocal, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)}), Operation(OperationType::Push, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)}), Operation(OperationType::DeleteLocal, {Variable(BaseTypes::GetInt64Type(), StorageType::Local, 0)})});
	ASSERT_FALSE(IsValidTranslateUnit(tu6));

	//todo add test for: label goto if call externalcallargument makereference getaddress getreference

	TranslateUnit tu7;
	tu7.NewFunction({}, BaseTypes::GetVoidType(), 0, {Operation(OperationType::NewLocal, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)}), Operation(OperationType::Push, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)}), Operation(OperationType::DeleteLocal, {Variable(BaseTypes::GetUInt64Type(), StorageType::Local, 0)})});
	ASSERT_TRUE(IsValidTranslateUnit(tu7));
}