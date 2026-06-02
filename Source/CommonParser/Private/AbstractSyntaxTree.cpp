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
#include "AbstractSyntaxTree.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonParser;
using namespace SpaceGameEngine::CommonParser::Parser::AbstractSyntaxTree;

SpaceGameEngine::CommonParser::Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode::AbstractSyntaxTreeNode(const Char* pname, const Vector<Lexer::Token>::ConstIterator& begin_token_iter, const Vector<Lexer::Token>::ConstIterator& end_token_iter, Vector<AbstractSyntaxTreeNode>&& children)
	: m_pName(pname), m_BeginTokenIter(begin_token_iter), m_EndTokenIter(end_token_iter), m_Children(std::move(children))
{
	SGE_ASSERT(NullPointerError, pname);
}

const Char* SpaceGameEngine::CommonParser::Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode::GetName() const
{
	return m_pName;
}

const Vector<Lexer::Token>::ConstIterator& SpaceGameEngine::CommonParser::Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode::GetBeginTokenIter() const
{
	return m_BeginTokenIter;
}

const Vector<Lexer::Token>::ConstIterator& SpaceGameEngine::CommonParser::Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode::GetEndTokenIter() const
{
	return m_EndTokenIter;
}

const Vector<AbstractSyntaxTreeNode>& SpaceGameEngine::CommonParser::Parser::AbstractSyntaxTree::AbstractSyntaxTreeNode::GetChildren() const
{
	return m_Children;
}
