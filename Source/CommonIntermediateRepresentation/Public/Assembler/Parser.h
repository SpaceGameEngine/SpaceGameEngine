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
#include "Lexer.h"
#include "TopDownParser.hpp"

/*!
@ingroup CommonIntermediateRepresentation
@{
*/

namespace SpaceGameEngine::CommonIntermediateRepresentation::Assembler
{
	namespace Grammar
	{
		using namespace SpaceGameEngine::CommonParser::Parser::Grammar;

		using Symbol = Rule<SGE_STR("Symbol"), MatchTokenType<Assembler::TokenTypes::SymbolIdentifier>>;

		using VariableIdentifier = Rule<SGE_STR("VariableIdentifier"),
										Sequence<
											MatchTokenType<CommonParser::Lexer::TokenTypes::Mod>,
											MatchTokenType<CommonParser::Lexer::TokenTypes::Identifier>>>;

		using VariableDefinition = Rule<SGE_STR("VariableDefinition"),
										Sequence<
											RuleReference<SGE_STR("VariableIdentifier")>,
											Grammar::Optional<
												Sequence<
													MatchTokenType<CommonParser::Lexer::TokenTypes::Colon>,
													RuleReference<SGE_STR("Symbol")>>>>>;

		using ParameterDefinition = Rule<SGE_STR("ParameterDefinition"),
										 Sequence<
											 RuleReference<SGE_STR("VariableIdentifier")>,
											 Sequence<
												 MatchTokenType<CommonParser::Lexer::TokenTypes::Colon>,
												 RuleReference<SGE_STR("Symbol")>>>>;

		using ParameterDefinitions = Rule<SGE_STR("ParameterDefinitions"),
										  Sequence<
											  MatchTokenType<CommonParser::Lexer::TokenTypes::LeftBracket>,
											  Grammar::Optional<
												  Sequence<
													  RuleReference<SGE_STR("ParameterDefinition")>,
													  Repeat<
														  Sequence<
															  MatchTokenType<CommonParser::Lexer::TokenTypes::Comma>,
															  RuleReference<SGE_STR("ParameterDefinition")>>>>>,
											  MatchTokenType<CommonParser::Lexer::TokenTypes::RightBracket>>>;

		using Block = Rule<SGE_STR("Block"),
						   Sequence<
							   MatchTokenType<CommonParser::Lexer::TokenTypes::LeftCurlyBracket>,
							   Repeat<
								   RuleReference<SGE_STR("Statement")>>,
							   MatchTokenType<CommonParser::Lexer::TokenTypes::RightCurlyBracket>>>;

		using Argument = Rule<SGE_STR("Argument"),
							  Select<
								  RuleReference<SGE_STR("VariableIdentifier")>,
								  RuleReference<SGE_STR("ParameterDefinitions")>,
								  RuleReference<SGE_STR("Block")>,
								  MatchTokenType<CommonParser::Lexer::TokenTypes::IntegerLiteral>,
								  MatchTokenType<CommonParser::Lexer::TokenTypes::FloatLiteral>,
								  MatchTokenType<CommonParser::Lexer::TokenTypes::DoubleLiteral>,
								  MatchTokenType<CommonParser::Lexer::TokenTypes::StringLiteral>,
								  MatchTokenType<Assembler::TokenTypes::BooleanLiteral>>>;

		using Statement = Rule<SGE_STR("Statement"),
							   Sequence<
								   RuleReference<SGE_STR("VariableDefinition")>,
								   MatchTokenType<CommonParser::Lexer::TokenTypes::Equal>,
								   Grammar::Optional<RuleReference<SGE_STR("AttributeDictionary")>>,
								   RuleReference<SGE_STR("Symbol")>,
								   Repeat<RuleReference<SGE_STR("Argument")>>,
								   MatchTokenType<CommonParser::Lexer::TokenTypes::Semicolon>>,
							   true>;

		using AttributeValue = Rule<SGE_STR("AttributeValue"),
									Select<
										MatchTokenType<CommonParser::Lexer::TokenTypes::IntegerLiteral>,
										MatchTokenType<CommonParser::Lexer::TokenTypes::FloatLiteral>,
										MatchTokenType<CommonParser::Lexer::TokenTypes::DoubleLiteral>,
										MatchTokenType<CommonParser::Lexer::TokenTypes::StringLiteral>,
										MatchTokenType<Assembler::TokenTypes::BooleanLiteral>,
										RuleReference<SGE_STR("AttributeDictionary")>>>;

		using AttributeDefinition = Rule<SGE_STR("AttributeDefinition"),
										 Sequence<
											 MatchTokenType<CommonParser::Lexer::TokenTypes::Identifier>,
											 MatchTokenType<CommonParser::Lexer::TokenTypes::Equal>,
											 RuleReference<SGE_STR("AttributeValue")>>>;

		using AttributeDictionary = Rule<SGE_STR("AttributeDictionary"),
										 Sequence<
											 MatchTokenType<CommonParser::Lexer::TokenTypes::LeftSquareBracket>,
											 Grammar::Optional<
												 Sequence<
													 RuleReference<SGE_STR("AttributeDefinition")>,
													 Repeat<
														 Sequence<
															 MatchTokenType<CommonParser::Lexer::TokenTypes::Comma>,
															 RuleReference<SGE_STR("AttributeDefinition")>>>>>,
											 MatchTokenType<CommonParser::Lexer::TokenTypes::RightSquareBracket>>>;

		using Program = Rule<SGE_STR("Program"),
							 Repeat<RuleReference<SGE_STR("Statement")>>>;

		using AssemblerLanguage = Language<Symbol,
										   VariableIdentifier,
										   VariableDefinition,
										   ParameterDefinition,
										   ParameterDefinitions,
										   Block,
										   Argument,
										   Statement,
										   AttributeValue,
										   AttributeDefinition,
										   AttributeDictionary,
										   Program>;
	}

	COMMON_INTERMEDIATE_REPRESENTATION_API Pair<SpaceGameEngine::Optional<CommonParser::Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode>, Vector<CommonParser::ParserError>> Parse(const Vector<CommonParser::Lexer::Token>& tokens);
}

/*!
@}
*/