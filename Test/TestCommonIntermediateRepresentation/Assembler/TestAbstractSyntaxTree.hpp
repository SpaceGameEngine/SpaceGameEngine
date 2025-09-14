/*
Copyright 2025 creatorlxd

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