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
#include "Grammar.hpp"
#include "Utility/Tuple.hpp"
#include "Utility/Variant.hpp"
#include "Utility/Optional.hpp"

/*!
@ingroup CommonParser
@{
*/

namespace SpaceGameEngine::CommonParser::Parser::AbstractSyntaxTree
{
	namespace Detail
	{
		template<Grammar::IsExpression _Expression>
		class AbstractSyntaxTreeNodeExpressionSpecifiedData
		{
		};
	}

	template<Grammar::IsExpression _Expression>
	class AbstractSyntaxTreeNode : public Detail::AbstractSyntaxTreeNodeExpressionSpecifiedData<Grammar::UnderlyingExpressionType<_Expression>>
	{
	public:
		template<typename... Args>
		inline AbstractSyntaxTreeNode(const Vector<Lexer::Token>::ConstIterator& begin_token_iter, const Vector<Lexer::Token>::ConstIterator& end_token_iter, Args&&... args)
			: m_BeginTokenIter(begin_token_iter), m_EndTokenIter(end_token_iter), Detail::AbstractSyntaxTreeNodeExpressionSpecifiedData<Grammar::UnderlyingExpressionType<_Expression>>(std::forward<Args>(args)...)
		{
		}

		inline const Vector<Lexer::Token>::ConstIterator& GetBeginTokenIter() const
		{
			return m_BeginTokenIter;
		}

		inline const Vector<Lexer::Token>::ConstIterator& GetEndTokenIter() const
		{
			return m_EndTokenIter;
		}

	private:
		Vector<Lexer::Token>::ConstIterator m_BeginTokenIter, m_EndTokenIter;
	};

	namespace Detail
	{
		template<Grammar::IsExpression... _Expressions>
		class AbstractSyntaxTreeNodeExpressionSpecifiedData<Grammar::SequenceExpression<_Expressions...>>
		{
		public:
			inline AbstractSyntaxTreeNodeExpressionSpecifiedData(Tuple<AbstractSyntaxTreeNode<_Expressions>...>&& children)
				: m_Children(std::move(children))
			{
			}

			inline const Tuple<AbstractSyntaxTreeNode<_Expressions>...>& GetChildren() const
			{
				return m_Children;
			}

		private:
			Tuple<AbstractSyntaxTreeNode<_Expressions>...> m_Children;
		};

		template<Grammar::IsExpression... _Expressions>
		class AbstractSyntaxTreeNodeExpressionSpecifiedData<Grammar::SelectExpression<_Expressions...>>
		{
		public:
			inline AbstractSyntaxTreeNodeExpressionSpecifiedData(Variant<AbstractSyntaxTreeNode<_Expressions>...>&& child)
				: m_Child(std::move(child))
			{
			}

			inline const Variant<AbstractSyntaxTreeNode<_Expressions>...>& GetChild() const
			{
				return m_Child;
			}

		private:
			Variant<AbstractSyntaxTreeNode<_Expressions>...> m_Child;
		};

		template<Grammar::IsExpression _Expression>
		class AbstractSyntaxTreeNodeExpressionSpecifiedData<Grammar::OptionalExpression<_Expression>>
		{
		public:
			inline AbstractSyntaxTreeNodeExpressionSpecifiedData(Optional<AbstractSyntaxTreeNode<_Expression>>&& child)
				: m_Child(std::move(child))
			{
			}

			inline const Optional<AbstractSyntaxTreeNode<_Expression>>& GetChild() const
			{
				return m_Child;
			}

		private:
			Optional<AbstractSyntaxTreeNode<_Expression>> m_Child;
		};

		template<Grammar::IsExpression _Expression, SizeType _MinCount, SizeType _MaxCount>
		class AbstractSyntaxTreeNodeExpressionSpecifiedData<Grammar::RepeatExpression<_Expression, _MinCount, _MaxCount>>
		{
		public:
			inline AbstractSyntaxTreeNodeExpressionSpecifiedData(Vector<AbstractSyntaxTreeNode<_Expression>>&& children)
				: m_Children(std::move(children))
			{
			}

			inline const Vector<AbstractSyntaxTreeNode<_Expression>>& GetChildren() const
			{
				return m_Children;
			}

		private:
			Vector<AbstractSyntaxTreeNode<_Expression>> m_Children;
		};

		template<ArrayLiteral _Name, Grammar::IsExpression _Expression, bool _IsSynchronousPoint>
		class AbstractSyntaxTreeNodeExpressionSpecifiedData<Grammar::RuleExpression<_Name, _Expression, _IsSynchronousPoint>> : public AbstractSyntaxTreeNodeExpressionSpecifiedData<Grammar::UnderlyingExpressionType<_Expression>>
		{
		public:
			template<typename... Args>
			inline AbstractSyntaxTreeNodeExpressionSpecifiedData(Args&&... args)
				: AbstractSyntaxTreeNodeExpressionSpecifiedData<Grammar::UnderlyingExpressionType<_Expression>>(std::forward<Args>(args)...)
			{
			}
		};
	}
}

/*!
@}
*/