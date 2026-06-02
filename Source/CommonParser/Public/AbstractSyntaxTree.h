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
#include "SGEString.hpp"
#include "Lexer.h"
#include "Utility/HeapObject.hpp"

/*!
@ingroup CommonParser
@{
*/

namespace SpaceGameEngine::CommonParser::Parser::AbstractSyntaxTree
{
	class COMMON_PARSER_API AbstractSyntaxTreeNode
	{
	public:
		AbstractSyntaxTreeNode(const Char* pname, const Vector<Lexer::Token>::ConstIterator& begin_token_iter, const Vector<Lexer::Token>::ConstIterator& end_token_iter, Vector<AbstractSyntaxTreeNode>&& children);

		const Char* GetName() const;

		const Vector<Lexer::Token>::ConstIterator& GetBeginTokenIter() const;

		const Vector<Lexer::Token>::ConstIterator& GetEndTokenIter() const;

		const Vector<AbstractSyntaxTreeNode>& GetChildren() const;

	private:
		const Char* m_pName;
		Vector<Lexer::Token>::ConstIterator m_BeginTokenIter, m_EndTokenIter;
		HeapObject<Vector<AbstractSyntaxTreeNode>> m_Children;
	};

	template<typename Callable>
	inline void Visit(const Char* prule_name, const AbstractSyntaxTreeNode& node, Callable&& callable)
	{
		if (prule_name == nullptr || IsSameCString(prule_name, node.GetName()))
			callable(node);
		for (auto iter = node.GetChildren().GetConstBegin(); iter != node.GetChildren().GetConstEnd(); ++iter)
			Visit(prule_name, *iter, callable);
	}

	template<typename Callable1, typename Callable2>
	inline void Visit(Callable1&& predicate, Callable2&& action, const AbstractSyntaxTreeNode& node)
	{
		if (predicate(node))
			action(node);
		for (auto iter = node.GetChildren().GetConstBegin(); iter != node.GetChildren().GetConstEnd(); ++iter)
			Visit(predicate, action, *iter);
	}

}

/*!
@}
*/