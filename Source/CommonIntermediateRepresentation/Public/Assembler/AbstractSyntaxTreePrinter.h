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
#include "Serializer/StringSerializer.h"
#include "Stream/StreamWriter.hpp"

/*!
@ingroup CommonIntermediateRepresentation
@{
*/

namespace SpaceGameEngine::CommonIntermediateRepresentation::Assembler
{
	struct InvalidAbstractSyntaxTreePrinterIndentationForEndPrintChildrenError
	{
		inline static const ErrorMessageChar pContent[] = SGE_ESTR("The AbstractSyntaxTreePrinter's indentation is invalid for EndPrintChildren().");
		static COMMON_INTERMEDIATE_REPRESENTATION_API bool Judge(SizeType indention);
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API AbstractSyntaxTreePrinter : public AbstractSyntaxTreeNodeVisitor
	{
	public:
		AbstractSyntaxTreePrinter(OutputStream& stream);

		virtual void Visit(const AbstractSyntaxTreeNodes::SymbolNode& node) override;
		virtual void Visit(const AbstractSyntaxTreeNodes::VariableNode& node) override;
		virtual void Visit(const AbstractSyntaxTreeNodes::VariableDefinitionNode& node) override;
		virtual void Visit(const AbstractSyntaxTreeNodes::ParameterDefinitionNode& node) override;
		virtual void Visit(const AbstractSyntaxTreeNodes::ParameterDefinitionsNode& node) override;

	private:
		void PrintLine(const String& str);
		void StartPrintChildren();
		void EndPrintChildren();

	private:
		StreamWriter<StringSerializer<String>> m_StreamWriter;
		SizeType m_CurrentIndentationLevel = 0;
	};
}

/*!
@}
*/