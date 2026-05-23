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
#include "AbstractSyntaxTreePrinter.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonParser;
using namespace SpaceGameEngine::CommonParser::Parser::AbstractSyntaxTree;

void SpaceGameEngine::CommonParser::Parser::AbstractSyntaxTree::Detail::PrintLine(StreamWriter<StringSerializer<String>>& stream_writer, const String& str, SizeType indent)
{
	for (SizeType i = 0; i < indent; ++i)
		stream_writer << SGE_STR('\t');
	stream_writer << str << SGE_STR('\n');
}
