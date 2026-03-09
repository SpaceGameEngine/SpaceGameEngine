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
#include "LexerUtility.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonParser;
using namespace SpaceGameEngine::CommonParser::Lexer;

Vector<Token> SpaceGameEngine::CommonParser::Lexer::CombineToken(const Vector<Token>& tokens, const Vector<TokenType>& token_type_pattern, TokenType result_token_type, bool repeat)
{
	Vector<Token> src_tokens;
	Vector<Token> result = tokens;	  // for next move
	bool has_changed = false;
	do
	{
		src_tokens = std::move(result);
		result.Clear();
		has_changed = false;
		auto iter = src_tokens.GetConstBegin();
		while (iter != src_tokens.GetConstEnd())
		{
			auto iter_compare = iter;
			auto iter_pattern = token_type_pattern.GetConstBegin();
			while (iter_compare != src_tokens.GetConstEnd() && iter_pattern != token_type_pattern.GetConstEnd() && iter_compare->GetType() == *iter_pattern)
			{
				++iter_compare;
				++iter_pattern;
			}
			if (iter_pattern == token_type_pattern.GetConstEnd())
			{
				has_changed = true;
				String new_content;
				for (auto iter_token = iter; iter_token != iter_compare; ++iter_token)
				{
					new_content += iter_token->GetContent();
				}
				result.EmplaceBack(result_token_type, new_content, iter->GetLine(), iter->GetColumn());
				iter = iter_compare;
			}
			else
			{
				result.EmplaceBack(*iter);
				++iter;
			}
		}

	} while (repeat && has_changed);
	return result;
}

Vector<Token> SpaceGameEngine::CommonParser::Lexer::RemoveToken(const Vector<Token>& tokens, const Set<TokenType>& remove_token_type_set)
{
	Vector<Token> result;
	for (auto iter = tokens.GetConstBegin(); iter != tokens.GetConstEnd(); ++iter)
	{
		if (!remove_token_type_set.Contains(iter->GetType()))
			result.EmplaceBack(*iter);
	}
	return result;
}
