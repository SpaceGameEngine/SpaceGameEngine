/*
Copyright 2026 creatorlxd

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
#include "Assembler/AbstractSyntaxTree.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonIntermediateRepresentation::Assembler;

TEST(AbstractSyntaxTreeNodes, SymbolNodeTest)
{
	AbstractSyntaxTreeNodes::SymbolNode symbol_node(SGE_STR("test_dialect"), SGE_STR("test_type"), 1, 2);

	ASSERT_EQ(symbol_node.GetDialectName(), SGE_STR("test_dialect"));
	ASSERT_EQ(symbol_node.GetName(), SGE_STR("test_type"));
	ASSERT_EQ(symbol_node.GetLine(), 1);
	ASSERT_EQ(symbol_node.GetColumn(), 2);
	ASSERT_EQ(symbol_node.GetFather(), nullptr);
}

TEST(AbstractSyntaxTreeNodes, VariableNodeTest)
{
	AbstractSyntaxTreeNodes::VariableNode variable_node(SGE_STR("test_variable"), 1, 2);

	ASSERT_EQ(variable_node.GetName(), SGE_STR("test_variable"));
	ASSERT_EQ(variable_node.GetLine(), 1);
	ASSERT_EQ(variable_node.GetColumn(), 2);
	ASSERT_EQ(variable_node.GetFather(), nullptr);
}

TEST(AbstractSyntaxTreeNodes, VariableDefinitionNodeTest)
{
	AbstractSyntaxTreeNodes::VariableDefinitionNode variable_definition_node_without_type(SGE_STR("test_variable"), OptionalTag::EmptyOptional, 1, 2);

	ASSERT_EQ(variable_definition_node_without_type.GetName(), SGE_STR("test_variable"));
	ASSERT_FALSE(variable_definition_node_without_type.GetTypeSymbolNode().HasValue());
	ASSERT_EQ(variable_definition_node_without_type.GetLine(), 1);
	ASSERT_EQ(variable_definition_node_without_type.GetColumn(), 2);
	ASSERT_EQ(variable_definition_node_without_type.GetFather(), nullptr);

	AbstractSyntaxTreeNodes::SymbolNode symbol_node(SGE_STR("test_dialect"), SGE_STR("test_type"), 3, 4);
	AbstractSyntaxTreeNodes::VariableDefinitionNode variable_definition_node_with_type(SGE_STR("test_variable2"), std::move(symbol_node), 5, 6);

	ASSERT_EQ(variable_definition_node_with_type.GetName(), SGE_STR("test_variable2"));
	ASSERT_TRUE(variable_definition_node_with_type.GetTypeSymbolNode().HasValue());
	ASSERT_EQ(variable_definition_node_with_type.GetTypeSymbolNode().Get().GetDialectName(), SGE_STR("test_dialect"));
	ASSERT_EQ(variable_definition_node_with_type.GetTypeSymbolNode().Get().GetName(), SGE_STR("test_type"));
	ASSERT_EQ(variable_definition_node_with_type.GetTypeSymbolNode().Get().GetLine(), 3);
	ASSERT_EQ(variable_definition_node_with_type.GetTypeSymbolNode().Get().GetColumn(), 4);
	ASSERT_EQ(variable_definition_node_with_type.GetTypeSymbolNode().Get().GetFather(), &variable_definition_node_with_type);
	ASSERT_EQ(variable_definition_node_with_type.GetLine(), 5);
	ASSERT_EQ(variable_definition_node_with_type.GetColumn(), 6);
	ASSERT_EQ(variable_definition_node_with_type.GetFather(), nullptr);
}

TEST(AbstractSyntaxTreeNodes, ParameterDefinitionNodeTest)
{
	AbstractSyntaxTreeNodes::SymbolNode symbol_node(SGE_STR("test_dialect"), SGE_STR("test_type"), 1, 2);
	AbstractSyntaxTreeNodes::ParameterDefinitionNode parameter_definition_node(SGE_STR("test_parameter"), std::move(symbol_node), 3, 4);

	ASSERT_EQ(parameter_definition_node.GetName(), SGE_STR("test_parameter"));
	ASSERT_EQ(parameter_definition_node.GetTypeSymbolNode().GetDialectName(), SGE_STR("test_dialect"));
	ASSERT_EQ(parameter_definition_node.GetTypeSymbolNode().GetName(), SGE_STR("test_type"));
	ASSERT_EQ(parameter_definition_node.GetTypeSymbolNode().GetLine(), 1);
	ASSERT_EQ(parameter_definition_node.GetTypeSymbolNode().GetColumn(), 2);
	ASSERT_EQ(parameter_definition_node.GetTypeSymbolNode().GetFather(), &parameter_definition_node);
	ASSERT_EQ(parameter_definition_node.GetLine(), 3);
	ASSERT_EQ(parameter_definition_node.GetColumn(), 4);
	ASSERT_EQ(parameter_definition_node.GetFather(), nullptr);
}

TEST(AbstractSyntaxTreeNodes, ParameterDefinitionsNodeTest)
{
	AbstractSyntaxTreeNodes::SymbolNode symbol_node(SGE_STR("test_dialect"), SGE_STR("test_type"), 1, 2);
	AbstractSyntaxTreeNodes::ParameterDefinitionNode parameter_definition_node(SGE_STR("test_parameter"), std::move(symbol_node), 3, 4);
	AbstractSyntaxTreeNodes::SymbolNode symbol_node2(SGE_STR("test_dialect"), SGE_STR("test_type2"), 5, 6);
	AbstractSyntaxTreeNodes::ParameterDefinitionNode parameter_definition_node2(SGE_STR("test_parameter2"), std::move(symbol_node2), 7, 8);
	AbstractSyntaxTreeNodes::ParameterDefinitionsNode parameter_definitions_node(Vector<AbstractSyntaxTreeNodes::ParameterDefinitionNode>{std::move(parameter_definition_node), std::move(parameter_definition_node2)}, 9, 10);

	const auto& parameter_definitions = parameter_definitions_node.GetParameterDefinitions();
	ASSERT_EQ(parameter_definitions.GetSize(), 2);
	ASSERT_EQ(parameter_definitions[0].GetName(), SGE_STR("test_parameter"));
	ASSERT_EQ(parameter_definitions[0].GetTypeSymbolNode().GetDialectName(), SGE_STR("test_dialect"));
	ASSERT_EQ(parameter_definitions[0].GetTypeSymbolNode().GetName(), SGE_STR("test_type"));
	ASSERT_EQ(parameter_definitions[0].GetTypeSymbolNode().GetLine(), 1);
	ASSERT_EQ(parameter_definitions[0].GetTypeSymbolNode().GetColumn(), 2);
	ASSERT_EQ(parameter_definitions[0].GetTypeSymbolNode().GetFather(), &parameter_definitions[0]);
	ASSERT_EQ(parameter_definitions[0].GetLine(), 3);
	ASSERT_EQ(parameter_definitions[0].GetColumn(), 4);
	ASSERT_EQ(parameter_definitions[0].GetFather(), &parameter_definitions_node);
	ASSERT_EQ(parameter_definitions[1].GetName(), SGE_STR("test_parameter2"));
	ASSERT_EQ(parameter_definitions[1].GetTypeSymbolNode().GetDialectName(), SGE_STR("test_dialect"));
	ASSERT_EQ(parameter_definitions[1].GetTypeSymbolNode().GetName(), SGE_STR("test_type2"));
	ASSERT_EQ(parameter_definitions[1].GetTypeSymbolNode().GetLine(), 5);
	ASSERT_EQ(parameter_definitions[1].GetTypeSymbolNode().GetColumn(), 6);
	ASSERT_EQ(parameter_definitions[1].GetTypeSymbolNode().GetFather(), &parameter_definitions[1]);
	ASSERT_EQ(parameter_definitions[1].GetLine(), 7);
	ASSERT_EQ(parameter_definitions[1].GetColumn(), 8);
	ASSERT_EQ(parameter_definitions[1].GetFather(), &parameter_definitions_node);
	ASSERT_EQ(parameter_definitions_node.GetLine(), 9);
	ASSERT_EQ(parameter_definitions_node.GetColumn(), 10);
	ASSERT_EQ(parameter_definitions_node.GetFather(), nullptr);
}

TEST(AbstractSyntaxTreeNodes, AttributeDefinitionNodeTest)
{
	AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node(Int64(42), 1, 2);
	AbstractSyntaxTreeNodes::AttributeDefinitionNode attribute_definition_node(SGE_STR("test_attribute"), std::move(attribute_value_node), 3, 4);
	ASSERT_EQ(attribute_definition_node.GetName(), SGE_STR("test_attribute"));
	const auto& value_node = attribute_definition_node.GetValue();
	auto int64_value = value_node.GetValue().Get<Int64>();
	ASSERT_EQ(int64_value, 42);
	ASSERT_EQ(attribute_definition_node.GetLine(), 3);
	ASSERT_EQ(attribute_definition_node.GetColumn(), 4);
	ASSERT_EQ(attribute_definition_node.GetFather(), nullptr);
	ASSERT_EQ(value_node.GetLine(), 1);
	ASSERT_EQ(value_node.GetColumn(), 2);
	ASSERT_EQ(value_node.GetFather(), &attribute_definition_node);
}

TEST(AbstractSyntaxTreeNodes, AttributeDictionaryNodeTest)
{
	AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node1(Int64(42), 1, 2);
	AbstractSyntaxTreeNodes::AttributeDefinitionNode attribute_definition_node1(SGE_STR("test_attribute1"), std::move(attribute_value_node1), 3, 4);
	AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node2(String(SGE_STR("attribute_value")), 5, 6);
	AbstractSyntaxTreeNodes::AttributeDefinitionNode attribute_definition_node2(SGE_STR("test_attribute2"), std::move(attribute_value_node2), 7, 8);
	AbstractSyntaxTreeNodes::AttributeDictionaryNode attribute_dictionary_node(Vector<AbstractSyntaxTreeNodes::AttributeDefinitionNode>{std::move(attribute_definition_node1), std::move(attribute_definition_node2)}, 9, 10);
	const auto& attribute_definitions = attribute_dictionary_node.GetAttributeDefinitions();
	ASSERT_EQ(attribute_definitions.GetSize(), 2);
	ASSERT_EQ(attribute_definitions[0].GetName(), SGE_STR("test_attribute1"));
	const auto& value_node1 = attribute_definitions[0].GetValue();
	auto int64_value = value_node1.GetValue().Get<Int64>();
	ASSERT_EQ(int64_value, 42);
	ASSERT_EQ(attribute_definitions[0].GetLine(), 3);
	ASSERT_EQ(attribute_definitions[0].GetColumn(), 4);
	ASSERT_EQ(attribute_definitions[0].GetFather(), &attribute_dictionary_node);
	ASSERT_EQ(attribute_definitions[1].GetName(), SGE_STR("test_attribute2"));
	const auto& value_node2 = attribute_definitions[1].GetValue();
	auto string_value = value_node2.GetValue().Get<String>();
	ASSERT_EQ(string_value, SGE_STR("attribute_value"));
	ASSERT_EQ(attribute_definitions[1].GetLine(), 7);
	ASSERT_EQ(attribute_definitions[1].GetColumn(), 8);
	ASSERT_EQ(attribute_definitions[1].GetFather(), &attribute_dictionary_node);
	ASSERT_EQ(attribute_dictionary_node.GetLine(), 9);
	ASSERT_EQ(attribute_dictionary_node.GetColumn(), 10);
	ASSERT_EQ(attribute_dictionary_node.GetFather(), nullptr);
}

TEST(AbstractSyntaxTreeNodes, AttributeValueNodeTest)
{
	{
		AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node(Int64(42), 1, 2);
		auto int64_value = attribute_value_node.GetValue().Get<Int64>();
		ASSERT_EQ(int64_value, 42);
		ASSERT_EQ(attribute_value_node.GetLine(), 1);
		ASSERT_EQ(attribute_value_node.GetColumn(), 2);
		ASSERT_EQ(attribute_value_node.GetFather(), nullptr);
	}
	{
		AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node(UInt64(84), 3, 4);
		auto uint64_value = attribute_value_node.GetValue().Get<UInt64>();
		ASSERT_EQ(uint64_value, 84);
		ASSERT_EQ(attribute_value_node.GetLine(), 3);
		ASSERT_EQ(attribute_value_node.GetColumn(), 4);
		ASSERT_EQ(attribute_value_node.GetFather(), nullptr);
	}
	{
		AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node(float(3.14f), 5, 6);
		auto float_value = attribute_value_node.GetValue().Get<float>();
		ASSERT_FLOAT_EQ(float_value, 3.14f);
		ASSERT_EQ(attribute_value_node.GetLine(), 5);
		ASSERT_EQ(attribute_value_node.GetColumn(), 6);
		ASSERT_EQ(attribute_value_node.GetFather(), nullptr);
	}
	{
		AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node(double(6.28), 7, 8);
		auto double_value = attribute_value_node.GetValue().Get<double>();
		ASSERT_DOUBLE_EQ(double_value, 6.28);
		ASSERT_EQ(attribute_value_node.GetLine(), 7);
		ASSERT_EQ(attribute_value_node.GetColumn(), 8);
		ASSERT_EQ(attribute_value_node.GetFather(), nullptr);
	}
	{
		AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node(true, 9, 10);
		auto bool_value = attribute_value_node.GetValue().Get<bool>();
		ASSERT_EQ(bool_value, true);
		ASSERT_EQ(attribute_value_node.GetLine(), 9);
		ASSERT_EQ(attribute_value_node.GetColumn(), 10);
		ASSERT_EQ(attribute_value_node.GetFather(), nullptr);
	}
	{
		AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node(String(SGE_STR("test_string")), 11, 12);
		auto string_value = attribute_value_node.GetValue().Get<String>();
		ASSERT_EQ(string_value, SGE_STR("test_string"));
		ASSERT_EQ(attribute_value_node.GetLine(), 11);
		ASSERT_EQ(attribute_value_node.GetColumn(), 12);
		ASSERT_EQ(attribute_value_node.GetFather(), nullptr);
	}
	{
		AbstractSyntaxTreeNodes::AttributeDefinitionNode attribute_definition_node(SGE_STR("test_attribute"), AbstractSyntaxTreeNodes::AttributeValueNode(Int64(100), 13, 14), 15, 16);
		AbstractSyntaxTreeNodes::AttributeDictionaryNode attribute_dictionary_node(Vector<AbstractSyntaxTreeNodes::AttributeDefinitionNode>{std::move(attribute_definition_node)}, 17, 18);
		AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node(std::move(attribute_dictionary_node), 19, 20);
		auto& dict_value = attribute_value_node.GetValue().Get<AbstractSyntaxTreeNodes::AttributeDictionaryNode>();
		const auto& attribute_definitions = dict_value.GetAttributeDefinitions();
		ASSERT_EQ(attribute_definitions.GetSize(), 1);
		ASSERT_EQ(attribute_definitions[0].GetName(), SGE_STR("test_attribute"));
		const auto& value_node = attribute_definitions[0].GetValue();
		auto int64_value = value_node.GetValue().Get<Int64>();
		ASSERT_EQ(int64_value, 100);
		ASSERT_EQ(attribute_definitions[0].GetLine(), 15);
		ASSERT_EQ(attribute_definitions[0].GetColumn(), 16);
		ASSERT_EQ(attribute_definitions[0].GetFather(), &dict_value);
		ASSERT_EQ(dict_value.GetLine(), 17);
		ASSERT_EQ(dict_value.GetColumn(), 18);
		ASSERT_EQ(dict_value.GetFather(), &attribute_value_node);
		ASSERT_EQ(attribute_value_node.GetLine(), 19);
		ASSERT_EQ(attribute_value_node.GetColumn(), 20);
		ASSERT_EQ(attribute_value_node.GetFather(), nullptr);
	}
}

TEST(AbstractSyntaxTreeNodes, ArgumentNodeTest)
{
	{
		AbstractSyntaxTreeNodes::ArgumentNode argument_node(Int64(42), 1, 2);
		auto int64_value = argument_node.GetValue().Get<Int64>();
		ASSERT_EQ(int64_value, 42);
		ASSERT_EQ(argument_node.GetLine(), 1);
		ASSERT_EQ(argument_node.GetColumn(), 2);
		ASSERT_EQ(argument_node.GetFather(), nullptr);
	}
	{
		AbstractSyntaxTreeNodes::ArgumentNode argument_node(UInt64(84), 3, 4);
		auto uint64_value = argument_node.GetValue().Get<UInt64>();
		ASSERT_EQ(uint64_value, 84);
		ASSERT_EQ(argument_node.GetLine(), 3);
		ASSERT_EQ(argument_node.GetColumn(), 4);
		ASSERT_EQ(argument_node.GetFather(), nullptr);
	}
	{
		AbstractSyntaxTreeNodes::ArgumentNode argument_node(float(3.14f), 5, 6);
		auto float_value = argument_node.GetValue().Get<float>();
		ASSERT_FLOAT_EQ(float_value, 3.14f);
		ASSERT_EQ(argument_node.GetLine(), 5);
		ASSERT_EQ(argument_node.GetColumn(), 6);
		ASSERT_EQ(argument_node.GetFather(), nullptr);
	}
	{
		AbstractSyntaxTreeNodes::ArgumentNode argument_node(double(6.28), 7, 8);
		auto double_value = argument_node.GetValue().Get<double>();
		ASSERT_DOUBLE_EQ(double_value, 6.28);
		ASSERT_EQ(argument_node.GetLine(), 7);
		ASSERT_EQ(argument_node.GetColumn(), 8);
		ASSERT_EQ(argument_node.GetFather(), nullptr);
	}
	{
		AbstractSyntaxTreeNodes::ArgumentNode argument_node(true, 9, 10);
		auto bool_value = argument_node.GetValue().Get<bool>();
		ASSERT_EQ(bool_value, true);
		ASSERT_EQ(argument_node.GetLine(), 9);
		ASSERT_EQ(argument_node.GetColumn(), 10);
		ASSERT_EQ(argument_node.GetFather(), nullptr);
	}
	{
		AbstractSyntaxTreeNodes::ArgumentNode argument_node(String(SGE_STR("test_string")), 11, 12);
		auto string_value = argument_node.GetValue().Get<String>();
		ASSERT_EQ(string_value, SGE_STR("test_string"));
		ASSERT_EQ(argument_node.GetLine(), 11);
		ASSERT_EQ(argument_node.GetColumn(), 12);
		ASSERT_EQ(argument_node.GetFather(), nullptr);
	}
	{
		AbstractSyntaxTreeNodes::VariableNode variable_node(SGE_STR("test_variable"), 13, 14);
		AbstractSyntaxTreeNodes::ArgumentNode argument_node(std::move(variable_node), 15, 16);
		auto& var_value = argument_node.GetValue().Get<AbstractSyntaxTreeNodes::VariableNode>();
		ASSERT_EQ(var_value.GetName(), SGE_STR("test_variable"));
		ASSERT_EQ(var_value.GetLine(), 13);
		ASSERT_EQ(var_value.GetColumn(), 14);
		ASSERT_EQ(var_value.GetFather(), &argument_node);
		ASSERT_EQ(argument_node.GetLine(), 15);
		ASSERT_EQ(argument_node.GetColumn(), 16);
		ASSERT_EQ(argument_node.GetFather(), nullptr);
	}
	{
		AbstractSyntaxTreeNodes::SymbolNode symbol_node(SGE_STR("test_dialect"), SGE_STR("test_type"), 17, 18);
		AbstractSyntaxTreeNodes::ParameterDefinitionNode parameter_definition_node(SGE_STR("test_parameter"), std::move(symbol_node), 19, 20);
		AbstractSyntaxTreeNodes::ParameterDefinitionsNode parameter_definitions_node(Vector<AbstractSyntaxTreeNodes::ParameterDefinitionNode>{std::move(parameter_definition_node)}, 21, 22);
		AbstractSyntaxTreeNodes::ArgumentNode argument_node(std::move(parameter_definitions_node), 23, 24);
		auto& param_defs_value = argument_node.GetValue().Get<AbstractSyntaxTreeNodes::ParameterDefinitionsNode>();
		const auto& parameter_definitions = param_defs_value.GetParameterDefinitions();
		ASSERT_EQ(parameter_definitions.GetSize(), 1);
		ASSERT_EQ(parameter_definitions[0].GetName(), SGE_STR("test_parameter"));
		ASSERT_EQ(parameter_definitions[0].GetTypeSymbolNode().GetDialectName(), SGE_STR("test_dialect"));
		ASSERT_EQ(parameter_definitions[0].GetTypeSymbolNode().GetName(), SGE_STR("test_type"));
		ASSERT_EQ(parameter_definitions[0].GetTypeSymbolNode().GetLine(), 17);
		ASSERT_EQ(parameter_definitions[0].GetTypeSymbolNode().GetColumn(), 18);
		ASSERT_EQ(parameter_definitions[0].GetTypeSymbolNode().GetFather(), &parameter_definitions[0]);
		ASSERT_EQ(parameter_definitions[0].GetLine(), 19);
		ASSERT_EQ(parameter_definitions[0].GetColumn(), 20);
		ASSERT_EQ(parameter_definitions[0].GetFather(), &param_defs_value);
		ASSERT_EQ(param_defs_value.GetLine(), 21);
		ASSERT_EQ(param_defs_value.GetColumn(), 22);
		ASSERT_EQ(param_defs_value.GetFather(), &argument_node);
		ASSERT_EQ(argument_node.GetLine(), 23);
		ASSERT_EQ(argument_node.GetColumn(), 24);
		ASSERT_EQ(argument_node.GetFather(), nullptr);
	}
	{
		AbstractSyntaxTreeNodes::VariableDefinitionNode variable_definition_node(SGE_STR("test_variable"), OptionalTag::EmptyOptional, 25, 26);
		AbstractSyntaxTreeNodes::SymbolNode symbol_node(SGE_STR("test_dialect"), SGE_STR("test_type"), 27, 28);
		AbstractSyntaxTreeNodes::StatementNode statement_node(std::move(variable_definition_node), OptionalTag::EmptyOptional, std::move(symbol_node), Vector<AbstractSyntaxTreeNodes::ArgumentNode>{}, 29, 30);
		AbstractSyntaxTreeNodes::BlockNode block_node(Vector<AbstractSyntaxTreeNodes::StatementNode>{std::move(statement_node)}, 31, 32);
		AbstractSyntaxTreeNodes::ArgumentNode argument_node(std::move(block_node), 33, 34);
		auto& block_value = argument_node.GetValue().Get<HeapObject<AbstractSyntaxTreeNodes::BlockNode>>().Get();
		const auto& statements = block_value.GetStatements();
		ASSERT_EQ(statements.GetSize(), 1);
		const auto& stmt = statements[0];
		ASSERT_EQ(stmt.GetVariableDefinition().GetName(), SGE_STR("test_variable"));
		ASSERT_FALSE(stmt.GetVariableDefinition().GetTypeSymbolNode().HasValue());
		ASSERT_EQ(stmt.GetVariableDefinition().GetLine(), 25);
		ASSERT_EQ(stmt.GetVariableDefinition().GetColumn(), 26);
		ASSERT_EQ(stmt.GetVariableDefinition().GetFather(), &stmt);
		ASSERT_FALSE(stmt.GetAttributeDictionary().HasValue());
		ASSERT_EQ(stmt.GetSymbol().GetDialectName(), SGE_STR("test_dialect"));
		ASSERT_EQ(stmt.GetSymbol().GetName(), SGE_STR("test_type"));
		ASSERT_EQ(stmt.GetSymbol().GetLine(), 27);
		ASSERT_EQ(stmt.GetSymbol().GetColumn(), 28);
		ASSERT_EQ(stmt.GetSymbol().GetFather(), &stmt);
		ASSERT_EQ(stmt.GetArguments().GetSize(), 0);
		ASSERT_EQ(stmt.GetLine(), 29);
		ASSERT_EQ(stmt.GetColumn(), 30);
		ASSERT_EQ(stmt.GetFather(), &block_value);
		ASSERT_EQ(block_value.GetLine(), 31);
		ASSERT_EQ(block_value.GetColumn(), 32);
		ASSERT_EQ(block_value.GetFather(), &argument_node);
		ASSERT_EQ(argument_node.GetLine(), 33);
		ASSERT_EQ(argument_node.GetColumn(), 34);
		ASSERT_EQ(argument_node.GetFather(), nullptr);
	}
}

TEST(AbstractSyntaxTreeNodes, StatementNodeTest)
{
	AbstractSyntaxTreeNodes::VariableDefinitionNode variable_definition_node(SGE_STR("test_variable"), OptionalTag::EmptyOptional, 1, 2);
	AbstractSyntaxTreeNodes::SymbolNode symbol_node(SGE_STR("test_dialect"), SGE_STR("test_type"), 3, 4);
	AbstractSyntaxTreeNodes::ArgumentNode argument_node(Int64(42), 5, 6);
	AbstractSyntaxTreeNodes::StatementNode statement_node(std::move(variable_definition_node), OptionalTag::EmptyOptional, std::move(symbol_node), Vector<AbstractSyntaxTreeNodes::ArgumentNode>{std::move(argument_node)}, 7, 8);
	ASSERT_EQ(statement_node.GetVariableDefinition().GetName(), SGE_STR("test_variable"));
	ASSERT_FALSE(statement_node.GetVariableDefinition().GetTypeSymbolNode().HasValue());
	ASSERT_EQ(statement_node.GetVariableDefinition().GetLine(), 1);
	ASSERT_EQ(statement_node.GetVariableDefinition().GetColumn(), 2);
	ASSERT_EQ(statement_node.GetVariableDefinition().GetFather(), &statement_node);
	ASSERT_FALSE(statement_node.GetAttributeDictionary().HasValue());
	ASSERT_EQ(statement_node.GetSymbol().GetDialectName(), SGE_STR("test_dialect"));
	ASSERT_EQ(statement_node.GetSymbol().GetName(), SGE_STR("test_type"));
	ASSERT_EQ(statement_node.GetSymbol().GetLine(), 3);
	ASSERT_EQ(statement_node.GetSymbol().GetColumn(), 4);
	ASSERT_EQ(statement_node.GetSymbol().GetFather(), &statement_node);
	const auto& arguments = statement_node.GetArguments();
	ASSERT_EQ(arguments.GetSize(), 1);
	auto int64_value = arguments[0].GetValue().Get<Int64>();
	ASSERT_EQ(int64_value, 42);
	ASSERT_EQ(arguments[0].GetLine(), 5);
	ASSERT_EQ(arguments[0].GetColumn(), 6);
	ASSERT_EQ(arguments[0].GetFather(), &statement_node);
	ASSERT_EQ(statement_node.GetLine(), 7);
	ASSERT_EQ(statement_node.GetColumn(), 8);
	ASSERT_EQ(statement_node.GetFather(), nullptr);
}

TEST(AbstractSyntaxTreeNodes, BlockNodeTest)
{
	AbstractSyntaxTreeNodes::VariableDefinitionNode variable_definition_node(SGE_STR("test_variable"), OptionalTag::EmptyOptional, 1, 2);
	AbstractSyntaxTreeNodes::SymbolNode symbol_node(SGE_STR("test_dialect"), SGE_STR("test_type"), 3, 4);
	AbstractSyntaxTreeNodes::ArgumentNode argument_node(Int64(42), 5, 6);
	AbstractSyntaxTreeNodes::StatementNode statement_node(std::move(variable_definition_node), OptionalTag::EmptyOptional, std::move(symbol_node), Vector<AbstractSyntaxTreeNodes::ArgumentNode>{std::move(argument_node)}, 7, 8);
	AbstractSyntaxTreeNodes::BlockNode block_node(Vector<AbstractSyntaxTreeNodes::StatementNode>{std::move(statement_node)}, 9, 10);
	const auto& statements = block_node.GetStatements();
	ASSERT_EQ(statements.GetSize(), 1);
	const auto& stmt = statements[0];
	ASSERT_EQ(stmt.GetVariableDefinition().GetName(), SGE_STR("test_variable"));
	ASSERT_FALSE(stmt.GetVariableDefinition().GetTypeSymbolNode().HasValue());
	ASSERT_EQ(stmt.GetVariableDefinition().GetLine(), 1);
	ASSERT_EQ(stmt.GetVariableDefinition().GetColumn(), 2);
	ASSERT_EQ(stmt.GetVariableDefinition().GetFather(), &stmt);
	ASSERT_FALSE(stmt.GetAttributeDictionary().HasValue());
	ASSERT_EQ(stmt.GetSymbol().GetDialectName(), SGE_STR("test_dialect"));
	ASSERT_EQ(stmt.GetSymbol().GetName(), SGE_STR("test_type"));
	ASSERT_EQ(stmt.GetSymbol().GetLine(), 3);
	ASSERT_EQ(stmt.GetSymbol().GetColumn(), 4);
	ASSERT_EQ(stmt.GetSymbol().GetFather(), &stmt);
	const auto& arguments = stmt.GetArguments();
	ASSERT_EQ(arguments.GetSize(), 1);
	auto int64_value = arguments[0].GetValue().Get<Int64>();
	ASSERT_EQ(int64_value, 42);
	ASSERT_EQ(arguments[0].GetLine(), 5);
	ASSERT_EQ(arguments[0].GetColumn(), 6);
	ASSERT_EQ(arguments[0].GetFather(), &stmt);
	ASSERT_EQ(stmt.GetLine(), 7);
	ASSERT_EQ(stmt.GetColumn(), 8);
	ASSERT_EQ(stmt.GetFather(), &block_node);
	ASSERT_EQ(block_node.GetLine(), 9);
	ASSERT_EQ(block_node.GetColumn(), 10);
	ASSERT_EQ(block_node.GetFather(), nullptr);
}

TEST(AbstractSyntaxTreeNodes, ProgramNodeTest)
{
	AbstractSyntaxTreeNodes::VariableDefinitionNode variable_definition_node(SGE_STR("test_variable"), OptionalTag::EmptyOptional, 1, 2);
	AbstractSyntaxTreeNodes::SymbolNode symbol_node(SGE_STR("test_dialect"), SGE_STR("test_type"), 3, 4);
	AbstractSyntaxTreeNodes::ArgumentNode argument_node(Int64(42), 5, 6);
	AbstractSyntaxTreeNodes::StatementNode statement_node(std::move(variable_definition_node), OptionalTag::EmptyOptional, std::move(symbol_node), Vector<AbstractSyntaxTreeNodes::ArgumentNode>{std::move(argument_node)}, 7, 8);
	AbstractSyntaxTreeNodes::ProgramNode program_node(Vector<AbstractSyntaxTreeNodes::StatementNode>{std::move(statement_node)}, 1, 1);
	const auto& statements = program_node.GetStatements();
	ASSERT_EQ(statements.GetSize(), 1);
	const auto& stmt = statements[0];
	ASSERT_EQ(stmt.GetVariableDefinition().GetName(), SGE_STR("test_variable"));
	ASSERT_FALSE(stmt.GetVariableDefinition().GetTypeSymbolNode().HasValue());
	ASSERT_EQ(stmt.GetVariableDefinition().GetLine(), 1);
	ASSERT_EQ(stmt.GetVariableDefinition().GetColumn(), 2);
	ASSERT_EQ(stmt.GetVariableDefinition().GetFather(), &stmt);
	ASSERT_FALSE(stmt.GetAttributeDictionary().HasValue());
	ASSERT_EQ(stmt.GetSymbol().GetDialectName(), SGE_STR("test_dialect"));
	ASSERT_EQ(stmt.GetSymbol().GetName(), SGE_STR("test_type"));
	ASSERT_EQ(stmt.GetSymbol().GetLine(), 3);
	ASSERT_EQ(stmt.GetSymbol().GetColumn(), 4);
	ASSERT_EQ(stmt.GetSymbol().GetFather(), &stmt);
	const auto& arguments = stmt.GetArguments();
	ASSERT_EQ(arguments.GetSize(), 1);
	auto int64_value = arguments[0].GetValue().Get<Int64>();
	ASSERT_EQ(int64_value, 42);
	ASSERT_EQ(arguments[0].GetLine(), 5);
	ASSERT_EQ(arguments[0].GetColumn(), 6);
	ASSERT_EQ(arguments[0].GetFather(), &stmt);
	ASSERT_EQ(stmt.GetLine(), 7);
	ASSERT_EQ(stmt.GetColumn(), 8);
	ASSERT_EQ(stmt.GetFather(), &program_node);
	ASSERT_EQ(program_node.GetLine(), 1);
	ASSERT_EQ(program_node.GetColumn(), 1);
	ASSERT_EQ(program_node.GetFather(), nullptr);
}