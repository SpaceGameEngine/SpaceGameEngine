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
#include "CommonIntermediateRepresentationAPI.h"
#include "SGEString.hpp"

/*!
@ingroup CommonIntermediateRepresentation
@{
*/

namespace SpaceGameEngine::CommonIntermediateRepresentation::Assembler
{
	class COMMON_INTERMEDIATE_REPRESENTATION_API AbstractSyntaxTreeNodeVisitor
	{
	public:
		virtual ~AbstractSyntaxTreeNodeVisitor() = default;

		virtual void Visit(const AbstractSyntaxTreeNodes::VariableNode& node) = 0;
		// todo more node types as needed
	};

	class COMMON_INTERMEDIATE_REPRESENTATION_API AbstractSyntaxTreeNode
	{
	public:
		AbstractSyntaxTreeNode() = default;
		AbstractSyntaxTreeNode(AbstractSyntaxTreeNode* pfather);
		virtual ~AbstractSyntaxTreeNode() = default;

		virtual void Accept(AbstractSyntaxTreeNodeVisitor& visitor) const = 0;

	private:
		AbstractSyntaxTreeNode* m_pFather = nullptr;
	};

	namespace AbstractSyntaxTreeNodes
	{
		class COMMON_INTERMEDIATE_REPRESENTATION_API VariableNode : public AbstractSyntaxTreeNode
		{
		public:
			VariableNode(const String& name, AbstractSyntaxTreeNode* pfather = nullptr);

			virtual void Accept(AbstractSyntaxTreeNodeVisitor& visitor) const override;

		private:
			String m_Name;
		};
	}
}

/*!
@}
*/