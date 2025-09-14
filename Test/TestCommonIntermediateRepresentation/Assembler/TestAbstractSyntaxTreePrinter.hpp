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
#include "Assembler/AbstractSyntaxTreePrinter.h"
#include "Stream/CumulateStream.hpp"
#include "Stream/StreamReader.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonIntermediateRepresentation::Assembler;

TEST(AbstractSyntaxTreePrinter, SymbolNodeTest)
{
	CumulateStream stream;
	StreamReader<StringSerializer<String>> reader(stream);

	AbstractSyntaxTreePrinter printer(stream);

	AbstractSyntaxTreeNodes::SymbolNode symbol_node(SGE_STR("test_dialect"), SGE_STR("test_type"), 1, 2);
	symbol_node.Accept(printer);

	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Symbol \"test_dialect.test_type\"\t<line:1, column:2>\n"));
}

TEST(AbstractSyntaxTreePrinter, VariableNodeTest)
{
	CumulateStream stream;
	StreamReader<StringSerializer<String>> reader(stream);

	AbstractSyntaxTreePrinter printer(stream);

	AbstractSyntaxTreeNodes::VariableNode variable_node(SGE_STR("test_variable"), 1, 2);
	variable_node.Accept(printer);

	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Variable \"test_variable\"\t<line:1, column:2>\n"));
}

TEST(AbstractSyntaxTreePrinter, VariableDefinitionNodeTest)
{
	CumulateStream stream;
	StreamReader<StringSerializer<String>> reader(stream);

	AbstractSyntaxTreePrinter printer(stream);

	AbstractSyntaxTreeNodes::VariableDefinitionNode variable_definition_node_without_type(SGE_STR("test_variable"), OptionalTag::EmptyOptional, 1, 2);
	variable_definition_node_without_type.Accept(printer);

	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("VariableDefinition \"test_variable\"\t<line:1, column:2>\n"));

	AbstractSyntaxTreeNodes::SymbolNode symbol_node(SGE_STR("test_dialect"), SGE_STR("test_type"), 3, 4);
	AbstractSyntaxTreeNodes::VariableDefinitionNode variable_definition_node_with_type(SGE_STR("test_variable2"), std::move(symbol_node), 5, 6);

	variable_definition_node_with_type.Accept(printer);
	result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("VariableDefinition \"test_variable2\"\t<line:5, column:6>\n"
							  "\tSymbol \"test_dialect.test_type\"\t<line:3, column:4>\n"));
}

TEST(AbstractSyntaxTreePrinter, ParameterDefinitionNodeTest)
{
	CumulateStream stream;
	StreamReader<StringSerializer<String>> reader(stream);

	AbstractSyntaxTreePrinter printer(stream);

	AbstractSyntaxTreeNodes::SymbolNode symbol_node(SGE_STR("test_dialect"), SGE_STR("test_type"), 1, 2);
	AbstractSyntaxTreeNodes::ParameterDefinitionNode parameter_definition_node(SGE_STR("test_parameter"), std::move(symbol_node), 3, 4);

	parameter_definition_node.Accept(printer);
	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("ParameterDefinition \"test_parameter\"\t<line:3, column:4>\n"
							  "\tSymbol \"test_dialect.test_type\"\t<line:1, column:2>\n"));
}

TEST(AbstractSyntaxTreePrinter, ParameterDefinitionsNodeTest)
{
	CumulateStream stream;
	StreamReader<StringSerializer<String>> reader(stream);

	AbstractSyntaxTreePrinter printer(stream);

	AbstractSyntaxTreeNodes::SymbolNode symbol_node(SGE_STR("test_dialect"), SGE_STR("test_type"), 1, 2);
	AbstractSyntaxTreeNodes::ParameterDefinitionNode parameter_definition_node(SGE_STR("test_parameter"), std::move(symbol_node), 3, 4);
	AbstractSyntaxTreeNodes::SymbolNode symbol_node2(SGE_STR("test_dialect"), SGE_STR("test_type2"), 5, 6);
	AbstractSyntaxTreeNodes::ParameterDefinitionNode parameter_definition_node2(SGE_STR("test_parameter2"), std::move(symbol_node2), 7, 8);
	AbstractSyntaxTreeNodes::ParameterDefinitionsNode parameter_definitions_node(Vector<AbstractSyntaxTreeNodes::ParameterDefinitionNode>{std::move(parameter_definition_node), std::move(parameter_definition_node2)}, 9, 10);

	parameter_definitions_node.Accept(printer);
	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("ParameterDefinitions \t<line:9, column:10>\n"
							  "\tParameterDefinition \"test_parameter\"\t<line:3, column:4>\n"
							  "\t\tSymbol \"test_dialect.test_type\"\t<line:1, column:2>\n"
							  "\tParameterDefinition \"test_parameter2\"\t<line:7, column:8>\n"
							  "\t\tSymbol \"test_dialect.test_type2\"\t<line:5, column:6>\n"));
}