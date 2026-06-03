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
#include "AbstractSyntaxTree.h"
#include "Utility/Optional.hpp"

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
		inline constexpr const SizeType RequireExpression = 1004;
		inline constexpr const SizeType UnsatisfiedExpression = 1005;
		inline constexpr const SizeType InvalidExpression = 1006;
		inline constexpr const SizeType RequireMoreRepetition = 1007;
		inline constexpr const SizeType EnablePanicMode = 1008;
		inline constexpr const SizeType DisablePanicMode = 1009;
	}

	namespace Detail
	{
		template<Grammar::IsLanguage _Language, Grammar::IsExpression _Expression>
		struct ParseCore
		{
		};

		using ParseResult = Pair<SpaceGameEngine::Optional<AbstractSyntaxTree::AbstractSyntaxTreeNode>, Vector<ParserError>>;

		inline ParserError MakeUnexpectedEndError(const Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& begin_iter)
		{
			if (iter == begin_iter)
				return ParserError(ErrorTypeId::UnexpectedEnd, 1, 1);
			auto prev_iter = iter - 1;
			return ParserError(ErrorTypeId::UnexpectedEnd, prev_iter->GetLine(), prev_iter->GetColumn() + prev_iter->GetContent().GetSize());
		}

		inline ParserError MakeOtherParserError(const Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& begin_iter, SizeType error_type_id, Vector<String>&& additional_information)
		{
			SizeType line = 1, column = 1;
			if (iter != begin_iter)
			{
				line = iter->GetLine();
				column = iter->GetColumn();
			}
			return ParserError(error_type_id, line, column, std::move(additional_information));
		}

		template<Grammar::IsLanguage _Language, Lexer::TokenType _Type>
		struct ParseCore<_Language, Grammar::MatchTokenType<_Type>>
		{
			inline static ParseResult Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& begin_iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
			{
				if (iter == end_iter)
					return ParseResult(OptionalTag::EmptyOptional, Vector<ParserError>{MakeUnexpectedEndError(iter, begin_iter)});
				else if (iter->GetType() == _Type)
				{
					auto node = AbstractSyntaxTree::AbstractSyntaxTreeNode(Grammar::MatchTokenType<_Type>::Name.m_Value, iter, iter + 1, {});
					++iter;
					return ParseResult(std::move(node), Vector<ParserError>());
				}
				else
					return ParseResult(OptionalTag::EmptyOptional, Vector<ParserError>{MakeOtherParserError(iter, begin_iter, ErrorTypeId::UnexpectedTokenType, Vector<String>{ToString<String>(_Type), ToString<String>(iter->GetType())})});
			}
		};

		template<Grammar::IsLanguage _Language, Lexer::TokenType _Type, ArrayLiteral _Content>
		struct ParseCore<_Language, Grammar::MatchTokenTypeAndContent<_Type, _Content>>
		{
			inline static ParseResult Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& begin_iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
			{
				if (iter == end_iter)
					return ParseResult(OptionalTag::EmptyOptional, Vector<ParserError>{MakeUnexpectedEndError(iter, begin_iter)});
				else if (iter->GetType() == _Type && iter->GetContent() == _Content.m_Value)
				{
					auto node = AbstractSyntaxTree::AbstractSyntaxTreeNode(Grammar::MatchTokenTypeAndContent<_Type, _Content>::Name.m_Value, iter, iter + 1, {});
					++iter;
					return ParseResult(std::move(node), Vector<ParserError>());
				}
				else
					return ParseResult(OptionalTag::EmptyOptional, Vector<ParserError>{MakeOtherParserError(iter, begin_iter, ErrorTypeId::UnexpectedToken, Vector<String>{ToString<String>(_Type), String(_Content.m_Value), ToString<String>(iter->GetType()), iter->GetContent()})});
			}
		};

		template<Grammar::IsLanguage _Language, Grammar::IsExpression... _Expressions>
		struct ParseCore<_Language, Grammar::Sequence<_Expressions...>>
		{
		public:
			inline static ParseResult Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& begin_iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
			{
				auto start_iter = iter;
				bool can_continue = true;
				Vector<AbstractSyntaxTree::AbstractSyntaxTreeNode> children;
				Vector<ParserError> errors;
				(ParseChildren<_Expressions>(iter, begin_iter, end_iter, children, errors, can_continue), ...);
				if (children.GetSize() == sizeof...(_Expressions))
					return ParseResult(AbstractSyntaxTree::AbstractSyntaxTreeNode(Grammar::Sequence<_Expressions...>::Name.m_Value, start_iter, iter, std::move(children)), std::move(errors));
				else
				{
					iter = start_iter;
					return ParseResult(OptionalTag::EmptyOptional, std::move(errors));
				}
			}

		private:
			template<Grammar::IsExpression ChildExpression>
			inline static void ParseChildren(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& begin_iter, const Vector<Lexer::Token>::ConstIterator& end_iter, Vector<AbstractSyntaxTree::AbstractSyntaxTreeNode>& children, Vector<ParserError>& errors, bool& can_continue)
			{
				if (!can_continue)
					return;
				auto backup_iter = iter;
				auto result = ParseCore<_Language, ChildExpression>::Parse(iter, begin_iter, end_iter);
				if (result.m_First.HasValue())
				{
					children.EmplaceBack(std::move(result.m_First.Get()));
					if (result.m_Second.GetSize() > 0)
						errors.Insert(errors.GetConstEnd(), result.m_Second.GetConstBegin(), result.m_Second.GetConstEnd());	// keep error for panic mode
				}
				else
				{
					can_continue = false;
					errors.EmplaceBack(MakeOtherParserError(backup_iter, begin_iter, ErrorTypeId::RequireExpression, Vector<String>{ChildExpression::Name.m_Value}));
					errors.Insert(errors.GetConstEnd(), result.m_Second.GetConstBegin(), result.m_Second.GetConstEnd());
				}
			}
		};

		template<Grammar::IsLanguage _Language, Grammar::IsExpression... _Expressions>
		struct ParseCore<_Language, Grammar::Select<_Expressions...>>
		{
		public:
			inline static ParseResult Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& begin_iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
			{
				auto start_iter = iter;
				Vector<AbstractSyntaxTree::AbstractSyntaxTreeNode> children;
				Vector<ParserError> errors;
				SpaceGameEngine::Optional<ParseResult> panic_mode_result;
				SpaceGameEngine::Optional<AbstractSyntaxTree::AbstractSyntaxTreeNode> successful_node;
				(ParseChildren<_Expressions>(iter, begin_iter, end_iter, errors, successful_node, panic_mode_result), ...);
				if (successful_node.HasValue())
				{
					children.EmplaceBack(std::move(successful_node.Get()));
					return ParseResult(AbstractSyntaxTree::AbstractSyntaxTreeNode(Grammar::Select<_Expressions...>::Name.m_Value, start_iter, iter, std::move(children)), Vector<ParserError>());
				}
				else if (panic_mode_result.HasValue())
				{
					start_iter = panic_mode_result.Get().m_First.Get().GetBeginTokenIter();
					auto end_iter = panic_mode_result.Get().m_First.Get().GetEndTokenIter();
					children.EmplaceBack(std::move(panic_mode_result.Get().m_First.Get()));
					return ParseResult(AbstractSyntaxTree::AbstractSyntaxTreeNode(Grammar::Select<_Expressions...>::Name.m_Value, start_iter, end_iter, std::move(children)), std::move(panic_mode_result.Get().m_Second));
				}
				else
					return ParseResult(OptionalTag::EmptyOptional, std::move(errors));
			}

		private:
			template<Grammar::IsExpression ChildExpression>
			inline static void ParseChildren(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& begin_iter, const Vector<Lexer::Token>::ConstIterator& end_iter, Vector<ParserError>& errors, SpaceGameEngine::Optional<AbstractSyntaxTree::AbstractSyntaxTreeNode>& successful_node, SpaceGameEngine::Optional<ParseResult>& panic_mode_result)
			{
				if (successful_node.HasValue())
					return;
				auto backup_iter = iter;
				auto result = ParseCore<_Language, ChildExpression>::Parse(iter, begin_iter, end_iter);
				if (result.m_First.HasValue())
				{
					if (result.m_Second.GetSize() == 0)
					{
						successful_node = std::move(result.m_First.Get());
						return;
					}
					if (!panic_mode_result.HasValue())
						panic_mode_result = std::move(result);
					iter = backup_iter;	   // need to restore iter for panic mode result
				}
				else
				{
					errors.EmplaceBack(MakeOtherParserError(backup_iter, begin_iter, ErrorTypeId::UnsatisfiedExpression, Vector<String>{ChildExpression::Name.m_Value}));
					errors.Insert(errors.GetConstEnd(), result.m_Second.GetConstBegin(), result.m_Second.GetConstEnd());
				}
			}
		};

		template<Grammar::IsLanguage _Language, Grammar::IsExpression _Expression>
		struct ParseCore<_Language, Grammar::Negate<_Expression>>
		{
			inline static ParseResult Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& begin_iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
			{
				auto start_iter = iter;
				auto result = ParseCore<_Language, _Expression>::Parse(iter, begin_iter, end_iter);
				iter = start_iter;
				if (result.m_First.HasValue() && result.m_Second.GetSize() == 0)
					return ParseResult(OptionalTag::EmptyOptional, Vector<ParserError>{MakeOtherParserError(start_iter, begin_iter, ErrorTypeId::InvalidExpression, Vector<String>{_Expression::Name.m_Value})});
				else
					return ParseResult(AbstractSyntaxTree::AbstractSyntaxTreeNode(Grammar::Negate<_Expression>::Name.m_Value, start_iter, start_iter, {}), Vector<ParserError>());	  // negative expression should not include any error coming from parsing inside expression, so the error vector is empty.
			}
		};

		template<Grammar::IsLanguage _Language, Grammar::IsExpression _Expression>
		struct ParseCore<_Language, Grammar::Optional<_Expression>>
		{
			inline static ParseResult Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& begin_iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
			{
				auto start_iter = iter;
				auto result = ParseCore<_Language, _Expression>::Parse(iter, begin_iter, end_iter);
				if (result.m_First.HasValue() && result.m_Second.GetSize() == 0)	// if there is error, we treat it as empty optional.
				{
					Vector<AbstractSyntaxTree::AbstractSyntaxTreeNode> children;
					children.EmplaceBack(std::move(result.m_First.Get()));
					return ParseResult(AbstractSyntaxTree::AbstractSyntaxTreeNode(Grammar::Optional<_Expression>::Name.m_Value, start_iter, iter, std::move(children)), std::move(result.m_Second));
				}
				else
				{
					iter = start_iter;	  // restore iter for panic mode, if there is error, we treat it as empty optional, so the iter should be restored for the following parsing.
					return ParseResult(AbstractSyntaxTree::AbstractSyntaxTreeNode(Grammar::Optional<_Expression>::Name.m_Value, start_iter, start_iter, {}), Vector<ParserError>());
				}
			}
		};

		template<Grammar::IsLanguage _Language, Grammar::IsExpression _Expression, SizeType _MinCount, SizeType _MaxCount, bool _IsAggressive>
		struct ParseCore<_Language, Grammar::Repeat<_Expression, _MinCount, _MaxCount, _IsAggressive>>
		{
			inline static ParseResult Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& begin_iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
			{
				auto start_iter = iter;
				Vector<AbstractSyntaxTree::AbstractSyntaxTreeNode> children;
				Vector<ParserError> errors;
				while (children.GetSize() < _MaxCount)
				{
					auto backup_iter = iter;
					auto result = ParseCore<_Language, _Expression>::Parse(iter, begin_iter, end_iter);
					if constexpr (_IsAggressive)
					{
						if (result.m_First.HasValue())	  // success or enter panic mode but still get result with error, we treat it as a successful parsing, so we push the node into children and continue to parse next one.
						{
							children.PushBack(std::move(result.m_First.Get()));
							if (result.m_Second.GetSize() > 0)
								errors.Insert(errors.GetConstEnd(), result.m_Second.GetConstBegin(), result.m_Second.GetConstEnd());	// need to keep error for panic mode
						}
						else	// failure with no result
						{
							if (children.GetSize() < _MinCount)
							{
								errors.EmplaceBack(MakeOtherParserError(iter, begin_iter, ErrorTypeId::RequireMoreRepetition, Vector<String>{_Expression::Name.m_Value, ToString<String>(_MinCount), ToString<String>(children.GetSize())}));
								errors.Insert(errors.GetConstEnd(), result.m_Second.GetConstBegin(), result.m_Second.GetConstEnd());
							}
							break;
						}
					}
					else
					{
						if (result.m_Second.GetSize() > 0)
						{
							if (children.GetSize() >= _MinCount)	// can break now
							{
								iter = backup_iter;	   // restore iter for panic mode, if there is error, we treat it as the end of repetition, so the iter should be restored for the following parsing.
								break;
							}
							if (!result.m_First.HasValue())
							{
								errors.EmplaceBack(MakeOtherParserError(iter, begin_iter, ErrorTypeId::RequireMoreRepetition, Vector<String>{_Expression::Name.m_Value, ToString<String>(_MinCount), ToString<String>(children.GetSize())}));
								errors.Insert(errors.GetConstEnd(), result.m_Second.GetConstBegin(), result.m_Second.GetConstEnd());
								break;
							}
							else
								errors.Insert(errors.GetConstEnd(), result.m_Second.GetConstBegin(), result.m_Second.GetConstEnd());	// need to keep error for panic mode
						}
						// no error here or get result with error but still can continue, we treat it as a successful parsing, so we push the node into children and continue to parse next one.
						children.PushBack(std::move(result.m_First.Get()));
					}
				}
				if (children.GetSize() < _MinCount)
				{
					iter = start_iter;
					return ParseResult(OptionalTag::EmptyOptional, std::move(errors));
				}
				return ParseResult(AbstractSyntaxTree::AbstractSyntaxTreeNode(Grammar::Repeat<_Expression, _MinCount, _MaxCount, _IsAggressive>::Name.m_Value, start_iter, iter, std::move(children)), std::move(errors));
			}
		};

		template<Grammar::IsLanguage _Language, ArrayLiteral _Name>
		struct ParseCore<_Language, Grammar::RuleReference<_Name>>
		{
			using RuleType = typename _Language::template GetRule<_Name>;
			inline static constexpr const bool _IsSynchronousPoint = RuleType::IsSynchronousPoint;
			using _Expression = typename RuleType::Expression;

			inline static ParseResult Parse(Vector<Lexer::Token>::ConstIterator& iter, const Vector<Lexer::Token>::ConstIterator& begin_iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
			{
				auto result = ParseCore<_Language, _Expression>::Parse(iter, begin_iter, end_iter);
				if (result.m_First.HasValue())
				{
					auto start_iter = result.m_First.Get().GetBeginTokenIter();
					auto end_iter = result.m_First.Get().GetEndTokenIter();
					Vector<AbstractSyntaxTree::AbstractSyntaxTreeNode> children;
					children.EmplaceBack(std::move(result.m_First.Get()));
					return ParseResult(AbstractSyntaxTree::AbstractSyntaxTreeNode(_Name.m_Value, start_iter, end_iter, std::move(children)), std::move(result.m_Second));
				}
				else
				{
					if constexpr (_IsSynchronousPoint)
					{
						if (iter == end_iter)	 // if there is no token left, there is no need to enter panic mode, just return the error.
							return ParseResult(OptionalTag::EmptyOptional, std::move(result.m_Second));
						Vector<ParserError> errors(std::move(result.m_Second));
						errors.EmplaceBack(MakeOtherParserError(iter, begin_iter, ErrorTypeId::EnablePanicMode, Vector<String>{_Name.m_Value}));
						auto backup_iter = iter;
						while (iter != end_iter)
						{
							++iter;
							if (iter == end_iter)
							{
								errors.EmplaceBack(MakeUnexpectedEndError(iter, begin_iter));
								break;
							}
							auto recovery_result = ParseCore<_Language, _Expression>::Parse(iter, begin_iter, end_iter);
							if (recovery_result.m_Second.GetSize() > 0)
								errors.Insert(errors.GetConstEnd(), recovery_result.m_Second.GetConstBegin(), recovery_result.m_Second.GetConstEnd());
							if (recovery_result.m_First.HasValue())
							{
								auto start_iter = recovery_result.m_First.Get().GetBeginTokenIter();
								auto end_iter = recovery_result.m_First.Get().GetEndTokenIter();
								Vector<AbstractSyntaxTree::AbstractSyntaxTreeNode> children;
								children.EmplaceBack(std::move(recovery_result.m_First.Get()));
								errors.EmplaceBack(MakeOtherParserError(iter, begin_iter, ErrorTypeId::DisablePanicMode, Vector<String>{_Name.m_Value}));
								return ParseResult(AbstractSyntaxTree::AbstractSyntaxTreeNode(_Name.m_Value, start_iter, end_iter, std::move(children)), std::move(errors));
							}
						}
						iter = backup_iter;
						return ParseResult(OptionalTag::EmptyOptional, std::move(errors));
					}
					else
						return ParseResult(OptionalTag::EmptyOptional, std::move(result.m_Second));
				}
			}
		};
	}

	template<Grammar::IsLanguage _Language, ArrayLiteral RootRuleName>
	inline auto Parse(const Vector<Lexer::Token>::ConstIterator& begin_iter, const Vector<Lexer::Token>::ConstIterator& end_iter)
	{
		auto iter = begin_iter;
		return Detail::ParseCore<_Language, Grammar::RuleReference<RootRuleName>>::Parse(iter, begin_iter, end_iter);
	}
}

/*!
@}
*/