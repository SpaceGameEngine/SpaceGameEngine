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

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreePrinter::Visit(const AbstractSyntaxTreeNodes::SymbolNode& node)
{
	PrintLine(Format(String(SGE_STR("Symbol \"{}.{}\"\t<line:{}, column:{}>")), node.GetDialectName(), node.GetName(), node.GetLine(), node.GetColumn()));
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreePrinter::Visit(const AbstractSyntaxTreeNodes::VariableNode& node)
{
	PrintLine(Format(String(SGE_STR("Variable \"{}\"\t<line:{}, column:{}>")), node.GetName(), node.GetLine(), node.GetColumn()));
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreePrinter::Visit(const AbstractSyntaxTreeNodes::VariableDefinitionNode& node)
{
	const auto& type_symbol = node.GetTypeSymbolNode();
	PrintLine(Format(String(SGE_STR("VariableDefinition \"{}\"\t<line:{}, column:{}>")), node.GetName(), node.GetLine(), node.GetColumn()));
	if (type_symbol.HasValue())
	{
		StartPrintChildren();
		Visit(node.GetTypeSymbolNode().Get());
		EndPrintChildren();
	}
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreePrinter::Visit(const AbstractSyntaxTreeNodes::ParameterDefinitionNode& node)
{
	PrintLine(Format(String(SGE_STR("ParameterDefinition \"{}\"\t<line:{}, column:{}>")), node.GetName(), node.GetLine(), node.GetColumn()));
	StartPrintChildren();
	Visit(node.GetTypeSymbolNode());
	EndPrintChildren();
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreePrinter::Visit(const AbstractSyntaxTreeNodes::ParameterDefinitionsNode& node)
{
	PrintLine(Format(String(SGE_STR("ParameterDefinitions\t<line:{}, column:{}>")), node.GetLine(), node.GetColumn()));
	StartPrintChildren();
	const auto& parameter_definitions = node.GetParameterDefinitions();
	for (auto iter = parameter_definitions.GetConstBegin(); iter != parameter_definitions.GetConstEnd(); ++iter)
		Visit(*iter);
	EndPrintChildren();
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreePrinter::Visit(const AbstractSyntaxTreeNodes::AttributeDefinitionNode& node)
{
	PrintLine(Format(String(SGE_STR("AttributeDefinition \"{}\"\t<line:{}, column:{}>")), node.GetName(), node.GetLine(), node.GetColumn()));
	StartPrintChildren();
	Visit(node.GetValue());
	EndPrintChildren();
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreePrinter::Visit(const AbstractSyntaxTreeNodes::AttributeDictionaryNode& node)
{
	PrintLine(Format(String(SGE_STR("AttributeDictionary\t<line:{}, column:{}>")), node.GetLine(), node.GetColumn()));
	StartPrintChildren();
	const auto& attribute_definitions = node.GetAttributeDefinitions();
	for (auto iter = attribute_definitions.GetConstBegin(); iter != attribute_definitions.GetConstEnd(); ++iter)
		Visit(*iter);
	EndPrintChildren();
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreePrinter::Visit(const AbstractSyntaxTreeNodes::AttributeValueNode& node)
{
	struct AttributeValueVisitor
	{
		AttributeValueVisitor(AbstractSyntaxTreePrinter& printer, const AbstractSyntaxTreeNodes::AttributeValueNode& node)
			: m_Printer(printer), m_Node(node)
		{
		}

		void operator()(Int64 value)
		{
			m_Printer.PrintLine(Format(String(SGE_STR("AttributeValue:Int64 {}\t<line:{}, column:{}>")), value, m_Node.GetLine(), m_Node.GetColumn()));
		}

		void operator()(UInt64 value)
		{
			m_Printer.PrintLine(Format(String(SGE_STR("AttributeValue:UInt64 {}\t<line:{}, column:{}>")), value, m_Node.GetLine(), m_Node.GetColumn()));
		}

		void operator()(float value)
		{
			m_Printer.PrintLine(Format(String(SGE_STR("AttributeValue:Float {}\t<line:{}, column:{}>")), value, m_Node.GetLine(), m_Node.GetColumn()));
		}

		void operator()(double value)
		{
			m_Printer.PrintLine(Format(String(SGE_STR("AttributeValue:Double {}\t<line:{}, column:{}>")), value, m_Node.GetLine(), m_Node.GetColumn()));
		}

		void operator()(bool value)
		{
			m_Printer.PrintLine(Format(String(SGE_STR("AttributeValue:Bool {}\t<line:{}, column:{}>")), value ? SGE_STR("true") : SGE_STR("false"), m_Node.GetLine(), m_Node.GetColumn()));
		}

		void operator()(const String& value)
		{
			m_Printer.PrintLine(Format(String(SGE_STR("AttributeValue:String \"{}\"\t<line:{}, column:{}>")), value, m_Node.GetLine(), m_Node.GetColumn()));
		}

		void operator()(const AbstractSyntaxTreeNodes::AttributeDictionaryNode& value)
		{
			m_Printer.PrintLine(Format(String(SGE_STR("AttributeValue:AttributeDictionary\t<line:{}, column:{}>")), m_Node.GetLine(), m_Node.GetColumn()));
			m_Printer.StartPrintChildren();
			m_Printer.Visit(value);
			m_Printer.EndPrintChildren();
		}

		AbstractSyntaxTreePrinter& m_Printer;
		const AbstractSyntaxTreeNodes::AttributeValueNode& m_Node;
	};
	AttributeValueVisitor visitor(*this, node);
	node.GetValue().Visit(visitor);
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreePrinter::Visit(const AbstractSyntaxTreeNodes::ArgumentNode& node)
{
	struct ArgumentValueVisitor
	{
		ArgumentValueVisitor(AbstractSyntaxTreePrinter& printer, const AbstractSyntaxTreeNodes::ArgumentNode& node)
			: m_Printer(printer), m_Node(node)
		{
		}

		void operator()(Int64 value)
		{
			m_Printer.PrintLine(Format(String(SGE_STR("Argument:Int64 {}\t<line:{}, column:{}>")), value, m_Node.GetLine(), m_Node.GetColumn()));
		}

		void operator()(UInt64 value)
		{
			m_Printer.PrintLine(Format(String(SGE_STR("Argument:UInt64 {}\t<line:{}, column:{}>")), value, m_Node.GetLine(), m_Node.GetColumn()));
		}

		void operator()(float value)
		{
			m_Printer.PrintLine(Format(String(SGE_STR("Argument:Float {}\t<line:{}, column:{}>")), value, m_Node.GetLine(), m_Node.GetColumn()));
		}

		void operator()(double value)
		{
			m_Printer.PrintLine(Format(String(SGE_STR("Argument:Double {}\t<line:{}, column:{}>")), value, m_Node.GetLine(), m_Node.GetColumn()));
		}

		void operator()(bool value)
		{
			m_Printer.PrintLine(Format(String(SGE_STR("Argument:Bool {}\t<line:{}, column:{}>")), value ? SGE_STR("true") : SGE_STR("false"), m_Node.GetLine(), m_Node.GetColumn()));
		}

		void operator()(const String& value)
		{
			m_Printer.PrintLine(Format(String(SGE_STR("Argument:String \"{}\"\t<line:{}, column:{}>")), value, m_Node.GetLine(), m_Node.GetColumn()));
		}

		void operator()(const AbstractSyntaxTreeNodes::VariableNode& value)
		{
			m_Printer.PrintLine(Format(String(SGE_STR("Argument:Variable\t<line:{}, column:{}>")), m_Node.GetLine(), m_Node.GetColumn()));
			m_Printer.StartPrintChildren();
			m_Printer.Visit(value);
			m_Printer.EndPrintChildren();
		}

		void operator()(const AbstractSyntaxTreeNodes::ParameterDefinitionsNode& value)
		{
			m_Printer.PrintLine(Format(String(SGE_STR("Argument:ParameterDefinitions\t<line:{}, column:{}>")), m_Node.GetLine(), m_Node.GetColumn()));
			m_Printer.StartPrintChildren();
			m_Printer.Visit(value);
			m_Printer.EndPrintChildren();
		}

		void operator()(const AbstractSyntaxTreeNodes::BlockNode& value)
		{
			m_Printer.PrintLine(Format(String(SGE_STR("Argument:Block\t<line:{}, column:{}>")), m_Node.GetLine(), m_Node.GetColumn()));
			m_Printer.StartPrintChildren();
			m_Printer.Visit(value);
			m_Printer.EndPrintChildren();
		}

		AbstractSyntaxTreePrinter& m_Printer;
		const AbstractSyntaxTreeNodes::ArgumentNode& m_Node;
	};
	ArgumentValueVisitor visitor(*this, node);
	node.GetValue().Visit(visitor);
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreePrinter::Visit(const AbstractSyntaxTreeNodes::StatementNode& node)
{
	PrintLine(Format(String(SGE_STR("Statement\t<line:{}, column:{}>")), node.GetLine(), node.GetColumn()));
	StartPrintChildren();
	Visit(node.GetVariableDefinition());
	if (node.GetAttributeDictionary().HasValue())
		Visit(node.GetAttributeDictionary().Get());
	Visit(node.GetSymbol());
	const auto& arguments = node.GetArguments();
	for (auto iter = arguments.GetConstBegin(); iter != arguments.GetConstEnd(); ++iter)
		Visit(*iter);
	EndPrintChildren();
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreePrinter::Visit(const AbstractSyntaxTreeNodes::BlockNode& node)
{
	PrintLine(Format(String(SGE_STR("Block\t<line:{}, column:{}>")), node.GetLine(), node.GetColumn()));
	StartPrintChildren();
	const auto& statements = node.GetStatements();
	for (auto iter = statements.GetConstBegin(); iter != statements.GetConstEnd(); ++iter)
		Visit(*iter);
	EndPrintChildren();
}

void SpaceGameEngine::CommonIntermediateRepresentation::Assembler::AbstractSyntaxTreePrinter::Visit(const AbstractSyntaxTreeNodes::ProgramNode& node)
{
	PrintLine(Format(String(SGE_STR("Program\t<line:{}, column:{}>")), node.GetLine(), node.GetColumn()));
	StartPrintChildren();
	const auto& statements = node.GetStatements();
	for (auto iter = statements.GetConstBegin(); iter != statements.GetConstEnd(); ++iter)
		Visit(*iter);
	EndPrintChildren();
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
