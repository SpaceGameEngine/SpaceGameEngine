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
#include "Attribute.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonIntermediateRepresentation;

TEST(Attribute, DynamicCastTest)
{
	Attribute attribute;
	IntegerAttribute int_attribute(42);
	ASSERT_TRUE(Attribute::IsInstance(attribute));
	ASSERT_TRUE(Attribute::IsInstance(int_attribute));	  // Attribute is a base class of IntegerAttribute, so this should return true
	ASSERT_EQ(DynamicCast<Attribute>(attribute), &attribute);
}

TEST(IntegerAttribute, SetValueTest)
{
	IntegerAttribute int_attribute(42);
	ASSERT_EQ(int_attribute.GetValue(), 42);
	int_attribute.SetValue(100);
	ASSERT_EQ(int_attribute.GetValue(), 100);
	int_attribute.SetValue(18446744073709551615ULL);
	ASSERT_EQ(int_attribute.GetValue(), 18446744073709551615ULL);
}

TEST(IntegerAttribute, GetValueTest)
{
	IntegerAttribute int_attribute(0);
	ASSERT_EQ(int_attribute.GetValue(), 0);
	IntegerAttribute int_attribute2(18446744073709551615ULL);
	ASSERT_EQ(int_attribute2.GetValue(), 18446744073709551615ULL);
}

TEST(IntegerAttribute, DynamicCastTest)
{
	IntegerAttribute int_attribute(42);
	FloatAttribute float_attribute(3.14f);
	Attribute& base_ref = int_attribute;
	ASSERT_TRUE(IntegerAttribute::IsInstance(int_attribute));
	ASSERT_FALSE(IntegerAttribute::IsInstance(float_attribute));
	ASSERT_EQ(DynamicCast<IntegerAttribute>(base_ref), &int_attribute);
}

TEST(FloatAttribute, SetValueTest)
{
	FloatAttribute float_attribute(3.14f);
	ASSERT_FLOAT_EQ(float_attribute.GetValue(), 3.14f);
	float_attribute.SetValue(-2.71f);
	ASSERT_FLOAT_EQ(float_attribute.GetValue(), -2.71f);
}

TEST(FloatAttribute, GetValueTest)
{
	FloatAttribute float_attribute(1.618f);
	ASSERT_FLOAT_EQ(float_attribute.GetValue(), 1.618f);
	FloatAttribute float_attribute2(-0.577f);
	ASSERT_FLOAT_EQ(float_attribute2.GetValue(), -0.577f);
}

TEST(FloatAttribute, DynamicCastTest)
{
	FloatAttribute float_attribute(3.14f);
	DoubleAttribute double_attribute(3.14);
	Attribute& base_ref = float_attribute;
	ASSERT_TRUE(FloatAttribute::IsInstance(float_attribute));
	ASSERT_FALSE(FloatAttribute::IsInstance(double_attribute));
	ASSERT_EQ(DynamicCast<FloatAttribute>(base_ref), &float_attribute);
}

TEST(DoubleAttribute, SetValueTest)
{
	DoubleAttribute double_attribute(3.141592653589793);
	ASSERT_DOUBLE_EQ(double_attribute.GetValue(), 3.141592653589793);
	double_attribute.SetValue(-2.718281828459045);
	ASSERT_DOUBLE_EQ(double_attribute.GetValue(), -2.718281828459045);
}

TEST(DoubleAttribute, GetValueTest)
{
	DoubleAttribute double_attribute(1.4142135623730951);
	ASSERT_DOUBLE_EQ(double_attribute.GetValue(), 1.4142135623730951);
	DoubleAttribute double_attribute2(-0.6931471805599453);
	ASSERT_DOUBLE_EQ(double_attribute2.GetValue(), -0.6931471805599453);
}

TEST(DoubleAttribute, DynamicCastTest)
{
	DoubleAttribute double_attribute(3.14);
	BooleanAttribute bool_attribute(true);
	Attribute& base_ref = double_attribute;
	ASSERT_TRUE(DoubleAttribute::IsInstance(double_attribute));
	ASSERT_FALSE(DoubleAttribute::IsInstance(bool_attribute));
	ASSERT_EQ(DynamicCast<DoubleAttribute>(base_ref), &double_attribute);
}

TEST(BooleanAttribute, SetValueTest)
{
	BooleanAttribute bool_attribute(true);
	ASSERT_TRUE(bool_attribute.GetValue());
	bool_attribute.SetValue(false);
	ASSERT_FALSE(bool_attribute.GetValue());
	bool_attribute.SetValue(true);
	ASSERT_TRUE(bool_attribute.GetValue());
}

TEST(BooleanAttribute, GetValueTest)
{
	BooleanAttribute bool_attribute(true);
	ASSERT_TRUE(bool_attribute.GetValue());
	BooleanAttribute bool_attribute2(false);
	ASSERT_FALSE(bool_attribute2.GetValue());
}

TEST(BooleanAttribute, DynamicCastTest)
{
	BooleanAttribute bool_attribute(true);
	StringAttribute string_attribute(SGE_STR("Test"));
	Attribute& base_ref = bool_attribute;
	ASSERT_TRUE(BooleanAttribute::IsInstance(bool_attribute));
	ASSERT_FALSE(BooleanAttribute::IsInstance(string_attribute));
	ASSERT_EQ(DynamicCast<BooleanAttribute>(base_ref), &bool_attribute);
}

