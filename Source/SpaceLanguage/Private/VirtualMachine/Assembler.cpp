/*
Copyright 2022 creatorlxd

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
#include "VirtualMachine/Assembler.h"
#include "Lexer.h"
#include "../Logger.h"
#include "File.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;
using namespace SpaceGameEngine::SpaceLanguage::Lexer;

SpaceGameEngine::SpaceLanguage::InstructionNameSet::InstructionNameSet()
{
	for (UInt8 i = 0; i < InstructionSetSize; ++i)
	{
		const InstructionType& instr = InstructionSet::GetSingleton().Get(i);
		m_Content.Insert(instr.m_Name, &instr);
	}
}

const InstructionType& SpaceGameEngine::SpaceLanguage::InstructionNameSet::Get(const String& instr_name) const
{
	SGE_ASSERT(InvalidInstructionNameError, instr_name);
	return *(m_Content.Find(instr_name)->m_Second);
}

bool SpaceGameEngine::SpaceLanguage::InstructionNameSet::IsInstructionName(const String& str) const
{
	return m_Content.Find(str) != m_Content.GetConstEnd();
}

bool SpaceGameEngine::SpaceLanguage::InvalidInstructionNameError::Judge(const String& str)
{
	return !InstructionNameSet::GetSingleton().IsInstructionName(str);
}

bool SpaceGameEngine::SpaceLanguage::InvalidAssemblerSourceStringError::Judge(const String& str, const String& error_info_formatter, const HashMap<String, Pair<UInt32, HashMap<String, UInt32>>>& module_functions)
{
	SizeType line = 1;
	SizeType col = 1;
	FileLineBreak flb = FileLineBreak::Unknown;
	String flb_str;
	Vector<Token> tokens = GetTokens(str, error_info_formatter);
	SizeType instr_size = 0;
	const InstructionType* pinstr = nullptr;
	HashMap<String, Pair<SizeType, SizeType>> need_tags;
	HashMap<String, bool> tags;

	for (auto iter = tokens.GetConstBegin(); iter != tokens.GetConstEnd(); ++iter)
	{
		if (iter->m_Type == TokenType::LineSeparator)
		{
			if (instr_size)
			{
				SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Invalid line break here"))));
				return true;
			}
			if (flb == FileLineBreak::Unknown)
			{
				if (iter->m_Content.GetSize() == 1)
					flb = GetFileLineBreak<String::CharType, String::ValueTrait>(iter->m_Content[0], iter->m_Content[0]);
				else
					flb = GetFileLineBreak<String::CharType, String::ValueTrait>(iter->m_Content[0], iter->m_Content[1]);
				flb_str = iter->m_Content;
			}
			line += 1;
			col = 1;
		}
		else if (iter->m_Type == TokenType::CommentBlock)
		{
			if (flb == FileLineBreak::Unknown)
			{
				bool has_lb = false;
				for (auto citer = iter->m_Content.GetConstBegin(); citer != iter->m_Content.GetConstEnd(); ++citer)
				{
					if (IsLineSeparatorCharacter<String::CharType, String::ValueTrait>(*citer))
					{
						if (instr_size)
						{
							SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Invalid line break here"))));
							return true;
						}
						flb = GetFileLineBreak<String::CharType, String::ValueTrait>(*citer, *citer);
						if (flb == FileLineBreak::CR)
						{
							auto cnext = citer + 1;
							if (cnext != iter->m_Content.GetConstEnd() && GetFileLineBreak<String::CharType, String::ValueTrait>(*citer, *cnext) == FileLineBreak::CRLF)
								flb = FileLineBreak::CRLF;
						}
						flb_str = GetFileLineBreakString<String::CharType, String::ValueTrait>(flb);
						line += 1;
						col = 3 + (iter->m_Content.GetConstEnd() - citer) - flb_str.GetSize();
						has_lb = true;
						break;
					}
				}
				if (!has_lb)
					col += iter->m_Content.GetSize() + 4;
			}
			else
			{
				auto citer = iter->m_Content.Find(flb_str, iter->m_Content.GetConstBegin(), iter->m_Content.GetConstEnd());
				if (citer != iter->m_Content.GetConstEnd())
				{
					if (instr_size)
					{
						SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Invalid line break here"))));
						return true;
					}
					line += 1;
					col = 3 + (iter->m_Content.GetConstEnd() - citer) - flb_str.GetSize();
				}
				else
					col += iter->m_Content.GetSize() + 4;
			}
		}
		else if (iter->m_Type == TokenType::CommentLine)
			col += iter->m_Content.GetSize() + 2;
		else if (iter->m_Type == TokenType::Colon)
		{
			if (instr_size)
			{
				SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Invalid : here"))));
				return true;
			}
			col += iter->m_Content.GetSize();
			auto next = iter + 1;
			if (next == tokens.GetConstEnd())
			{
				SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need tag here"))));
				return true;
			}
			if (next->m_Type != TokenType::Identifier)
			{
				SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Invalid tag name"))));
				return true;
			}
			if (tags.Find(next->m_Content) != tags.GetEnd())
			{
				SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Repeated tag name"))));
				return true;
			}
			tags.Insert(next->m_Content, true);
			auto ntiter = need_tags.Find(next->m_Content);
			if (ntiter != need_tags.GetEnd())
				need_tags.Remove(ntiter);
			col += next->m_Content.GetSize();
			++iter;
		}
		else if (instr_size == 8)
		{
			if (iter->m_Type == TokenType::Identifier)
			{
				if (pinstr->m_Index != InstructionTypeIndex::ExternalCall)
				{
					if (tags.Find(iter->m_Content) == tags.GetEnd())
						need_tags.Insert(iter->m_Content, Pair<SizeType, SizeType>(line, col));
					col += iter->m_Content.GetSize();
				}
				else
				{
					if (module_functions.Find(iter->m_Content) == module_functions.GetConstEnd())
					{
						SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Unknown module name"))));
						return true;
					}
					col += iter->m_Content.GetSize();
					auto next = iter + 1;
					if (next == tokens.GetConstEnd() || next->m_Type != TokenType::Colon)
					{
						SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need : here"))));
						return true;
					}
					col += next->m_Content.GetSize();
					auto next_next = next + 1;
					if (next_next == tokens.GetConstEnd())
					{
						SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need function name here"))));
						return true;
					}
					if (module_functions.Find(next_next->m_Content) == module_functions.GetConstEnd())
					{
						SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Unknown function name"))));
						return true;
					}
					col += next_next->m_Content.GetSize();
					iter += 2;
				}
			}
			else if (iter->m_Type != TokenType::IntegerLiteral)
			{
				SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need integer here"))));
				return true;
			}
			else
				col += iter->m_Content.GetSize();

			instr_size = 0;
		}
		else
		{
			if (iter->m_Type != TokenType::WordSeparator)
			{
				if (instr_size)
				{
					if (iter->m_Type != TokenType::IntegerLiteral)
					{
						SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need integer here"))));
						return true;
					}
					if (StringTo<String, UInt8>(iter->m_Content) >= RegistersSize)
					{
						SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Invalid register index"))));
						return true;
					}
					instr_size -= 1;
				}
				else if (iter->m_Type == TokenType::Identifier)
				{
					if (!InstructionNameSet::GetSingleton().IsInstructionName(iter->m_Content))
					{
						SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Invalid instruction name"))));
						return true;
					}
					pinstr = &(InstructionNameSet::GetSingleton().Get(iter->m_Content));
					instr_size = pinstr->m_Size - 1;
				}
				else
				{
					SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Invalid token type"))));
					return true;
				}
			}
			col += iter->m_Content.GetSize();
		}
	}

	if (instr_size)
	{
		SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, line, col, SGE_STR("Need complete instruction"))));
		return true;
	}

	if (need_tags.GetSize())
	{
		for (auto ntiter = need_tags.GetConstBegin(); ntiter != need_tags.GetConstEnd(); ++ntiter)
			SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, ntiter->m_Second.m_First, ntiter->m_Second.m_Second, SGE_STR("Unknown tag"))));
		return true;
	}

	return false;
}

InstructionsGenerator SpaceGameEngine::SpaceLanguage::Assembler::Compile(const String& str, const String& error_info_formatter) const
{
	SGE_ASSERT(InvalidAssemblerSourceStringError, str, error_info_formatter, m_ModuleFunctions);

	InstructionsGenerator result;

	return result;
}
