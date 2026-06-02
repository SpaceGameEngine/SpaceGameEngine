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
#include "Utility/Format.hpp"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonParser;
using namespace SpaceGameEngine::CommonParser::Parser::AbstractSyntaxTree;

void SpaceGameEngine::CommonParser::Parser::AbstractSyntaxTree::Detail::PrintLine(StreamWriter<StringSerializer<String>>& stream_writer, const String& str, SizeType indent)
{
	for (SizeType i = 0; i < indent; ++i)
		stream_writer << SGE_STR('\t');
	stream_writer << str << SGE_STR('\n');
}

void SpaceGameEngine::CommonParser::Parser::AbstractSyntaxTree::Detail::PrintNode(StreamWriter<StringSerializer<String>>& stream_writer, const AbstractSyntaxTreeNode& node, SizeType indent)
{
	Detail::PrintLine(stream_writer, Format(String(SGE_STR("{}\t<line:{}, column:{}>")), node.GetName(), node.GetBeginTokenIter()->GetLine(), node.GetBeginTokenIter()->GetColumn()), indent);
	for (auto iter = node.GetChildren().GetConstBegin(); iter != node.GetChildren().GetConstEnd(); ++iter)
		Detail::PrintNode(stream_writer, *iter, indent + 1);
}

void SpaceGameEngine::CommonParser::Parser::AbstractSyntaxTree::PrintAbstractSyntaxTree(OutputStream& stream, const AbstractSyntaxTreeNode& node)
{
	StreamWriter<StringSerializer<String>> stream_writer(stream);
	Detail::PrintNode(stream_writer, node);
}
