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
#include "AbstractSyntaxTree.hpp"

/*!
@ingroup CommonParser
@{
*/

namespace SpaceGameEngine::CommonParser::Parser::TopDownParser
{
	namespace ErrorTypeId
	{
		inline constexpr const SizeType UnexpectedEnd = 1001;
		inline constexpr const SizeType UnexpectedTokenType = 1002;
		inline constexpr const SizeType UnexpectedToken = 1003;
		inline constexpr const SizeType InvalidExpression = 1004;
		inline constexpr const SizeType RequireMoreRepetition = 1005;
	}

	namespace Detail
	{
		template<Grammar::IsExpression _Expression>
		struct ParseCore
		{
		};

		template<Grammar::IsExpression _Expression>
		using ParseResult = Pair<Optional<AbstractSyntaxTree::AbstractSyntaxTreeNode<_Expression>>, Vector<ParserError>>;

		template<Lexer::TokenType _Type>
		struct ParseCore<Grammar::MatchTokenTypeExpression<_Type>>
		{
			inline static ParseResult<Grammar::MatchTokenTypeExpression<_Type>> Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
			{
				if (iter == end_iter)
				{
					auto prev_iter = iter - 1;
					return ParseResult<Grammar::MatchTokenTypeExpression<_Type>>(OptionalTag::EmptyOptional, Vector<ParserError>{ParserError(ErrorTypeId::UnexpectedEnd, prev_iter->GetLine(), prev_iter->GetColumn())});
				}
				else if (iter->GetType() == _Type)
				{
					auto node = AbstractSyntaxTree::AbstractSyntaxTreeNode<Grammar::MatchTokenTypeExpression<_Type>>(iter, iter + 1);
					++iter;
					return ParseResult<Grammar::MatchTokenTypeExpression<_Type>>(std::move(node), Vector<ParserError>());
				}
				else
					return ParseResult<Grammar::MatchTokenTypeExpression<_Type>>(OptionalTag::EmptyOptional, Vector<ParserError>{ParserError(ErrorTypeId::UnexpectedTokenType, iter->GetLine(), iter->GetColumn(), Vector<String>{ToString<String>(_Type), ToString<String>(iter->GetType())})});
			}
		};

		template<Lexer::TokenType _Type, ArrayLiteral _Content>
		struct ParseCore<Grammar::MatchTokenTypeAndContentExpression<_Type, _Content>>
		{
			inline static ParseResult<Grammar::MatchTokenTypeAndContentExpression<_Type, _Content>> Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
			{
				if (iter == end_iter)
				{
					auto prev_iter = iter - 1;
					return ParseResult<Grammar::MatchTokenTypeAndContentExpression<_Type, _Content>>(OptionalTag::EmptyOptional, Vector<ParserError>{ParserError(ErrorTypeId::UnexpectedEnd, prev_iter->GetLine(), prev_iter->GetColumn())});
				}
				else if (iter->GetType() == _Type && iter->GetContent() == _Content.m_Value)
				{
					auto node = AbstractSyntaxTree::AbstractSyntaxTreeNode<Grammar::MatchTokenTypeAndContentExpression<_Type, _Content>>(iter, iter + 1);
					++iter;
					return ParseResult<Grammar::MatchTokenTypeAndContentExpression<_Type, _Content>>(std::move(node), Vector<ParserError>());
				}
				else
					return ParseResult<Grammar::MatchTokenTypeAndContentExpression<_Type, _Content>>(OptionalTag::EmptyOptional, Vector<ParserError>{ParserError(ErrorTypeId::UnexpectedToken, iter->GetLine(), iter->GetColumn(), Vector<String>{ToString<String>(_Type), String(_Content.m_Value), ToString<String>(iter->GetType()), iter->GetContent()})});
			}
		};

		template<Grammar::IsExpression... _Expressions>
		struct ParseCore<Grammar::SequenceExpression<_Expressions...>>
		{
		public:
			inline static ParseResult<Grammar::SequenceExpression<_Expressions...>> Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
			{
				auto backup_iter = iter;
				auto result = ParseImplement<_Expressions...>::Parse(iter, end_iter);
				if (result.m_First.HasValue())
				{
					auto node = AbstractSyntaxTree::AbstractSyntaxTreeNode<Grammar::SequenceExpression<_Expressions...>>(backup_iter, iter, std::move(result.m_First.Get()));
					return ParseResult<Grammar::SequenceExpression<_Expressions...>>(std::move(node), std::move(result.m_Second));
				}
				else
				{
					iter = backup_iter;
					return ParseResult<Grammar::SequenceExpression<_Expressions...>>(OptionalTag::EmptyOptional, std::move(result.m_Second));
				}
			}

		private:
			using Expressions = typename Grammar::SequenceExpression<_Expressions...>::Expressions;

			template<Grammar::IsExpression... __Expressions>
			struct ParseImplement
			{
			};

			template<Grammar::IsExpression... __Expressions>
			using ParseImplementResult = Pair<Optional<Tuple<AbstractSyntaxTree::AbstractSyntaxTreeNode<__Expressions>...>>, Vector<ParserError>>;

