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
#include "Assembler/AbstractSyntaxTreePrinter.h"
#include "Utility/Format.hpp"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonIntermediateRepresentation;
using namespace SpaceGameEngine::CommonIntermediateRepresentation::Assembler;

bool SpaceGameEngine::CommonIntermediateRepresentation::Assembler::InvalidAbstractSyntaxTreePrinterIndentationForEndPrintChildrenError::Judge(SizeType indention)
{
	return indention == 0;
}

SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreePrinter::AbstractSyntaxTreePrinter(OutputStream& stream)
	: m_StreamWriter(stream)
{
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreePrinter::Visit(const AbstractSyntaxTreeNodes::VariableNode& node)
{
	PrintLine(Format(String(SGE_STR("Variable \"{}\"")), node.GetName()));
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreePrinter::PrintLine(const String& str)
{
	for (SizeType i = 0; i < m_CurrentIndentationLevel; ++i)
		m_StreamWriter << SGE_STR('\t');
	m_StreamWriter << str << SGE_STR('\n');
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreePrinter::StartPrintChildren()
{
	++m_CurrentIndentationLevel;
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreePrinter::EndPrintChildren()
{
	SGE_ASSERT(InvalidAbstractSyntaxTreePrinterIndentationForEndPrintChildrenError, m_CurrentIndentationLevel);
	--m_CurrentIndentationLevel;
}
