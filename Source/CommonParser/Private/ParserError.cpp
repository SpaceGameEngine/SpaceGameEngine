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
#include "ParserError.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonParser;

SpaceGameEngine::CommonParser::ParserError::ParserError(SizeType type_id, SizeType line, SizeType column, Vector<String>&& additional_information)
	: m_TypeId(type_id), m_Line(line), m_Column(column), m_AdditionalInformation(std::move(additional_information))
{
}

SizeType SpaceGameEngine::CommonParser::ParserError::GetTypeId() const
{
	return m_TypeId;
}

SizeType SpaceGameEngine::CommonParser::ParserError::GetLine() const
{
	return m_Line;
}

SizeType SpaceGameEngine::CommonParser::ParserError::GetColumn() const
{
	return m_Column;
}

const Vector<String>& SpaceGameEngine::CommonParser::ParserError::GetAdditionalInformation() const
{
	return m_AdditionalInformation;
}