			template<Grammar::IsExpression Expression1>
			struct ParseImplement<Expression1>
			{
				inline static ParseImplementResult<Expression1> Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
				{
					auto result = ParseCore<Expression1>::Parse(iter, end_iter);
					if (!result.m_First.HasValue())
						return ParseImplementResult<Expression1>(OptionalTag::EmptyOptional, std::move(result.m_Second));
					return ParseImplementResult<Expression1>(MakeTuple(result.m_First.Get()), std::move(result.m_Second));
				}
			};

			template<Grammar::IsExpression Expression1, Grammar::IsExpression... __Expressions>
			struct ParseImplement<Expression1, __Expressions...>
			{
				inline static ParseImplementResult<Expression1, __Expressions...> Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
				{
					auto result1 = ParseCore<Expression1>::Parse(iter, end_iter);
					if (!result1.m_First.HasValue())
						return ParseImplementResult<Expression1, __Expressions...>(OptionalTag::EmptyOptional, std::move(result1.m_Second));
					auto result2 = ParseImplement<__Expressions...>::Parse(iter, end_iter);
					if (!result2.m_First.HasValue())
						return ParseImplementResult<Expression1, __Expressions...>(OptionalTag::EmptyOptional, std::move(result2.m_Second));
					Vector<ParserError> errors(std::move(result1.m_Second));
					if (result2.m_Second.GetSize() > 0)
						errors.Insert(errors.GetConstEnd(), result2.m_Second.GetConstBegin(), result2.m_Second.GetConstEnd());
					return ParseImplementResult<Expression1, __Expressions...>(ConcatTuples(MakeTuple(result1.m_First.Get()), result2.m_First.Get()), std::move(errors));
				}
			};
		};

		template<Grammar::IsExpression... _Expressions>
		struct ParseCore<Grammar::SelectExpression<_Expressions...>>
		{
		public:
			inline static ParseResult<Grammar::SelectExpression<_Expressions...>> Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
			{
				auto backup_iter = iter;
				auto result = ParseImplement<0, _Expressions...>::Parse(iter, end_iter);
				if (result.m_First.HasValue())
				{
					auto node = AbstractSyntaxTree::AbstractSyntaxTreeNode<Grammar::SelectExpression<_Expressions...>>(backup_iter, iter, std::move(result.m_First.Get()));
					return ParseResult<Grammar::SelectExpression<_Expressions...>>(std::move(node), std::move(result.m_Second));
				}
				else
				{
					iter = backup_iter;
					return ParseResult<Grammar::SelectExpression<_Expressions...>>(OptionalTag::EmptyOptional, std::move(result.m_Second));
				}
			}

		private:
			template<SizeType Index, Grammar::IsExpression... __Expressions>
			struct ParseImplement
			{
			};

			using ParseImplementResult = Pair<Optional<Variant<AbstractSyntaxTree::AbstractSyntaxTreeNode<_Expressions>...>>, Vector<ParserError>>;

			template<SizeType Index, Grammar::IsExpression Expression1>
			struct ParseImplement<Index, Expression1>
			{
				inline static ParseImplementResult Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
				{
					auto result = ParseCore<Expression1>::Parse(iter, end_iter);
					if (!result.m_First.HasValue())
						return ParseImplementResult(OptionalTag::EmptyOptional, std::move(result.m_Second));
					return ParseImplementResult(Variant<AbstractSyntaxTree::AbstractSyntaxTreeNode<_Expressions>...>(ValueWrapper<Index>(), std::move(result.m_First.Get())), std::move(result.m_Second));
				}
			};

			template<SizeType Index, Grammar::IsExpression Expression1, Grammar::IsExpression... __Expressions>
			struct ParseImplement<Index, Expression1, __Expressions...>
			{
				inline static ParseImplementResult Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
				{
					auto result1 = ParseImplement<Index, Expression1>::Parse(iter, end_iter);
					if (result1.m_First.HasValue())
						return ParseImplementResult(Variant<AbstractSyntaxTree::AbstractSyntaxTreeNode<_Expressions>...>(std::move(result1.m_First.Get())), std::move(result1.m_Second));
					auto result2 = ParseImplement<Index + 1, __Expressions...>::Parse(iter, end_iter);
					if (result2.m_First.HasValue())
						return ParseImplementResult(Variant<AbstractSyntaxTree::AbstractSyntaxTreeNode<_Expressions>...>(std::move(result2.m_First.Get())), std::move(result2.m_Second));
					Vector<ParserError> errors(std::move(result1.m_Second));
					if (result2.m_Second.GetSize() > 0)
						errors.Insert(errors.GetConstEnd(), result2.m_Second.GetConstBegin(), result2.m_Second.GetConstEnd());
					return ParseImplementResult(OptionalTag::EmptyOptional, std::move(errors));
				}
			};
		};

