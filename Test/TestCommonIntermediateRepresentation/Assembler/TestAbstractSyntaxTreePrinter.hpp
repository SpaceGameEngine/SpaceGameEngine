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
	ASSERT_EQ(result, SGE_STR("ParameterDefinitions\t<line:9, column:10>\n"
							  "\tParameterDefinition \"test_parameter\"\t<line:3, column:4>\n"
							  "\t\tSymbol \"test_dialect.test_type\"\t<line:1, column:2>\n"
							  "\tParameterDefinition \"test_parameter2\"\t<line:7, column:8>\n"
							  "\t\tSymbol \"test_dialect.test_type2\"\t<line:5, column:6>\n"));
}

TEST(AbstractSyntaxTreePrinter, AttributeDefinitionNodeTest)
{
	CumulateStream stream;
	StreamReader<StringSerializer<String>> reader(stream);

	AbstractSyntaxTreePrinter printer(stream);

	AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node(Int64(42), 1, 2);
	AbstractSyntaxTreeNodes::AttributeDefinitionNode attribute_definition_node(SGE_STR("test_attribute"), std::move(attribute_value_node), 3, 4);

	attribute_definition_node.Accept(printer);
	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("AttributeDefinition \"test_attribute\"\t<line:3, column:4>\n"
							  "\tAttributeValue:Int64 42\t<line:1, column:2>\n"));
}

TEST(AbstractSyntaxTreePrinter, AttributeDictionaryNodeTest)
{
	CumulateStream stream;
	StreamReader<StringSerializer<String>> reader(stream);

	AbstractSyntaxTreePrinter printer(stream);

	AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node1(Int64(42), 1, 2);
	AbstractSyntaxTreeNodes::AttributeDefinitionNode attribute_definition_node1(SGE_STR("test_attribute1"), std::move(attribute_value_node1), 3, 4);
	AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node2(String(SGE_STR("attribute_value")), 5, 6);
	AbstractSyntaxTreeNodes::AttributeDefinitionNode attribute_definition_node2(SGE_STR("test_attribute2"), std::move(attribute_value_node2), 7, 8);
	AbstractSyntaxTreeNodes::AttributeDictionaryNode attribute_dictionary_node(Vector<AbstractSyntaxTreeNodes::AttributeDefinitionNode>{std::move(attribute_definition_node1), std::move(attribute_definition_node2)}, 9, 10);

	attribute_dictionary_node.Accept(printer);
	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("AttributeDictionary\t<line:9, column:10>\n"
							  "\tAttributeDefinition \"test_attribute1\"\t<line:3, column:4>\n"
							  "\t\tAttributeValue:Int64 42\t<line:1, column:2>\n"
							  "\tAttributeDefinition \"test_attribute2\"\t<line:7, column:8>\n"
							  "\t\tAttributeValue:String \"attribute_value\"\t<line:5, column:6>\n"));
}

TEST(AbstractSyntaxTreePrinter, AttributeValueNodeTest)
{
	CumulateStream stream;
	StreamReader<StringSerializer<String>> reader(stream);

	AbstractSyntaxTreePrinter printer(stream);

	AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node_int64(Int64(42), 1, 2);
	attribute_value_node_int64.Accept(printer);
	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("AttributeValue:Int64 42\t<line:1, column:2>\n"));

	AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node_uint64(UInt64(84), 3, 4);
	attribute_value_node_uint64.Accept(printer);
	result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("AttributeValue:UInt64 84\t<line:3, column:4>\n"));

	AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node_float(3.14f, 5, 6);
	attribute_value_node_float.Accept(printer);
	result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("AttributeValue:Float 3.1400\t<line:5, column:6>\n"));

	AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node_double(2.71828, 7, 8);
	attribute_value_node_double.Accept(printer);
	result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("AttributeValue:Double 2.718280\t<line:7, column:8>\n"));

	AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node_bool(true, 9, 10);
	attribute_value_node_bool.Accept(printer);
	result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("AttributeValue:Bool true\t<line:9, column:10>\n"));

	AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node_string(String(SGE_STR("test_string")), 11, 12);
	attribute_value_node_string.Accept(printer);
	result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("AttributeValue:String \"test_string\"\t<line:11, column:12>\n"));

	AbstractSyntaxTreeNodes::AttributeValueNode attribute_value_node_dict(
		AbstractSyntaxTreeNodes::AttributeDictionaryNode(Vector<AbstractSyntaxTreeNodes::AttributeDefinitionNode>{}, 13, 14), 15, 16);
	attribute_value_node_dict.Accept(printer);
	result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("AttributeValue:AttributeDictionary\t<line:15, column:16>\n"
							  "\tAttributeDictionary\t<line:13, column:14>\n"));
}

