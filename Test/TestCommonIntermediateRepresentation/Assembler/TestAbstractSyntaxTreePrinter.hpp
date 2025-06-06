/*
Copyright 2025 creatorlxd

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
#include "Assembler/AbstractSyntaxTreePrinter.h"
#include "Stream/CumulateStream.hpp"
#include "Stream/StreamReader.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonIntermediateRepresentation::Assembler;

TEST(AbstractSyntaxTreePrinter, Test)
{
	CumulateStream stream;
	StreamReader<StringSerializer<String>> reader(stream);

	AbstractSyntaxTreePrinter printer(stream);

	AbstractSyntaxTreeNodes::VariableNode variable_node(SGE_STR("test"));
	variable_node.Accept(printer);

	String result;
	reader >> result;
	ASSERT_EQ(result, SGE_STR("Variable \"test\"\n"));

	// todo add more tests for other node types
}