TEST(StringAttribute, SetValueTest)
{
	StringAttribute string_attribute(SGE_STR("Hello, World!"));
	ASSERT_EQ(string_attribute.GetValue(), SGE_STR("Hello, World!"));
	String test_string(SGE_STR("Test"));
	string_attribute.SetValue(test_string);
	ASSERT_EQ(string_attribute.GetValue(), SGE_STR("Test"));
	string_attribute.SetValue(SGE_STR("Changed Value"));
	ASSERT_EQ(string_attribute.GetValue(), SGE_STR("Changed Value"));
	String another_string(SGE_STR("Another Value"));
	string_attribute.SetValue(std::move(another_string));
	ASSERT_EQ(string_attribute.GetValue(), SGE_STR("Another Value"));
}

TEST(StringAttribute, GetValueTest)
{
	StringAttribute string_attribute(SGE_STR("Initial Value"));
	ASSERT_EQ(string_attribute.GetValue(), SGE_STR("Initial Value"));
	String test_string(SGE_STR("Another Value"));
	StringAttribute string_attribute2(test_string);
	ASSERT_EQ(string_attribute2.GetValue(), SGE_STR("Another Value"));
	StringAttribute string_attribute3(std::move(test_string));
	ASSERT_EQ(string_attribute3.GetValue(), SGE_STR("Another Value"));
}

TEST(StringAttribute, DynamicCastTest)
{
	StringAttribute string_attribute(SGE_STR("Test"));
	IntegerAttribute int_attribute(42);
	Attribute& base_ref = string_attribute;
	ASSERT_TRUE(StringAttribute::IsInstance(string_attribute));
	ASSERT_FALSE(StringAttribute::IsInstance(int_attribute));
	ASSERT_EQ(DynamicCast<StringAttribute>(base_ref), &string_attribute);
}

TEST(DictionaryAttribute, UpsertAttributeTest)
{
	DictionaryAttribute dictionary;
	ASSERT_FALSE(dictionary.UpsertAttribute<IntegerAttribute>(SGE_STR("int"), 42));
	ASSERT_TRUE(dictionary.UpsertAttribute<IntegerAttribute>(SGE_STR("int"), 100));
	Attribute* int_ptr = dictionary.GetAttribute(SGE_STR("int"));
	ASSERT_NE(int_ptr, nullptr);
	IntegerAttribute* int_attribute = DynamicCast<IntegerAttribute>(*int_ptr);
	ASSERT_NE(int_attribute, nullptr);
	ASSERT_EQ(int_attribute->GetValue(), 100);
}

TEST(DictionaryAttribute, UpsertAttributeWithDifferentTypeTest)
{
	DictionaryAttribute dictionary;
	ASSERT_FALSE(dictionary.UpsertAttribute<IntegerAttribute>(SGE_STR("key"), 42));
	ASSERT_TRUE(dictionary.UpsertAttribute<StringAttribute>(SGE_STR("key"), SGE_STR("Hello")));
	Attribute* attr_ptr = dictionary.GetAttribute(SGE_STR("key"));
	ASSERT_NE(attr_ptr, nullptr);
	ASSERT_EQ(DynamicCast<IntegerAttribute>(*attr_ptr), nullptr);
	StringAttribute* string_attribute = DynamicCast<StringAttribute>(*attr_ptr);
	ASSERT_NE(string_attribute, nullptr);
	ASSERT_EQ(string_attribute->GetValue(), SGE_STR("Hello"));
}

TEST(DictionaryAttribute, RemoveAttributeTest)
{
	DictionaryAttribute dictionary;
	dictionary.UpsertAttribute<IntegerAttribute>(SGE_STR("int"), 42);
	dictionary.UpsertAttribute<FloatAttribute>(SGE_STR("float"), 3.14f);
	ASSERT_TRUE(dictionary.RemoveAttribute(SGE_STR("int")));
	ASSERT_EQ(dictionary.GetAttribute(SGE_STR("int")), nullptr);
	ASSERT_FALSE(dictionary.RemoveAttribute(SGE_STR("int")));
	ASSERT_NE(dictionary.GetAttribute(SGE_STR("float")), nullptr);
}

TEST(DictionaryAttribute, GetAttributeTest)
{
	DictionaryAttribute dictionary;
	ASSERT_EQ(dictionary.GetAttribute(SGE_STR("key")), nullptr);
	dictionary.UpsertAttribute<StringAttribute>(SGE_STR("str"), SGE_STR("Hello"));
	const DictionaryAttribute& const_dictionary = dictionary;
	const Attribute* str_ptr = const_dictionary.GetAttribute(SGE_STR("str"));
	ASSERT_NE(str_ptr, nullptr);
	const StringAttribute* string_attribute = DynamicCast<StringAttribute>(*str_ptr);
	ASSERT_NE(string_attribute, nullptr);
	ASSERT_EQ(string_attribute->GetValue(), SGE_STR("Hello"));
	ASSERT_EQ(const_dictionary.GetAttribute(SGE_STR("key")), nullptr);
}

TEST(DictionaryAttribute, DynamicCastTest)
{
	DictionaryAttribute dictionary;
	IntegerAttribute int_attribute(42);
	Attribute& base_ref = dictionary;
	ASSERT_TRUE(DictionaryAttribute::IsInstance(dictionary));
	ASSERT_FALSE(DictionaryAttribute::IsInstance(int_attribute));
	ASSERT_EQ(DynamicCast<DictionaryAttribute>(base_ref), &dictionary);
}