TEST(AbstractSyntaxTreePrinter, ArgumentNodeTest)
{
	CumulateStream stream;
	StreamReader<StringSerializer<String>> reader(stream);

	AbstractSyntaxTreePrinter printer(stream);

	AbstractSyntaxTreeNodes::ArgumentNode argument_node_int64(Int64(42), 1, 2);
	argument_node_int64.Accept(printer);
	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Argument:Int64 42\t<line:1, column:2>\n"));

	AbstractSyntaxTreeNodes::ArgumentNode argument_node_uint64(UInt64(84), 3, 4);
	argument_node_uint64.Accept(printer);
	result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Argument:UInt64 84\t<line:3, column:4>\n"));

	AbstractSyntaxTreeNodes::ArgumentNode argument_node_float(3.14f, 5, 6);
	argument_node_float.Accept(printer);
	result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Argument:Float 3.1400\t<line:5, column:6>\n"));

	AbstractSyntaxTreeNodes::ArgumentNode argument_node_double(2.71828, 7, 8);
	argument_node_double.Accept(printer);
	result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Argument:Double 2.718280\t<line:7, column:8>\n"));

	AbstractSyntaxTreeNodes::ArgumentNode argument_node_bool(true, 9, 10);
	argument_node_bool.Accept(printer);
	result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Argument:Bool true\t<line:9, column:10>\n"));

	AbstractSyntaxTreeNodes::ArgumentNode argument_node_string(String(SGE_STR("test_string")), 11, 12);
	argument_node_string.Accept(printer);
	result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Argument:String \"test_string\"\t<line:11, column:12>\n"));

	AbstractSyntaxTreeNodes::VariableNode variable_node(SGE_STR("test_variable"), 13, 14);
	AbstractSyntaxTreeNodes::ArgumentNode argument_node_variable(std::move(variable_node), 15, 16);
	argument_node_variable.Accept(printer);
	result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Argument:Variable\t<line:15, column:16>\n"
							  "\tVariable \"test_variable\"\t<line:13, column:14>\n"));

	AbstractSyntaxTreeNodes::SymbolNode symbol_node(SGE_STR("test_dialect"), SGE_STR("test_type"), 17, 18);
	AbstractSyntaxTreeNodes::ParameterDefinitionNode parameter_definition_node(SGE_STR("test_parameter"), std::move(symbol_node), 19, 20);
	AbstractSyntaxTreeNodes::ParameterDefinitionsNode parameter_definitions_node(Vector<AbstractSyntaxTreeNodes::ParameterDefinitionNode>{std::move(parameter_definition_node)}, 21, 22);
	AbstractSyntaxTreeNodes::ArgumentNode argument_node_param_defs(std::move(parameter_definitions_node), 23, 24);
	argument_node_param_defs.Accept(printer);
	result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Argument:ParameterDefinitions\t<line:23, column:24>\n"
							  "\tParameterDefinitions\t<line:21, column:22>\n"
							  "\t\tParameterDefinition \"test_parameter\"\t<line:19, column:20>\n"
							  "\t\t\tSymbol \"test_dialect.test_type\"\t<line:17, column:18>\n"));

	AbstractSyntaxTreeNodes::BlockNode block_node(Vector<AbstractSyntaxTreeNodes::StatementNode>{}, 25, 26);
	AbstractSyntaxTreeNodes::ArgumentNode argument_node_block(std::move(block_node), 27, 28);
	argument_node_block.Accept(printer);
	result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Argument:Block\t<line:27, column:28>\n"
							  "\tBlock\t<line:25, column:26>\n"));
}

