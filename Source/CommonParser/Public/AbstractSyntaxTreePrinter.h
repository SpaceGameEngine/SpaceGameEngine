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
#include "AbstractSyntaxTree.h"
#include "Serializer/StringSerializer.h"
#include "Stream/StreamWriter.hpp"

/*!
@ingroup CommonParser
@{
*/

namespace SpaceGameEngine::CommonParser::Parser::AbstractSyntaxTree
{
	namespace Detail
	{
		COMMON_PARSER_API void PrintLine(StreamWriter<StringSerializer<String>>& stream_writer, const String& str, SizeType indent = 0);

		COMMON_PARSER_API void PrintNode(StreamWriter<StringSerializer<String>>& stream_writer, const AbstractSyntaxTreeNode& node, SizeType indent = 0);
	}

	COMMON_PARSER_API void PrintAbstractSyntaxTree(OutputStream& stream, const AbstractSyntaxTreeNode& node);
}

/*!
@}
*/