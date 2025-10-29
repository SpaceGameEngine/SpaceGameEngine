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
#include "Utility/Optional.hpp"

/*!
@ingroup CommonIntermediateRepresentation
@{
*/

namespace SpaceGameEngine::CommonIntermediateRepresentation::Assembler
{
	class AbstractSyntaxTreeNodeVisitor;

	class COMMON_INTERMEDIATE_REPRESENTATION_API AbstractSyntaxTreeNode
	{
	public:
		AbstractSyntaxTreeNode() = default;
		AbstractSyntaxTreeNode(SizeType line, SizeType column, AbstractSyntaxTreeNode* pfather);
		virtual ~AbstractSyntaxTreeNode() = default;

		virtual void Accept(AbstractSyntaxTreeNodeVisitor& visitor) const = 0;

		SizeType GetLine() const;
		SizeType GetColumn() const;
		AbstractSyntaxTreeNode* GetFather() const;

	protected:
		void SetFather(AbstractSyntaxTreeNode& father);
		/*!
		@brief Link the children nodes' father pointer to this node. This function should be called when the nodes which have children just been constructed or been moved.
		*/
		virtual void Link();

	private:
		SizeType m_Line = 0;
		SizeType m_Column = 0;
		AbstractSyntaxTreeNode* m_pFather = nullptr;
	};

	namespace AbstractSyntaxTreeNodes
	{
		class COMMON_INTERMEDIATE_REPRESENTATION_API SymbolNode : public AbstractSyntaxTreeNode
		{
		public:
			friend class VariableDefinitionNode;
			friend class ParameterDefinitionNode;

			SymbolNode(const String& dialect_name, const String& name, SizeType line, SizeType column, AbstractSyntaxTreeNode* pfather = nullptr);

			virtual void Accept(AbstractSyntaxTreeNodeVisitor& visitor) const override;

			const String& GetDialectName() const;
			const String& GetName() const;

		private:
			String m_DialectName;
			String m_Name;
		};

		class COMMON_INTERMEDIATE_REPRESENTATION_API VariableNode : public AbstractSyntaxTreeNode
		{
		public:
			VariableNode(const String& name, SizeType line, SizeType column, AbstractSyntaxTreeNode* pfather = nullptr);

			virtual void Accept(AbstractSyntaxTreeNodeVisitor& visitor) const override;

			const String& GetName() const;

		private:
			String m_Name;
		};

		class COMMON_INTERMEDIATE_REPRESENTATION_API VariableDefinitionNode : public AbstractSyntaxTreeNode
		{
		public:
			VariableDefinitionNode(const String& name, Optional<SymbolNode>&& type_symbol, SizeType line, SizeType column, AbstractSyntaxTreeNode* pfather = nullptr);

			virtual void Accept(AbstractSyntaxTreeNodeVisitor& visitor) const override;

			const String& GetName() const;
			const Optional<SymbolNode>& GetTypeSymbolNode() const;

		protected:
			virtual void Link() override;

		private:
			String m_Name;
			Optional<SymbolNode> m_TypeSymbol;
		};

		class COMMON_INTERMEDIATE_REPRESENTATION_API ParameterDefinitionNode : public AbstractSyntaxTreeNode
		{
		public:
			friend class ParameterDefinitionsNode;

			ParameterDefinitionNode(const String& name, SymbolNode&& type_symbol, SizeType line, SizeType column, AbstractSyntaxTreeNode* pfather = nullptr);

			virtual void Accept(AbstractSyntaxTreeNodeVisitor& visitor) const override;

			const String& GetName() const;
			const SymbolNode& GetTypeSymbolNode() const;

		protected:
			virtual void Link() override;

		private:
			String m_Name;
			SymbolNode m_TypeSymbol;
		};

		class COMMON_INTERMEDIATE_REPRESENTATION_API ParameterDefinitionsNode : public AbstractSyntaxTreeNode
		{
		public:
			ParameterDefinitionsNode(Vector<ParameterDefinitionNode>&& parameter_definitions, SizeType line, SizeType column, AbstractSyntaxTreeNode* pfather = nullptr);

			virtual void Accept(AbstractSyntaxTreeNodeVisitor& visitor) const override;

			const Vector<ParameterDefinitionNode>& GetParameterDefinitions() const;

		protected:
			virtual void Link() override;

		private:
			Vector<ParameterDefinitionNode> m_ParameterDefinitions;
		};

		/*class AttributeValueNode;

		class COMMON_INTERMEDIATE_REPRESENTATION_API AttributeDefinitionNode : public AbstractSyntaxTreeNode
		{
		public:
			AttributeDefinitionNode(const String& name, AttributeValueNode&& value, SizeType line, SizeType column, AbstractSyntaxTreeNode* pfather = nullptr);

			virtual void Accept(AbstractSyntaxTreeNodeVisitor& visitor) const override;

			const String& GetName() const;
			const AttributeValueNode& GetValue() const;

		protected:
			virtual void Link() override;

		private:
			String m_Name;
			AttributeValueNode m_Value;
		};*/
	}

	class COMMON_INTERMEDIATE_REPRESENTATION_API AbstractSyntaxTreeNodeVisitor
	{
	public:
		virtual ~AbstractSyntaxTreeNodeVisitor() = default;

		virtual void Visit(const AbstractSyntaxTreeNodes::SymbolNode& node) = 0;
		virtual void Visit(const AbstractSyntaxTreeNodes::VariableNode& node) = 0;
		virtual void Visit(const AbstractSyntaxTreeNodes::VariableDefinitionNode& node) = 0;
		virtual void Visit(const AbstractSyntaxTreeNodes::ParameterDefinitionNode& node) = 0;
		virtual void Visit(const AbstractSyntaxTreeNodes::ParameterDefinitionsNode& node) = 0;
		// todo more node types as needed
	};
}

/*!
@}
*/