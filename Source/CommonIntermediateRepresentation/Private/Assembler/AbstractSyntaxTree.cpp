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

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::SymbolNode::SymbolNode(const String& dialect_name, const String& name, SizeType line, SizeType column, AbstractSyntaxTreeNode* pfather)
	: m_DialectName(dialect_name), m_Name(name), AbstractSyntaxTreeNode(line, column, pfather)
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

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::VariableNode::VariableNode(const String& name, SizeType line, SizeType column, AbstractSyntaxTreeNode* pfather)
	: m_Name(name), AbstractSyntaxTreeNode(line, column, pfather)
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

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNode::Relink()
{
	// Base class has no children, do nothing.
}

const String& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::VariableNode::GetName() const
{
	return m_Name;
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::VariableDefinitionNode::VariableDefinitionNode(const String& name, Optional<SymbolNode>&& type_symbol, SizeType line, SizeType column, AbstractSyntaxTreeNode* pfather)
	: m_Name(name), m_TypeSymbol(std::move(type_symbol)), AbstractSyntaxTreeNode(line, column, pfather)
{
	if (m_TypeSymbol.HasValue())
		m_TypeSymbol.Get().SetFather(*this);
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

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::VariableDefinitionNode::Relink()
{
	if (m_TypeSymbol.HasValue())
		m_TypeSymbol.Get().SetFather(*this);
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ParameterDefinitionNode::ParameterDefinitionNode(const String& name, SymbolNode&& type_symbol, SizeType line, SizeType column, AbstractSyntaxTreeNode* pfather)
	: m_Name(name), m_TypeSymbol(std::move(type_symbol)), AbstractSyntaxTreeNode(line, column, pfather)
{
	m_TypeSymbol.SetFather(*this);
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

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ParameterDefinitionNode::Relink()
{
	m_TypeSymbol.SetFather(*this);
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ParameterDefinitionsNode::ParameterDefinitionsNode(Vector<ParameterDefinitionNode>&& parameter_definitions, SizeType line, SizeType column, AbstractSyntaxTreeNode* pfather)
	: m_ParameterDefinitions(std::move(parameter_definitions)), AbstractSyntaxTreeNode(line, column, pfather)
{
	for (auto iter = m_ParameterDefinitions.GetBegin(); iter != m_ParameterDefinitions.GetEnd(); ++iter)
	{
		iter->SetFather(*this);
		iter->Relink();
	}
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ParameterDefinitionsNode::Accept(AbstractSyntaxTreeNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

const Vector<ParameterDefinitionNode>& SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreeNodes::ParameterDefinitionsNode::GetParameterDefinitions() const
{
	return m_ParameterDefinitions;
}
