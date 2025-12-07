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
#include "Assembler/AbstractSyntaxTree.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonIntermediateRepresentation;
using namespace SpaceGameEngine::CommonIntermediateRepresentation::Assembler;
using namespace SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes;

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNode::AbstractSyntaxTreeNode(SizeType line, SizeType column, AbstractSyntaxTreeNode* pfather)
	: m_Line(line), m_Column(column), m_pFather(pfather)
{
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::SymbolNode::SymbolNode(const String& dialect_name, const String& name, SizeType line, SizeType column)
	: m_DialectName(dialect_name), m_Name(name), AbstractSyntaxTreeNode(line, column, nullptr)
{
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::SymbolNode::Accept(AbstractSyntaxTreeNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

const String& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::SymbolNode::GetDialectName() const
{
	return m_DialectName;
}

const String& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::SymbolNode::GetName() const
{
	return m_Name;
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::VariableNode::VariableNode(const String& name, SizeType line, SizeType column)
	: m_Name(name), AbstractSyntaxTreeNode(line, column, nullptr)
{
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::VariableNode::Accept(AbstractSyntaxTreeNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

SizeType SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNode::GetLine() const
{
	return m_Line;
}

SizeType SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNode::GetColumn() const
{
	return m_Column;
}

AbstractSyntaxTreeNode* SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNode::GetFather() const
{
	return m_pFather;
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNode::SetFather(AbstractSyntaxTreeNode& father)
{
	m_pFather = &father;
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNode::Link()
{
	// Base class has no children, do nothing.
}

const String& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::VariableNode::GetName() const
{
	return m_Name;
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::VariableDefinitionNode::VariableDefinitionNode(const String& name, Optional<SymbolNode>&& type_symbol, SizeType line, SizeType column)
	: m_Name(name), m_TypeSymbol(std::move(type_symbol)), AbstractSyntaxTreeNode(line, column, nullptr)
{
	Link();
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::VariableDefinitionNode::Accept(AbstractSyntaxTreeNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

const String& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::VariableDefinitionNode::GetName() const
{
	return m_Name;
}

const Optional<SymbolNode>& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::VariableDefinitionNode::GetTypeSymbolNode() const
{
	return m_TypeSymbol;
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::VariableDefinitionNode::Link()
{
	if (m_TypeSymbol.HasValue())
		m_TypeSymbol.Get().SetFather(*this);
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ParameterDefinitionNode::ParameterDefinitionNode(const String& name, SymbolNode&& type_symbol, SizeType line, SizeType column)
	: m_Name(name), m_TypeSymbol(std::move(type_symbol)), AbstractSyntaxTreeNode(line, column, nullptr)
{
	Link();
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ParameterDefinitionNode::Accept(AbstractSyntaxTreeNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

const String& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ParameterDefinitionNode::GetName() const
{
	return m_Name;
}

const SymbolNode& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ParameterDefinitionNode::GetTypeSymbolNode() const
{
	return m_TypeSymbol;
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ParameterDefinitionNode::Link()
{
	m_TypeSymbol.SetFather(*this);
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ParameterDefinitionsNode::ParameterDefinitionsNode(Vector<ParameterDefinitionNode>&& parameter_definitions, SizeType line, SizeType column)
	: m_ParameterDefinitions(std::move(parameter_definitions)), AbstractSyntaxTreeNode(line, column, nullptr)
{
	Link();
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ParameterDefinitionsNode::Accept(AbstractSyntaxTreeNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

const Vector<ParameterDefinitionNode>& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ParameterDefinitionsNode::GetParameterDefinitions() const
{
	return m_ParameterDefinitions;
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ParameterDefinitionsNode::Link()
{
	for (auto iter = m_ParameterDefinitions.GetBegin(); iter != m_ParameterDefinitions.GetEnd(); ++iter)
	{
		iter->SetFather(*this);
		iter->Link();
	}
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeDefinitionNode::AttributeDefinitionNode(const String& name, AttributeValueNode&& value, SizeType line, SizeType column)
	: m_Name(name), m_Value(std::move(value)), AbstractSyntaxTreeNode(line, column, nullptr)
{
	Link();
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeDefinitionNode::Accept(AbstractSyntaxTreeNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

const String& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeDefinitionNode::GetName() const
{
	return m_Name;
}

const AttributeValueNode& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeDefinitionNode::GetValue() const
{
	return m_Value.Get();
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeDefinitionNode::Link()
{
	m_Value.Get().SetFather(*this);
	m_Value.Get().Link();
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeDictionaryNode::AttributeDictionaryNode(Vector<AttributeDefinitionNode>&& attribute_definitions, SizeType line, SizeType column)
	: m_AttributeDefinitions(std::move(attribute_definitions)), AbstractSyntaxTreeNode(line, column, nullptr)
{
	Link();
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeDictionaryNode::Accept(AbstractSyntaxTreeNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

const Vector<AttributeDefinitionNode>& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeDictionaryNode::GetAttributeDefinitions() const
{
	return m_AttributeDefinitions;
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeDictionaryNode::Link()
{
	for (auto iter = m_AttributeDefinitions.GetBegin(); iter != m_AttributeDefinitions.GetEnd(); ++iter)
	{
		iter->SetFather(*this);
		iter->Link();
	}
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeValueNode::AttributeValueNode(Int64 value, SizeType line, SizeType column)
	: m_Value(InPlaceType<Int64>, value), AbstractSyntaxTreeNode(line, column, nullptr)
{
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeValueNode::AttributeValueNode(UInt64 value, SizeType line, SizeType column)
	: m_Value(InPlaceType<UInt64>, value), AbstractSyntaxTreeNode(line, column, nullptr)
{
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeValueNode::AttributeValueNode(float value, SizeType line, SizeType column)
	: m_Value(InPlaceType<float>, value), AbstractSyntaxTreeNode(line, column, nullptr)
{
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeValueNode::AttributeValueNode(double value, SizeType line, SizeType column)
	: m_Value(InPlaceType<double>, value), AbstractSyntaxTreeNode(line, column, nullptr)
{
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeValueNode::AttributeValueNode(bool value, SizeType line, SizeType column)
	: m_Value(InPlaceType<bool>, value), AbstractSyntaxTreeNode(line, column, nullptr)
{
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeValueNode::AttributeValueNode(const String& value, SizeType line, SizeType column)
	: m_Value(InPlaceType<String>, value), AbstractSyntaxTreeNode(line, column, nullptr)
{
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeValueNode::AttributeValueNode(AttributeDictionaryNode&& value, SizeType line, SizeType column)
	: m_Value(InPlaceType<AttributeDictionaryNode>, std::move(value)), AbstractSyntaxTreeNode(line, column, nullptr)
{
	Link();
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeValueNode::Accept(AbstractSyntaxTreeNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

const UniqueVariant<Int64, UInt64, float, double, bool, String, AttributeDictionaryNode>& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeValueNode::GetValue() const
{
	return m_Value;
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::AttributeValueNode::Link()
{
	auto pattribute_dict_node = m_Value.Query<AttributeDictionaryNode>();
	if (pattribute_dict_node)
	{
		pattribute_dict_node->SetFather(*this);
		pattribute_dict_node->Link();
	}
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ArgumentNode::ArgumentNode(Int64 value, SizeType line, SizeType column)
	: m_Value(InPlaceType<Int64>, value), AbstractSyntaxTreeNode(line, column, nullptr)
{
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ArgumentNode::ArgumentNode(UInt64 value, SizeType line, SizeType column)
	: m_Value(InPlaceType<UInt64>, value), AbstractSyntaxTreeNode(line, column, nullptr)
{
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ArgumentNode::ArgumentNode(float value, SizeType line, SizeType column)
	: m_Value(InPlaceType<float>, value), AbstractSyntaxTreeNode(line, column, nullptr)
{
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ArgumentNode::ArgumentNode(double value, SizeType line, SizeType column)
	: m_Value(InPlaceType<double>, value), AbstractSyntaxTreeNode(line, column, nullptr)
{
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ArgumentNode::ArgumentNode(bool value, SizeType line, SizeType column)
	: m_Value(InPlaceType<bool>, value), AbstractSyntaxTreeNode(line, column, nullptr)
{
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ArgumentNode::ArgumentNode(const String& value, SizeType line, SizeType column)
	: m_Value(InPlaceType<String>, value), AbstractSyntaxTreeNode(line, column, nullptr)
{
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ArgumentNode::ArgumentNode(VariableNode&& value, SizeType line, SizeType column)
	: m_Value(InPlaceType<VariableNode>, std::move(value)), AbstractSyntaxTreeNode(line, column, nullptr)
{
	Link();
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ArgumentNode::ArgumentNode(ParameterDefinitionsNode&& value, SizeType line, SizeType column)
	: m_Value(InPlaceType<ParameterDefinitionsNode>, std::move(value)), AbstractSyntaxTreeNode(line, column, nullptr)
{
	Link();
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ArgumentNode::ArgumentNode(BlockNode&& value, SizeType line, SizeType column)
	: m_Value(InPlaceType<HeapObject<BlockNode>>, std::move(value)), AbstractSyntaxTreeNode(line, column, nullptr)
{
	Link();
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ArgumentNode::Accept(AbstractSyntaxTreeNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

const UniqueVariant<Int64, UInt64, float, double, bool, String, VariableNode, ParameterDefinitionsNode, HeapObject<BlockNode>>& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ArgumentNode::GetValue() const
{
	return m_Value;
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ArgumentNode::Link()
{
	auto pvariable_node = m_Value.Query<VariableNode>();
	if (pvariable_node)
	{
		pvariable_node->SetFather(*this);
		pvariable_node->Link();
		return;
	}
	auto pparameter_definitions_node = m_Value.Query<ParameterDefinitionsNode>();
	if (pparameter_definitions_node)
	{
		pparameter_definitions_node->SetFather(*this);
		pparameter_definitions_node->Link();
		return;
	}
	auto pblock_node = m_Value.Query<HeapObject<BlockNode>>();
	if (pblock_node)
	{
		pblock_node->Get().SetFather(*this);
		pblock_node->Get().Link();
		return;
	}
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::StatementNode::StatementNode(VariableDefinitionNode&& variable_definition, Optional<AttributeDictionaryNode>&& attribute_dictionary, SymbolNode&& symbol, Vector<ArgumentNode>&& arguments, SizeType line, SizeType column)
	: m_VariableDefinition(std::move(variable_definition)), m_AttributeDictionary(std::move(attribute_dictionary)), m_Symbol(std::move(symbol)), m_Arguments(std::move(arguments)), AbstractSyntaxTreeNode(line, column, nullptr)
{
	Link();
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::StatementNode::Accept(AbstractSyntaxTreeNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

const VariableDefinitionNode& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::StatementNode::GetVariableDefinition() const
{
	return m_VariableDefinition;
}

const Optional<AttributeDictionaryNode>& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::StatementNode::GetAttributeDictionary() const
{
	return m_AttributeDictionary;
}

const SymbolNode& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::StatementNode::GetSymbol() const
{
	return m_Symbol;
}

const Vector<ArgumentNode>& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::StatementNode::GetArguments() const
{
	return m_Arguments;
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::StatementNode::Link()
{
	m_VariableDefinition.SetFather(*this);
	m_VariableDefinition.Link();
	if (m_AttributeDictionary.HasValue())
	{
		auto& attribute_dictionary = m_AttributeDictionary.Get();
		attribute_dictionary.SetFather(*this);
		attribute_dictionary.Link();
	}
	m_Symbol.SetFather(*this);
	m_Symbol.Link();
	for (auto iter = m_Arguments.GetBegin(); iter != m_Arguments.GetEnd(); ++iter)
	{
		iter->SetFather(*this);
		iter->Link();
	}
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::BlockNode::BlockNode(Vector<StatementNode>&& statements, SizeType line, SizeType column)
	: m_Statements(std::move(statements)), AbstractSyntaxTreeNode(line, column, nullptr)
{
	Link();
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::BlockNode::Accept(AbstractSyntaxTreeNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

const Vector<StatementNode>& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::BlockNode::GetStatements() const
{
	return m_Statements;
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::BlockNode::Link()
{
	for (auto iter = m_Statements.GetBegin(); iter != m_Statements.GetEnd(); ++iter)
	{
		iter->SetFather(*this);
		iter->Link();
	}
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ProgramNode::ProgramNode(Vector<StatementNode>&& statements, SizeType line, SizeType column)
	: m_Statements(std::move(statements)), AbstractSyntaxTreeNode(line, column, nullptr)
{
	Link();
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ProgramNode::Accept(AbstractSyntaxTreeNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

const Vector<StatementNode>& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ProgramNode::GetStatements() const
{
	return m_Statements;
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ProgramNode::Link()
{
	for (auto iter = m_Statements.GetBegin(); iter != m_Statements.GetEnd(); ++iter)
	{
		iter->SetFather(*this);
		iter->Link();
	}
}