		template<Grammar::IsExpression _Expression>
		struct ParseCore<Grammar::NegateExpression<_Expression>>
		{
			inline static ParseResult<Grammar::NegateExpression<_Expression>> Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
			{
				auto backup_iter = iter;
				auto result = ParseCore<_Expression>::Parse(iter, end_iter);
				if (result.m_First.HasValue())
				{
					iter = backup_iter;
					return ParseResult<Grammar::NegateExpression<_Expression>>(OptionalTag::EmptyOptional, Vector<ParserError>{ParserError(ErrorTypeId::InvalidExpression, backup_iter->GetLine(), backup_iter->GetColumn())});
				}
				else
				{
					iter = backup_iter;
					auto node = AbstractSyntaxTree::AbstractSyntaxTreeNode<Grammar::NegateExpression<_Expression>>(backup_iter, backup_iter);
					return ParseResult<Grammar::NegateExpression<_Expression>>(std::move(node), std::move(result.m_Second));
				}
			}
		};

		template<Grammar::IsExpression _Expression>
		struct ParseCore<Grammar::OptionalExpression<_Expression>>
		{
			inline static ParseResult<Grammar::OptionalExpression<_Expression>> Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
			{
				auto backup_iter = iter;
				auto result = ParseCore<_Expression>::Parse(iter, end_iter);
				if (result.m_First.HasValue())
				{
					auto node = AbstractSyntaxTree::AbstractSyntaxTreeNode<Grammar::OptionalExpression<_Expression>>(backup_iter, iter, std::move(result.m_First));
					return ParseResult<Grammar::OptionalExpression<_Expression>>(std::move(node), std::move(result.m_Second));
				}
				else
				{
					iter = backup_iter;
					auto node = AbstractSyntaxTree::AbstractSyntaxTreeNode<Grammar::OptionalExpression<_Expression>>(backup_iter, backup_iter, OptionalTag::EmptyOptional);
					return ParseResult<Grammar::OptionalExpression<_Expression>>(std::move(node), std::move(result.m_Second));
				}
			}
		};

		template<Grammar::IsExpression _Expression, SizeType _MinCount, SizeType _MaxCount>
		struct ParseCore<Grammar::RepeatExpression<_Expression, _MinCount, _MaxCount>>
		{
			inline static ParseResult<Grammar::RepeatExpression<_Expression, _MinCount, _MaxCount>> Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
			{
				auto backup_iter = iter;
				Vector<AbstractSyntaxTree::AbstractSyntaxTreeNode<_Expression>> nodes;
				Vector<ParserError> errors;
				while (nodes.GetSize() < _MaxCount)
				{
					auto result = ParseCore<_Expression>::Parse(iter, end_iter);
					if (result.m_First.HasValue())
						nodes.PushBack(std::move(result.m_First.Get()));
					else
					{
						if (result.m_Second.GetSize() > 0)
							errors.Insert(errors.GetConstEnd(), result.m_Second.GetConstBegin(), result.m_Second.GetConstEnd());
						break;
					}
				}
				if (nodes.GetSize() < _MinCount)
				{
					errors.EmplaceBack(ErrorTypeId::RequireMoreRepetition, iter->GetLine(), iter->GetColumn());
					iter = backup_iter;
					return ParseResult<Grammar::RepeatExpression<_Expression, _MinCount, _MaxCount>>(OptionalTag::EmptyOptional, std::move(errors));
				}
				else
				{
					auto node = AbstractSyntaxTree::AbstractSyntaxTreeNode<Grammar::RepeatExpression<_Expression, _MinCount, _MaxCount>>(backup_iter, iter, std::move(nodes));
					return ParseResult<Grammar::RepeatExpression<_Expression, _MinCount, _MaxCount>>(std::move(node), std::move(errors));
				}
			}
		};

		template<ArrayLiteral _Name, Grammar::IsExpression _Expression, bool _IsSynchronousPoint>
		struct ParseCore<Grammar::RuleExpression<_Name, _Expression, _IsSynchronousPoint>>
		{
			inline static ParseResult<Grammar::RuleExpression<_Name, _Expression, _IsSynchronousPoint>> Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
			{
				auto backup_iter = iter;
				auto result = ParseCore<_Expression>::Parse(iter, end_iter);
				if (result.m_First.HasValue())
				{
					auto node = AbstractSyntaxTree::AbstractSyntaxTreeNode<Grammar::RuleExpression<_Name, _Expression, _IsSynchronousPoint>>(backup_iter, iter, std::move(result.m_First.Get()));
					return ParseResult<Grammar::RuleExpression<_Name, _Expression, _IsSynchronousPoint>>(std::move(node), std::move(result.m_Second));
				}
				else
				{
					iter = backup_iter;
					return ParseResult<Grammar::RuleExpression<_Name, _Expression, _IsSynchronousPoint>>(OptionalTag::EmptyOptional, std::move(result.m_Second));
				}
			}
		};
	}

	template<Grammar::IsExpression _Expression>
	inline auto Parse(const Vector<Lexer::Token>::ConstIterator& begin_iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
	{
		auto iter = begin_iter;
		return Detail::ParseCore<Grammar::UnderlyingExpressionType<_Expression>>::Parse(iter, end_iter);
	}
}

/*!
@}
*/