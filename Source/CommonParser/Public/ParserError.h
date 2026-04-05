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
#include "CommonParserAPI.h"
#include "TypeDefinition.hpp"
#include "Container/Vector.hpp"
#include "SGEString.hpp"

/*!
@ingroup CommonParser
@{
*/

namespace SpaceGameEngine::CommonParser
{
	class COMMON_PARSER_API ParserError
	{
	public:
		ParserError(SizeType type_id, SizeType line, SizeType column, Vector<String>&& additional_information = Vector<String>());

		ParserError(const ParserError&) = default;
		ParserError(ParserError&&) = default;

		ParserError& operator=(const ParserError&) = default;
		ParserError& operator=(ParserError&&) = default;

		SizeType GetTypeId() const;
		SizeType GetLine() const;
		SizeType GetColumn() const;
		const Vector<String>& GetAdditionalInformation() const;

	private:
		SizeType m_TypeId;
		SizeType m_Line;
		SizeType m_Column;
		Vector<String> m_AdditionalInformation;
	};
}

/*!
@}
*/