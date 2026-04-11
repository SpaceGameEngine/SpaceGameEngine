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
#include "ParserError.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonParser;

TEST(ParserError, ConstructorTest)
{
	Vector<String> additional_info;
	additional_info.EmplaceBack(SGE_STR("test1"));
	additional_info.EmplaceBack(SGE_STR("test2"));
	ParserError error(123, 2, 4, std::move(additional_info));
	ASSERT_EQ(error.GetTypeId(), 123);
	ASSERT_EQ(error.GetLine(), 2);
	ASSERT_EQ(error.GetColumn(), 4);
	ASSERT_EQ(error.GetAdditionalInformation().GetSize(), 2);
	ASSERT_EQ(error.GetAdditionalInformation()[0], SGE_STR("test1"));
	ASSERT_EQ(error.GetAdditionalInformation()[1], SGE_STR("test2"));

	ParserError error2 = error;
	ASSERT_EQ(error2.GetTypeId(), 123);
	ASSERT_EQ(error2.GetLine(), 2);
	ASSERT_EQ(error2.GetColumn(), 4);
	ASSERT_EQ(error2.GetAdditionalInformation().GetSize(), 2);
	ASSERT_EQ(error2.GetAdditionalInformation()[0], SGE_STR("test1"));
	ASSERT_EQ(error2.GetAdditionalInformation()[1], SGE_STR("test2"));

	ParserError error3 = std::move(error);
	ASSERT_EQ(error3.GetTypeId(), 123);
	ASSERT_EQ(error3.GetLine(), 2);
	ASSERT_EQ(error3.GetColumn(), 4);
	ASSERT_EQ(error3.GetAdditionalInformation().GetSize(), 2);
	ASSERT_EQ(error3.GetAdditionalInformation()[0], SGE_STR("test1"));
	ASSERT_EQ(error3.GetAdditionalInformation()[1], SGE_STR("test2"));
}

TEST(ParserError, AssignmentTest)
{
	Vector<String> additional_info1;
	additional_info1.EmplaceBack(SGE_STR("test1"));
	additional_info1.EmplaceBack(SGE_STR("test2"));
	ParserError error1(123, 2, 4, std::move(additional_info1));

	Vector<String> additional_info2;
	additional_info2.EmplaceBack(SGE_STR("test3"));
	ParserError error2(456, 5, 6, std::move(additional_info2));

	error2 = error1;
	ASSERT_EQ(error2.GetTypeId(), 123);
	ASSERT_EQ(error2.GetLine(), 2);
	ASSERT_EQ(error2.GetColumn(), 4);
	ASSERT_EQ(error2.GetAdditionalInformation().GetSize(), 2);
	ASSERT_EQ(error2.GetAdditionalInformation()[0], SGE_STR("test1"));
	ASSERT_EQ(error2.GetAdditionalInformation()[1], SGE_STR("test2"));

	ASSERT_EQ(error1.GetTypeId(), 123);
	ASSERT_EQ(error1.GetLine(), 2);
	ASSERT_EQ(error1.GetColumn(), 4);
	ASSERT_EQ(error1.GetAdditionalInformation().GetSize(), 2);

	Vector<String> additional_info3;
	additional_info3.EmplaceBack(SGE_STR("test4"));
	additional_info3.EmplaceBack(SGE_STR("test5"));
	ParserError error3(789, 7, 8, std::move(additional_info3));

	error3 = std::move(error1);
	ASSERT_EQ(error3.GetTypeId(), 123);
	ASSERT_EQ(error3.GetLine(), 2);
	ASSERT_EQ(error3.GetColumn(), 4);
	ASSERT_EQ(error3.GetAdditionalInformation().GetSize(), 2);
	ASSERT_EQ(error3.GetAdditionalInformation()[0], SGE_STR("test1"));
	ASSERT_EQ(error3.GetAdditionalInformation()[1], SGE_STR("test2"));
}

TEST(ParserError, EqualTest)
{
	Vector<String> additional_info1;
	additional_info1.EmplaceBack(SGE_STR("test1"));
	additional_info1.EmplaceBack(SGE_STR("test2"));
	ParserError error1(123, 2, 4, std::move(additional_info1));
	Vector<String> additional_info2;
	additional_info2.EmplaceBack(SGE_STR("test1"));
	additional_info2.EmplaceBack(SGE_STR("test2"));
	ParserError error2(123, 2, 4, std::move(additional_info2));
	ASSERT_TRUE(error1 == error2);
}

TEST(ParserError, NotEqualTest)
{
	Vector<String> additional_info1;
	additional_info1.EmplaceBack(SGE_STR("test1"));
	additional_info1.EmplaceBack(SGE_STR("test2"));
	ParserError error1(123, 2, 4, std::move(additional_info1));
	Vector<String> additional_info2;
	additional_info2.EmplaceBack(SGE_STR("test1"));
	additional_info2.EmplaceBack(SGE_STR("test3"));
	ParserError error2(123, 2, 4, std::move(additional_info2));
	ASSERT_TRUE(error1 != error2);
}