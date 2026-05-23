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
#include "Serializer/StringSerializer.h"
#include "Stream/StreamWriter.hpp"
#include "Utility/Format.hpp"

/*!
@ingroup CommonParser
@{
*/

namespace SpaceGameEngine::CommonParser::Parser::AbstractSyntaxTree
{
	namespace Detail
	{
		COMMON_PARSER_API void PrintLine(StreamWriter<StringSerializer<String>>& stream_writer, const String& str, SizeType indent = 0);

		template<Grammar::IsExpression _Expression>
		struct AbstractSyntaxTreePrinter
		{
			inline static void Print(StreamWriter<StringSerializer<String>>& stream_writer, const AbstractSyntaxTreeNode<_Expression>& node, SizeType indent = 0)
			{
				AbstractSyntaxTreePrinter<Grammar::UnderlyingExpressionType<_Expression>>::Print(stream_writer, node, indent);
			}
		};

		template<Lexer::TokenType _Type>
		struct AbstractSyntaxTreePrinter<Grammar::MatchTokenTypeExpression<_Type>>
		{
			inline static void Print(StreamWriter<StringSerializer<String>>& stream_writer, const AbstractSyntaxTreeNode<Grammar::MatchTokenTypeExpression<_Type>>& node, SizeType indent = 0)
			{
				auto& token = *(node.GetBeginTokenIter());
				PrintLine(stream_writer, Format(String(SGE_STR("MatchTokenType {} \"{}\"\t<line:{}, column:{}>")), _Type, token.GetContent(), token.GetLine(), token.GetColumn()), indent);
			}
		};

		template<Lexer::TokenType _Type, ArrayLiteral _Content>
		struct AbstractSyntaxTreePrinter<Grammar::MatchTokenTypeAndContentExpression<_Type, _Content>>
		{
			inline static void Print(StreamWriter<StringSerializer<String>>& stream_writer, const AbstractSyntaxTreeNode<Grammar::MatchTokenTypeAndContentExpression<_Type, _Content>>& node, SizeType indent = 0)
			{
				auto& token = *(node.GetBeginTokenIter());
				PrintLine(stream_writer, Format(String(SGE_STR("MatchTokenTypeAndContent {} \"{}\"\t<line:{}, column:{}>")), _Type, _Content.m_Value, token.GetLine(), token.GetColumn()), indent);
			}
		};

		template<Grammar::IsExpression... _Expressions>
		struct AbstractSyntaxTreePrinter<Grammar::SequenceExpression<_Expressions...>>
		{
		public:
			inline static void Print(StreamWriter<StringSerializer<String>>& stream_writer, const AbstractSyntaxTreeNode<Grammar::SequenceExpression<_Expressions...>>& node, SizeType indent = 0)
			{
				auto begin_iter = node.GetBeginTokenIter();
				PrintLine(stream_writer, Format(String(SGE_STR("Sequence\t<line:{}, column:{}>")), begin_iter->GetLine(), begin_iter->GetColumn()), indent);
				PrintChildren(stream_writer, node, indent, std::make_integer_sequence<SizeType, sizeof...(_Expressions)>());
			}

		private:
			template<SizeType... Indices>
			inline static void PrintChildren(StreamWriter<StringSerializer<String>>& stream_writer, const AbstractSyntaxTreeNode<Grammar::SequenceExpression<_Expressions...>>& node, SizeType indent, std::integer_sequence<SizeType, Indices...>)
			{
				((AbstractSyntaxTreePrinter<_Expressions>::Print(stream_writer, node.GetChildren().Get<Indices>(), indent + 1)), ...);
			}
		};

		template<Grammar::IsExpression... _Expressions>
		struct AbstractSyntaxTreePrinter<Grammar::SelectExpression<_Expressions...>>
		{
		public:
			inline static void Print(StreamWriter<StringSerializer<String>>& stream_writer, const AbstractSyntaxTreeNode<Grammar::SelectExpression<_Expressions...>>& node, SizeType indent = 0)
			{
				auto begin_iter = node.GetBeginTokenIter();
				PrintLine(stream_writer, Format(String(SGE_STR("Select\t<line:{}, column:{}>")), begin_iter->GetLine(), begin_iter->GetColumn()), indent);
				PrintChildren(stream_writer, node, indent, std::make_integer_sequence<SizeType, sizeof...(_Expressions)>());
			}

