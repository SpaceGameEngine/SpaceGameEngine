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
	ASSERT_EQ(bts.GetName(BaseType::Int8), SGE_STR("Int8"));
	ASSERT_EQ(bts.GetSize(BaseType::Int8), sizeof(Int8));
	ASSERT_EQ(bts.GetName(BaseType::UInt8), SGE_STR("UInt8"));
	ASSERT_EQ(bts.GetSize(BaseType::UInt8), sizeof(UInt8));
	ASSERT_EQ(bts.GetName(BaseType::Int16), SGE_STR("Int16"));
	ASSERT_EQ(bts.GetSize(BaseType::Int16), sizeof(Int16));
	ASSERT_EQ(bts.GetName(BaseType::UInt16), SGE_STR("UInt16"));
	ASSERT_EQ(bts.GetSize(BaseType::UInt16), sizeof(UInt16));
	ASSERT_EQ(bts.GetName(BaseType::Int32), SGE_STR("Int32"));
	ASSERT_EQ(bts.GetSize(BaseType::Int32), sizeof(Int32));
	ASSERT_EQ(bts.GetName(BaseType::UInt32), SGE_STR("UInt32"));
	ASSERT_EQ(bts.GetSize(BaseType::UInt32), sizeof(UInt32));
	ASSERT_EQ(bts.GetName(BaseType::Int64), SGE_STR("Int64"));
	ASSERT_EQ(bts.GetSize(BaseType::Int64), sizeof(Int64));
	ASSERT_EQ(bts.GetName(BaseType::UInt64), SGE_STR("UInt64"));
	ASSERT_EQ(bts.GetSize(BaseType::UInt64), sizeof(UInt64));
	ASSERT_EQ(bts.GetName(BaseType::Float), SGE_STR("Float"));
	ASSERT_EQ(bts.GetSize(BaseType::Float), sizeof(float));
	ASSERT_EQ(bts.GetName(BaseType::Double), SGE_STR("Double"));
	ASSERT_EQ(bts.GetSize(BaseType::Double), sizeof(double));
	ASSERT_EQ(IntermediateRepresentation::BaseTypeSize, 11);
}

TEST(IntermediateRepresentation_Type, Test)
{
	using namespace IntermediateRepresentation;

	Type t_void = BaseTypes::GetVoidType();
	ASSERT_EQ(t_void.GetContent().GetSize(), 1);
	ASSERT_EQ(t_void.GetContent()[0], BaseType::Void);
	ASSERT_EQ(t_void.GetSize(), 0);

	Type t_int16(BaseType::Int16);
	ASSERT_EQ(t_int16.GetContent().GetSize(), 1);
	ASSERT_EQ(t_int16.GetContent()[0], BaseType::Int16);
	ASSERT_EQ(t_int16.GetSize(), 2);

	Type t_compose({BaseType::Int16, BaseType::Int32});
	ASSERT_EQ(t_compose.GetContent().GetSize(), 2);
	ASSERT_EQ(t_compose.GetContent()[0], BaseType::Int16);
	ASSERT_EQ(t_compose.GetContent()[1], BaseType::Int32);
	ASSERT_EQ(t_compose.GetSize(), 6);

	Type t_compose2(BaseType::Int8);
	Type t_re = t_compose2 + t_compose;

	ASSERT_EQ(t_compose2.GetContent().GetSize(), 1);
	ASSERT_EQ(t_compose2.GetContent()[0], BaseType::Int8);
	ASSERT_EQ(t_compose2.GetSize(), 1);

	ASSERT_EQ(t_re.GetContent().GetSize(), 3);
	ASSERT_EQ(t_re.GetContent()[0], BaseType::Int8);
	ASSERT_EQ(t_re.GetContent()[1], BaseType::Int16);
	ASSERT_EQ(t_re.GetContent()[2], BaseType::Int32);
	ASSERT_EQ(t_re.GetSize(), 7);

	ASSERT_EQ(t_compose2.GetContent().GetSize(), 1);
	ASSERT_EQ(t_compose2.GetContent()[0], BaseType::Int8);
	ASSERT_EQ(t_compose2.GetSize(), 1);

	t_compose2 += t_compose;

	ASSERT_EQ(t_compose2.GetContent().GetSize(), 3);
	ASSERT_EQ(t_compose2.GetContent()[0], BaseType::Int8);
	ASSERT_EQ(t_compose2.GetContent()[1], BaseType::Int16);
	ASSERT_EQ(t_compose2.GetContent()[2], BaseType::Int32);
	ASSERT_EQ(t_compose2.GetSize(), 7);

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
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArgumentsSize(OperationType::NewLocal), 1);
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArgumentsSize(OperationType::DeleteLocal), 1);
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArgumentsSize(OperationType::Push), 1);
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArgumentsSize(OperationType::Pop), 1);
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetArgumentsSize(OperationType::Copy), 2);
	//todo

	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::NewLocal), SGE_STR("NewLocal"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::DeleteLocal), SGE_STR("DeleteLocal"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::Push), SGE_STR("Push"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::Pop), SGE_STR("Pop"));
	ASSERT_EQ(OperationTypeSet::GetSingleton().GetName(OperationType::Copy), SGE_STR("Copy"));
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