TEST(AbstractSyntaxTreePrinter, StatementNodeTest)
{
	CumulateStream stream;
	StreamReader<StringSerializer<String>> reader(stream);

	AbstractSyntaxTreePrinter printer(stream);

	AbstractSyntaxTreeNodes::VariableDefinitionNode variable_definition_node(SGE_STR("test_variable"), OptionalTag::EmptyOptional, 1, 2);
	AbstractSyntaxTreeNodes::SymbolNode symbol_node(SGE_STR("test_dialect"), SGE_STR("test_type"), 3, 4);
	AbstractSyntaxTreeNodes::AttributeDictionaryNode attribute_dictionary_node(Vector<AbstractSyntaxTreeNodes::AttributeDefinitionNode>{
																				   AbstractSyntaxTreeNodes::AttributeDefinitionNode(SGE_STR("test_attribute"), AbstractSyntaxTreeNodes::AttributeValueNode(Int64(42), 5, 6), 7, 8)},
																			   9, 10);
	AbstractSyntaxTreeNodes::ArgumentNode argument_node(Int64(42), 11, 12);
	AbstractSyntaxTreeNodes::StatementNode statement_node(std::move(variable_definition_node), std::move(attribute_dictionary_node), std::move(symbol_node), Vector<AbstractSyntaxTreeNodes::ArgumentNode>{std::move(argument_node)}, 13, 14);

	statement_node.Accept(printer);
	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Statement\t<line:13, column:14>\n"
							  "\tVariableDefinition \"test_variable\"\t<line:1, column:2>\n"
							  "\tAttributeDictionary\t<line:9, column:10>\n"
							  "\t\tAttributeDefinition \"test_attribute\"\t<line:7, column:8>\n"
							  "\t\t\tAttributeValue:Int64 42\t<line:5, column:6>\n"
							  "\tSymbol \"test_dialect.test_type\"\t<line:3, column:4>\n"
							  "\tArgument:Int64 42\t<line:11, column:12>\n"));
}

TEST(AbstractSyntaxTreePrinter, BlockNodeTest)
{
	CumulateStream stream;
	StreamReader<StringSerializer<String>> reader(stream);

	AbstractSyntaxTreePrinter printer(stream);

	AbstractSyntaxTreeNodes::VariableDefinitionNode variable_definition_node(SGE_STR("test_variable"), OptionalTag::EmptyOptional, 1, 2);
	AbstractSyntaxTreeNodes::SymbolNode symbol_node(SGE_STR("test_dialect"), SGE_STR("test_type"), 3, 4);
	AbstractSyntaxTreeNodes::ArgumentNode argument_node(Int64(42), 5, 6);
	AbstractSyntaxTreeNodes::StatementNode statement_node(std::move(variable_definition_node), OptionalTag::EmptyOptional, std::move(symbol_node), Vector<AbstractSyntaxTreeNodes::ArgumentNode>{std::move(argument_node)}, 7, 8);
	AbstractSyntaxTreeNodes::BlockNode block_node(Vector<AbstractSyntaxTreeNodes::StatementNode>{std::move(statement_node)}, 9, 10);

	block_node.Accept(printer);
	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Block\t<line:9, column:10>\n"
							  "\tStatement\t<line:7, column:8>\n"
							  "\t\tVariableDefinition \"test_variable\"\t<line:1, column:2>\n"
							  "\t\tSymbol \"test_dialect.test_type\"\t<line:3, column:4>\n"
							  "\t\tArgument:Int64 42\t<line:5, column:6>\n"));
}

TEST(AbstractSyntaxTreePrinter, ProgramNodeTest)
{
	CumulateStream stream;
	StreamReader<StringSerializer<String>> reader(stream);

	AbstractSyntaxTreePrinter printer(stream);

	AbstractSyntaxTreeNodes::VariableDefinitionNode variable_definition_node(SGE_STR("test_variable"), OptionalTag::EmptyOptional, 1, 2);
	AbstractSyntaxTreeNodes::SymbolNode symbol_node(SGE_STR("test_dialect"), SGE_STR("test_type"), 3, 4);
	AbstractSyntaxTreeNodes::ArgumentNode argument_node(Int64(42), 5, 6);
	AbstractSyntaxTreeNodes::StatementNode statement_node(std::move(variable_definition_node), OptionalTag::EmptyOptional, std::move(symbol_node), Vector<AbstractSyntaxTreeNodes::ArgumentNode>{std::move(argument_node)}, 7, 8);
	AbstractSyntaxTreeNodes::ProgramNode program_node(Vector<AbstractSyntaxTreeNodes::StatementNode>{std::move(statement_node)}, 9, 10);

	program_node.Accept(printer);
	String result = reader.ReadAll();
	ASSERT_EQ(result, SGE_STR("Program\t<line:9, column:10>\n"
							  "\tStatement\t<line:7, column:8>\n"
							  "\t\tVariableDefinition \"test_variable\"\t<line:1, column:2>\n"
							  "\t\tSymbol \"test_dialect.test_type\"\t<line:3, column:4>\n"
							  "\t\tArgument:Int64 42\t<line:5, column:6>\n"));
}