		private:
			template<SizeType... Indices>
			inline static void PrintChildren(StreamWriter<StringSerializer<String>>& stream_writer, const AbstractSyntaxTreeNode<Grammar::SelectExpression<_Expressions...>>& node, SizeType indent, std::integer_sequence<SizeType, Indices...>)
			{
				(PrintChild<Indices>(stream_writer, node, indent), ...);
			}

			template<SizeType Index>
			inline static void PrintChild(StreamWriter<StringSerializer<String>>& stream_writer, const AbstractSyntaxTreeNode<Grammar::SelectExpression<_Expressions...>>& node, SizeType indent)
			{
				auto pchild = node.GetChild().template Query<Index>();
				if (pchild)
					AbstractSyntaxTreePrinter<typename TypeList<_Expressions...>::template Get<Index>>::Print(stream_writer, *pchild, indent + 1);
			}
		};

		template<Grammar::IsExpression _Expression>
		struct AbstractSyntaxTreePrinter<Grammar::NegateExpression<_Expression>>
		{
			inline static void Print(StreamWriter<StringSerializer<String>>& stream_writer, const AbstractSyntaxTreeNode<Grammar::NegateExpression<_Expression>>& node, SizeType indent = 0)
			{
				PrintLine(stream_writer, SGE_STR("Negate"), indent);
			}
		};

		template<Grammar::IsExpression _Expression>
		struct AbstractSyntaxTreePrinter<Grammar::OptionalExpression<_Expression>>
		{
			inline static void Print(StreamWriter<StringSerializer<String>>& stream_writer, const AbstractSyntaxTreeNode<Grammar::OptionalExpression<_Expression>>& node, SizeType indent = 0)
			{
				auto child = node.GetChild();
				if (child.HasValue())
				{
					auto begin_iter = node.GetBeginTokenIter();
					PrintLine(stream_writer, Format(String(SGE_STR("Optional\t<line:{}, column:{}>")), begin_iter->GetLine(), begin_iter->GetColumn()), indent);
					AbstractSyntaxTreePrinter<_Expression>::Print(stream_writer, child.Get(), indent + 1);
				}
				else
					PrintLine(stream_writer, SGE_STR("Optional"), indent);
			}
		};

		template<Grammar::IsExpression _Expression, SizeType _MinCount, SizeType _MaxCount>
		struct AbstractSyntaxTreePrinter<Grammar::RepeatExpression<_Expression, _MinCount, _MaxCount>>
		{
			inline static void Print(StreamWriter<StringSerializer<String>>& stream_writer, const AbstractSyntaxTreeNode<Grammar::RepeatExpression<_Expression, _MinCount, _MaxCount>>& node, SizeType indent = 0)
			{
				auto children = node.GetChildren();
				if (children.GetSize())
				{
					auto begin_iter = node.GetBeginTokenIter();
					PrintLine(stream_writer, Format(String(SGE_STR("Repeat[{}]\t<line:{}, column:{}>")), children.GetSize(), begin_iter->GetLine(), begin_iter->GetColumn()), indent);
					for (auto iter = children.GetConstBegin(); iter != children.GetConstEnd(); ++iter)
						AbstractSyntaxTreePrinter<_Expression>::Print(stream_writer, *iter, indent + 1);
				}
				else
					PrintLine(stream_writer, SGE_STR("Repeat"), indent);
			}
		};

		template<ArrayLiteral _Name, Grammar::IsExpression _Expression, bool _IsSynchronousPoint>
		struct AbstractSyntaxTreePrinter<Grammar::RuleExpression<_Name, _Expression, _IsSynchronousPoint>>
		{
			inline static void Print(StreamWriter<StringSerializer<String>>& stream_writer, const AbstractSyntaxTreeNode<Grammar::RuleExpression<_Name, _Expression, _IsSynchronousPoint>>& node, SizeType indent = 0)
			{
				auto begin_iter = node.GetBeginTokenIter();
				PrintLine(stream_writer, Format(String(SGE_STR("Rule {}\t<line:{}, column:{}>")), _Name.m_Value, begin_iter->GetLine(), begin_iter->GetColumn()), indent);
				AbstractSyntaxTreePrinter<_Expression>::Print(stream_writer, static_cast<const AbstractSyntaxTreeNode<_Expression>&>(node), indent + 1);
			}
		};
	}

	template<Grammar::IsExpression _Expression>
	inline void PrintAbstractSyntaxTree(OutputStream& stream, const AbstractSyntaxTreeNode<_Expression>& node)
	{
		StreamWriter<StringSerializer<String>> stream_writer(stream);
		Detail::AbstractSyntaxTreePrinter<_Expression>::Print(stream_writer, node);
	}
}

/*!
@}
*/