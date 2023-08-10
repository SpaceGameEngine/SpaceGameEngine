/*
Copyright 2023 creatorlxd

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

SpaceGameEngine::SpaceLanguage::RegisterNameSet::RegisterNameSet()
	: m_Content({
		  Pair<const String, UInt8>(SGE_STR("pc"), Register::ProgramCounter),
		  Pair<const String, UInt8>(SGE_STR("bp"), Register::BasePointer),
		  Pair<const String, UInt8>(SGE_STR("sp"), Register::StackPointer),
		  Pair<const String, UInt8>(SGE_STR("gp"), Register::GlobalPointer),
		  Pair<const String, UInt8>(SGE_STR("rv"), Register::ReturnValue),
	  })
{
	for (UInt8 i = SpecialRegistersSize; i < ArgumentRegistersStartIndex; ++i)
		m_Content.Insert(Format(String(SGE_STR("c{}")), i - SpecialRegistersSize), i);

	for (UInt8 i = ArgumentRegistersStartIndex; i < RegistersSize; ++i)
		m_Content.Insert(Format(String(SGE_STR("a{}")), i - ArgumentRegistersStartIndex), i);
}

UInt8 SpaceGameEngine::SpaceLanguage::RegisterNameSet::Get(const String& register_name) const
{
	SGE_ASSERT(InvalidRegisterNameError, register_name);
	return m_Content.Find(register_name)->m_Second;
}

bool SpaceGameEngine::SpaceLanguage::RegisterNameSet::IsRegisterName(const String& str) const
{
	return m_Content.Find(str) != m_Content.GetConstEnd();
}

bool SpaceGameEngine::SpaceLanguage::InvalidRegisterNameError::Judge(const String& str)
{
	return !RegisterNameSet::GetSingleton().IsRegisterName(str);
}

bool SpaceGameEngine::SpaceLanguage::InvalidAssemblerSourceStringError::Judge(const String& str, const String& error_info_formatter, const HashMap<String, Pair<UInt32, HashMap<String, UInt32>>>& module_functions)
{
	if (InvalidSourceStringError::Judge(str, error_info_formatter))
		return true;

	Vector<Token> tokens = GetTokens(str, error_info_formatter);
	SizeType instr_size = 0;
	const InstructionType* pinstr = nullptr;
	HashMap<String, Pair<SizeType, SizeType>> need_tags;
	HashMap<String, bool> tags;
	SizeType instr_line = 0;

	for (auto iter = tokens.GetConstBegin(); iter != tokens.GetConstEnd(); ++iter)
	{
		if (iter->GetType() == TokenType::LineSeparator || iter->GetType() == TokenType::CommentLine || iter->GetType() == TokenType::CommentBlock)
			continue;
		else if (iter->GetType() == TokenType::Colon)
		{
			if (instr_size)
			{
				SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, iter->GetLine(), iter->GetColumn(), SGE_STR("Invalid : here"))));
				return true;
			}
			auto next = iter + 1;
			if (next == tokens.GetConstEnd())
			{
				SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, iter->GetLine(), iter->GetColumn(), SGE_STR("Need tag here"))));
				return true;
			}
			if (next->GetType() != TokenType::Identifier)
			{
				SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, iter->GetLine(), iter->GetColumn(), SGE_STR("Invalid tag name"))));
				return true;
			}
			if (tags.Find(next->GetContent()) != tags.GetEnd())
			{
				SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, iter->GetLine(), iter->GetColumn(), SGE_STR("Repeated tag name"))));
				return true;
			}
			tags.Insert(next->GetContent(), true);
			auto ntiter = need_tags.Find(next->GetContent());
			if (ntiter != need_tags.GetEnd())
				need_tags.Remove(ntiter);
			++iter;
		}
		else if (instr_size == 8)
		{
			if (iter->GetType() == TokenType::Identifier)
			{
				if (pinstr->m_Index != InstructionTypeIndex::ExternalCall)
				{
					if (tags.Find(iter->GetContent()) == tags.GetEnd())
					{
						if (need_tags.Find(iter->GetContent()) == need_tags.GetEnd())
							need_tags.Insert(iter->GetContent(), Pair<SizeType, SizeType>(iter->GetLine(), iter->GetColumn()));
					}
				}
				else
				{
					auto mod_iter = module_functions.Find(iter->GetContent());
					if (mod_iter == module_functions.GetConstEnd())
					{
						SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, iter->GetLine(), iter->GetColumn(), SGE_STR("Unknown module name"))));
						return true;
					}
					auto next = iter + 1;
					if (next == tokens.GetConstEnd() || next->GetType() != TokenType::Colon)
					{
						SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, iter->GetLine(), iter->GetColumn(), SGE_STR("Need : here"))));
						return true;
					}
					auto next_next = next + 1;
					if (next_next == tokens.GetConstEnd())
					{
						SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, iter->GetLine(), iter->GetColumn(), SGE_STR("Need function name here"))));
						return true;
					}
					if (mod_iter->m_Second.m_Second.Find(next_next->GetContent()) == mod_iter->m_Second.m_Second.GetConstEnd())
					{
						SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, iter->GetLine(), iter->GetColumn(), SGE_STR("Unknown function name"))));
						return true;
					}
					iter += 2;
				}
			}
			else if (iter->GetType() != TokenType::IntegerLiteral && iter->GetType() != TokenType::WordSeparator)
			{
				SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, iter->GetLine(), iter->GetColumn(), SGE_STR("Need integer here"))));
				return true;
			}

			if (iter->GetType() != TokenType::WordSeparator)
				instr_size = 0;
		}
		else
		{
			if (iter->GetType() != TokenType::WordSeparator)
			{
				if (instr_size)
				{
					if (iter->GetType() == TokenType::IntegerLiteral)
					{
						if (StringTo<String, UInt8>(iter->GetContent()) >= RegistersSize)
						{
							SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, iter->GetLine(), iter->GetColumn(), SGE_STR("Invalid register index"))));
							return true;
						}
					}
					else if (iter->GetType() == TokenType::Identifier)
					{
						if (!RegisterNameSet::GetSingleton().IsRegisterName(iter->GetContent()))
						{
							SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, iter->GetLine(), iter->GetColumn(), SGE_STR("Invalid register name"))));
							return true;
						}
					}
					else
					{
						SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, iter->GetLine(), iter->GetColumn(), SGE_STR("Need register here"))));
						return true;
					}

					instr_size -= 1;
				}
				else if (iter->GetType() == TokenType::Identifier)
				{
					if (iter->GetLine() == instr_line)
					{
						SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, iter->GetLine(), iter->GetColumn(), SGE_STR("Need line break here"))));
						return true;
					}
					if (!InstructionNameSet::GetSingleton().IsInstructionName(iter->GetContent()))
					{
						SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, iter->GetLine(), iter->GetColumn(), SGE_STR("Invalid instruction name"))));
						return true;
					}
					pinstr = &(InstructionNameSet::GetSingleton().Get(iter->GetContent()));
					instr_size = pinstr->m_Size - 1;
					instr_line = iter->GetLine();
				}
				else
				{
					SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, iter->GetLine(), iter->GetColumn(), SGE_STR("Invalid token type"))));
					return true;
				}
			}
		}
	}

	if (instr_size)
	{
		auto iter = tokens.GetConstReverseBegin();
		SGE_LOG(GetSpaceLanguageLogger(), LogLevel::Error, SGE_STR_TO_UTF8(Format(error_info_formatter, iter->GetLine(), iter->GetColumn(), SGE_STR("Need complete instruction"))));
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

bool SpaceGameEngine::SpaceLanguage::AssemblerExternalCallerModuleAlreadyExistError::Judge(const String& module_name, const HashMap<String, Pair<UInt32, HashMap<String, UInt32>>>& module_functions)
{
	return module_functions.Find(module_name) != module_functions.GetConstEnd();
}

void SpaceGameEngine::SpaceLanguage::Assembler::RegisterExternalCallerModule(const String& module_name, UInt32 module_id, const HashMap<String, UInt32>& functions)
{
	SGE_ASSERT(AssemblerExternalCallerModuleAlreadyExistError, module_name, m_ModuleFunctions);
	m_ModuleFunctions.Insert(module_name, Pair<UInt32, HashMap<String, UInt32>>(module_id, functions));
}

Vector<UInt8> SpaceGameEngine::SpaceLanguage::Assembler::Compile(const String& str, const String& error_info_formatter) const
{
	SGE_ASSERT(InvalidAssemblerSourceStringError, str, error_info_formatter, m_ModuleFunctions);

	Vector<UInt8> result;
	Vector<Token> tokens = GetTokens(str, error_info_formatter);
	SizeType instr_size = 0;
	const InstructionType* pinstr = nullptr;
	HashMap<String, Vector<SizeType>> need_tags;
	HashMap<String, UInt64> tags;
	SizeType write_idx = 0;

	for (auto iter = tokens.GetConstBegin(); iter != tokens.GetConstEnd(); ++iter)
	{
		if (iter->GetType() == TokenType::LineSeparator || iter->GetType() == TokenType::WordSeparator || iter->GetType() == TokenType::CommentBlock || iter->GetType() == TokenType::CommentLine)
			continue;

		if (instr_size == 8)
		{
			if (iter->GetType() == TokenType::IntegerLiteral)
				*(UInt64*)(&result[write_idx]) = StringTo<String, UInt64>(iter->GetContent());
			else if (pinstr->m_Index != InstructionTypeIndex::ExternalCall)
			{
				auto titer = tags.Find(iter->GetContent());
				if (titer != tags.GetEnd())
					*(UInt64*)(&result[write_idx]) = titer->m_Second;
				else
					need_tags[iter->GetContent()].EmplaceBack(write_idx);
			}
			else
			{
				auto mod_iter = m_ModuleFunctions.Find(iter->GetContent());
				iter += 2;
				auto function_iter = mod_iter->m_Second.m_Second.Find(iter->GetContent());
				*(UInt64*)(&result[write_idx]) = ExternalCaller::GetIndex(mod_iter->m_Second.m_First, function_iter->m_Second);
			}

			instr_size = 0;
			write_idx += 8;
		}
		else if (instr_size)
		{
			if (iter->GetType() == TokenType::IntegerLiteral)
				result[write_idx] = StringTo<String, UInt8>(iter->GetContent());
			else	// Identifier
				result[write_idx] = RegisterNameSet::GetSingleton().Get(iter->GetContent());
			instr_size -= 1;
			++write_idx;
		}
		else
		{
			if (iter->GetType() == TokenType::Identifier)
			{
				pinstr = &InstructionNameSet::GetSingleton().Get(iter->GetContent());
				instr_size = pinstr->m_Size - 1;
				result.SetSize(result.GetSize() + pinstr->m_Size, 0);
				result[write_idx] = pinstr->m_Index;
				++write_idx;
			}
			else if (iter->GetType() == TokenType::Colon)
			{
				++iter;
				tags.Insert(iter->GetContent(), result.GetSize());
				auto ntiter = need_tags.Find(iter->GetContent());
				if (ntiter != need_tags.GetEnd())
				{
					for (auto witer = ntiter->m_Second.GetConstBegin(); witer != ntiter->m_Second.GetConstEnd(); ++witer)
						*(UInt64*)(&result[*witer]) = result.GetSize();
					need_tags.Remove(ntiter);
				}
			}
		}
	}

	return result;
}
