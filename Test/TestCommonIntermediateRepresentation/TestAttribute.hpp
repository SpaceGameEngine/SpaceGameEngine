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

TEST(IntegerAttribute, CopyConstructionTest)
{
	IntegerAttribute int_attribute(42);
	IntegerAttribute int_attribute2(int_attribute);
	ASSERT_EQ(int_attribute.GetValue(), 42);
	ASSERT_EQ(int_attribute2.GetValue(), 42);
}

TEST(IntegerAttribute, MoveConstructionTest)
{
	IntegerAttribute int_attribute(42);
	IntegerAttribute int_attribute2(std::move(int_attribute));
	ASSERT_EQ(int_attribute2.GetValue(), 42);
}

TEST(IntegerAttribute, CopyAssignmentTest)
{
	IntegerAttribute int_attribute(42);
	IntegerAttribute int_attribute2(0);
	int_attribute2 = int_attribute;
	ASSERT_EQ(int_attribute.GetValue(), 42);
	ASSERT_EQ(int_attribute2.GetValue(), 42);
}

TEST(IntegerAttribute, MoveAssignmentTest)
{
	IntegerAttribute int_attribute(42);
	IntegerAttribute int_attribute2(0);
	int_attribute2 = std::move(int_attribute);
	ASSERT_EQ(int_attribute2.GetValue(), 42);
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

TEST(IntegerAttribute, CloneTest)
{
	IntegerAttribute int_attribute(42);
	Attribute* pclone = int_attribute.Clone();
	ASSERT_NE(pclone, nullptr);
	ASSERT_NE(pclone, &int_attribute);
	IntegerAttribute* pint = DynamicCast<IntegerAttribute>(*pclone);
	ASSERT_NE(pint, nullptr);
	ASSERT_EQ(pint->GetValue(), 42);
	pint->SetValue(100);
	ASSERT_EQ(int_attribute.GetValue(), 42);
	DefaultAllocator::Delete(pclone);
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

TEST(IntegerAttribute, GetTypeIdTest)
{
	IntegerAttribute int_attribute(42);

	ASSERT_EQ(int_attribute.GetTypeId(), GetTypeId<IntegerAttribute>());
}

TEST(FloatAttribute, CopyConstructionTest)
{
	FloatAttribute float_attribute(3.14f);
	FloatAttribute float_attribute2(float_attribute);
	ASSERT_FLOAT_EQ(float_attribute.GetValue(), 3.14f);
	ASSERT_FLOAT_EQ(float_attribute2.GetValue(), 3.14f);
}

TEST(FloatAttribute, MoveConstructionTest)
{
	FloatAttribute float_attribute(3.14f);
	FloatAttribute float_attribute2(std::move(float_attribute));
	ASSERT_FLOAT_EQ(float_attribute2.GetValue(), 3.14f);
}

TEST(FloatAttribute, CopyAssignmentTest)
{
	FloatAttribute float_attribute(3.14f);
	FloatAttribute float_attribute2(0.0f);
	float_attribute2 = float_attribute;
	ASSERT_FLOAT_EQ(float_attribute.GetValue(), 3.14f);
	ASSERT_FLOAT_EQ(float_attribute2.GetValue(), 3.14f);
}

TEST(FloatAttribute, MoveAssignmentTest)
{
	FloatAttribute float_attribute(3.14f);
	FloatAttribute float_attribute2(0.0f);
	float_attribute2 = std::move(float_attribute);
	ASSERT_FLOAT_EQ(float_attribute2.GetValue(), 3.14f);
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

TEST(FloatAttribute, CloneTest)
{
	FloatAttribute float_attribute(3.14f);
	Attribute* pclone = float_attribute.Clone();
	ASSERT_NE(pclone, nullptr);
	ASSERT_NE(pclone, &float_attribute);
	FloatAttribute* pfloat = DynamicCast<FloatAttribute>(*pclone);
	ASSERT_NE(pfloat, nullptr);
	ASSERT_FLOAT_EQ(pfloat->GetValue(), 3.14f);
	pfloat->SetValue(-2.71f);
	ASSERT_FLOAT_EQ(float_attribute.GetValue(), 3.14f);
	DefaultAllocator::Delete(pclone);
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

TEST(FloatAttribute, GetTypeIdTest)
{
	FloatAttribute float_attribute(3.14f);

	ASSERT_EQ(float_attribute.GetTypeId(), GetTypeId<FloatAttribute>());
}

TEST(DoubleAttribute, CopyConstructionTest)
{
	DoubleAttribute double_attribute(3.141592653589793);
	DoubleAttribute double_attribute2(double_attribute);
	ASSERT_DOUBLE_EQ(double_attribute.GetValue(), 3.141592653589793);
	ASSERT_DOUBLE_EQ(double_attribute2.GetValue(), 3.141592653589793);
}

TEST(DoubleAttribute, MoveConstructionTest)
{
	DoubleAttribute double_attribute(3.141592653589793);
	DoubleAttribute double_attribute2(std::move(double_attribute));
	ASSERT_DOUBLE_EQ(double_attribute2.GetValue(), 3.141592653589793);
}

TEST(DoubleAttribute, CopyAssignmentTest)
{
	DoubleAttribute double_attribute(3.141592653589793);
	DoubleAttribute double_attribute2(0.0);
	double_attribute2 = double_attribute;
	ASSERT_DOUBLE_EQ(double_attribute.GetValue(), 3.141592653589793);
	ASSERT_DOUBLE_EQ(double_attribute2.GetValue(), 3.141592653589793);
}

TEST(DoubleAttribute, MoveAssignmentTest)
{
	DoubleAttribute double_attribute(3.141592653589793);
	DoubleAttribute double_attribute2(0.0);
	double_attribute2 = std::move(double_attribute);
	ASSERT_DOUBLE_EQ(double_attribute2.GetValue(), 3.141592653589793);
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

TEST(DoubleAttribute, CloneTest)
{
	DoubleAttribute double_attribute(3.141592653589793);
	Attribute* pclone = double_attribute.Clone();
	ASSERT_NE(pclone, nullptr);
	ASSERT_NE(pclone, &double_attribute);
	DoubleAttribute* pdouble = DynamicCast<DoubleAttribute>(*pclone);
	ASSERT_NE(pdouble, nullptr);
	ASSERT_DOUBLE_EQ(pdouble->GetValue(), 3.141592653589793);
	pdouble->SetValue(-2.718281828459045);
	ASSERT_DOUBLE_EQ(double_attribute.GetValue(), 3.141592653589793);
	DefaultAllocator::Delete(pclone);
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

TEST(DoubleAttribute, GetTypeIdTest)
{
	DoubleAttribute double_attribute(3.14);

	ASSERT_EQ(double_attribute.GetTypeId(), GetTypeId<DoubleAttribute>());
}

TEST(BooleanAttribute, CopyConstructionTest)
{
	BooleanAttribute bool_attribute(true);
	BooleanAttribute bool_attribute2(bool_attribute);
	ASSERT_TRUE(bool_attribute.GetValue());
	ASSERT_TRUE(bool_attribute2.GetValue());
}

TEST(BooleanAttribute, MoveConstructionTest)
{
	BooleanAttribute bool_attribute(true);
	BooleanAttribute bool_attribute2(std::move(bool_attribute));
	ASSERT_TRUE(bool_attribute2.GetValue());
}

TEST(BooleanAttribute, CopyAssignmentTest)
{
	BooleanAttribute bool_attribute(true);
	BooleanAttribute bool_attribute2(false);
	bool_attribute2 = bool_attribute;
	ASSERT_TRUE(bool_attribute.GetValue());
	ASSERT_TRUE(bool_attribute2.GetValue());
}

TEST(BooleanAttribute, MoveAssignmentTest)
{
	BooleanAttribute bool_attribute(true);
	BooleanAttribute bool_attribute2(false);
	bool_attribute2 = std::move(bool_attribute);
	ASSERT_TRUE(bool_attribute2.GetValue());
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

TEST(BooleanAttribute, CloneTest)
{
	BooleanAttribute bool_attribute(true);
	Attribute* pclone = bool_attribute.Clone();
	ASSERT_NE(pclone, nullptr);
	ASSERT_NE(pclone, &bool_attribute);
	BooleanAttribute* pbool = DynamicCast<BooleanAttribute>(*pclone);
	ASSERT_NE(pbool, nullptr);
	ASSERT_TRUE(pbool->GetValue());
	pbool->SetValue(false);
	ASSERT_TRUE(bool_attribute.GetValue());
	DefaultAllocator::Delete(pclone);
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

TEST(BooleanAttribute, GetTypeIdTest)
{
	BooleanAttribute bool_attribute(true);

	ASSERT_EQ(bool_attribute.GetTypeId(), GetTypeId<BooleanAttribute>());
}

TEST(StringAttribute, CopyConstructionTest)
{
	StringAttribute string_attribute(SGE_STR("Hello"));
	StringAttribute string_attribute2(string_attribute);
	ASSERT_EQ(string_attribute.GetValue(), SGE_STR("Hello"));
	ASSERT_EQ(string_attribute2.GetValue(), SGE_STR("Hello"));
}

TEST(StringAttribute, MoveConstructionTest)
{
	StringAttribute string_attribute(SGE_STR("Hello"));
	StringAttribute string_attribute2(std::move(string_attribute));
	ASSERT_EQ(string_attribute2.GetValue(), SGE_STR("Hello"));
}

TEST(StringAttribute, CopyAssignmentTest)
{
	StringAttribute string_attribute(SGE_STR("Hello"));
	StringAttribute string_attribute2(SGE_STR("World"));
	string_attribute2 = string_attribute;
	ASSERT_EQ(string_attribute.GetValue(), SGE_STR("Hello"));
	ASSERT_EQ(string_attribute2.GetValue(), SGE_STR("Hello"));
}

TEST(StringAttribute, MoveAssignmentTest)
{
	StringAttribute string_attribute(SGE_STR("Hello"));
	StringAttribute string_attribute2(SGE_STR("World"));
	string_attribute2 = std::move(string_attribute);
	ASSERT_EQ(string_attribute2.GetValue(), SGE_STR("Hello"));
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

TEST(StringAttribute, CloneTest)
{
	StringAttribute string_attribute(SGE_STR("Hello"));
	Attribute* pclone = string_attribute.Clone();
	ASSERT_NE(pclone, nullptr);
	ASSERT_NE(pclone, &string_attribute);
	StringAttribute* pstr = DynamicCast<StringAttribute>(*pclone);
	ASSERT_NE(pstr, nullptr);
	ASSERT_EQ(pstr->GetValue(), SGE_STR("Hello"));
	pstr->SetValue(SGE_STR("Changed"));
	ASSERT_EQ(string_attribute.GetValue(), SGE_STR("Hello"));
	DefaultAllocator::Delete(pclone);
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

TEST(StringAttribute, GetTypeIdTest)
{
	StringAttribute string_attribute(SGE_STR("Test"));

	ASSERT_EQ(string_attribute.GetTypeId(), GetTypeId<StringAttribute>());
}

TEST(DictionaryAttribute, CopyConstructionTest)
{
	DictionaryAttribute dictionary;
	dictionary.UpsertAttribute<IntegerAttribute>(SGE_STR("int"), 42);
	dictionary.UpsertAttribute<StringAttribute>(SGE_STR("str"), SGE_STR("Hello"));

	DictionaryAttribute dictionary2(dictionary);
	Attribute* pint_attr = dictionary2.GetAttribute(SGE_STR("int"));
	ASSERT_NE(pint_attr, nullptr);
	ASSERT_NE(pint_attr, dictionary.GetAttribute(SGE_STR("int")));
	IntegerAttribute* pint = DynamicCast<IntegerAttribute>(*pint_attr);
	ASSERT_NE(pint, nullptr);
	ASSERT_EQ(pint->GetValue(), 42);
	Attribute* pstr_attr = dictionary2.GetAttribute(SGE_STR("str"));
	ASSERT_NE(pstr_attr, nullptr);
	ASSERT_EQ(DynamicCast<StringAttribute>(*pstr_attr)->GetValue(), SGE_STR("Hello"));

	pint->SetValue(100);
	ASSERT_EQ(DynamicCast<IntegerAttribute>(*dictionary.GetAttribute(SGE_STR("int")))->GetValue(), 42);
}

TEST(DictionaryAttribute, MoveConstructionTest)
{
	DictionaryAttribute dictionary;
	dictionary.UpsertAttribute<IntegerAttribute>(SGE_STR("int"), 42);

	DictionaryAttribute dictionary2(std::move(dictionary));
	Attribute* pint_attr = dictionary2.GetAttribute(SGE_STR("int"));
	ASSERT_NE(pint_attr, nullptr);
	ASSERT_EQ(DynamicCast<IntegerAttribute>(*pint_attr)->GetValue(), 42);
}

TEST(DictionaryAttribute, CopyAssignmentTest)
{
	DictionaryAttribute dictionary;
	dictionary.UpsertAttribute<IntegerAttribute>(SGE_STR("int"), 42);

	DictionaryAttribute dictionary2;
	dictionary2.UpsertAttribute<StringAttribute>(SGE_STR("str"), SGE_STR("Hello"));
	dictionary2 = dictionary;

	ASSERT_EQ(dictionary2.GetAttribute(SGE_STR("str")), nullptr);
	Attribute* pint_attr = dictionary2.GetAttribute(SGE_STR("int"));
	ASSERT_NE(pint_attr, nullptr);
	ASSERT_NE(pint_attr, dictionary.GetAttribute(SGE_STR("int")));
	ASSERT_EQ(DynamicCast<IntegerAttribute>(*pint_attr)->GetValue(), 42);
}

TEST(DictionaryAttribute, MoveAssignmentTest)
{
	DictionaryAttribute dictionary;
	dictionary.UpsertAttribute<IntegerAttribute>(SGE_STR("int"), 42);

	DictionaryAttribute dictionary2;
	dictionary2.UpsertAttribute<StringAttribute>(SGE_STR("str"), SGE_STR("Hello"));
	dictionary2 = std::move(dictionary);

	Attribute* pint_attr = dictionary2.GetAttribute(SGE_STR("int"));
	ASSERT_NE(pint_attr, nullptr);
	ASSERT_EQ(DynamicCast<IntegerAttribute>(*pint_attr)->GetValue(), 42);
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

TEST(DictionaryAttribute, VisitTest)
{
	DictionaryAttribute dictionary;
	SizeType empty_count = 0;
	dictionary.Visit([&](const String&, Attribute*) { empty_count += 1; });
	ASSERT_EQ(empty_count, 0);

	dictionary.UpsertAttribute<IntegerAttribute>(SGE_STR("int"), 42);
	dictionary.UpsertAttribute<StringAttribute>(SGE_STR("str"), SGE_STR("Hello"));

	SizeType count = 0;
	dictionary.Visit([&](const String& key, Attribute* pattr) {
		ASSERT_NE(pattr, nullptr);
		if (key == SGE_STR("int"))
		{
			IntegerAttribute* pint = DynamicCast<IntegerAttribute>(*pattr);
			ASSERT_NE(pint, nullptr);
			ASSERT_EQ(pint->GetValue(), 42);
			pint->SetValue(100);
		}
		else if (key == SGE_STR("str"))
		{
			StringAttribute* pstr = DynamicCast<StringAttribute>(*pattr);
			ASSERT_NE(pstr, nullptr);
			ASSERT_EQ(pstr->GetValue(), SGE_STR("Hello"));
		}
		else
			ASSERT_TRUE(false);
		count += 1;
	});
	ASSERT_EQ(count, 2);

	Attribute* pint_attr = dictionary.GetAttribute(SGE_STR("int"));
	ASSERT_NE(pint_attr, nullptr);
	ASSERT_EQ(DynamicCast<IntegerAttribute>(*pint_attr)->GetValue(), 100);
}

TEST(DictionaryAttribute, ConstVisitTest)
{
	DictionaryAttribute dictionary;
	const DictionaryAttribute& const_dictionary = dictionary;
	SizeType empty_count = 0;
	const_dictionary.Visit([&](const String&, const Attribute*) { empty_count += 1; });
	ASSERT_EQ(empty_count, 0);

	dictionary.UpsertAttribute<IntegerAttribute>(SGE_STR("int"), 42);
	dictionary.UpsertAttribute<StringAttribute>(SGE_STR("str"), SGE_STR("Hello"));

	SizeType count = 0;
	const_dictionary.Visit([&](const String& key, const Attribute* pattr) {
		ASSERT_NE(pattr, nullptr);
		if (key == SGE_STR("int"))
		{
			const IntegerAttribute* pint = DynamicCast<IntegerAttribute>(*pattr);
			ASSERT_NE(pint, nullptr);
			ASSERT_EQ(pint->GetValue(), 42);
		}
		else if (key == SGE_STR("str"))
		{
			const StringAttribute* pstr = DynamicCast<StringAttribute>(*pattr);
			ASSERT_NE(pstr, nullptr);
			ASSERT_EQ(pstr->GetValue(), SGE_STR("Hello"));
		}
		else
			ASSERT_TRUE(false);
		count += 1;
	});
	ASSERT_EQ(count, 2);
}

TEST(DictionaryAttribute, CloneTest)
{
	DictionaryAttribute dictionary;
	dictionary.UpsertAttribute<IntegerAttribute>(SGE_STR("int"), 42);
	dictionary.UpsertAttribute<StringAttribute>(SGE_STR("str"), SGE_STR("Hello"));
	dictionary.UpsertAttribute<DictionaryAttribute>(SGE_STR("dict"));
	DictionaryAttribute* pinner = DynamicCast<DictionaryAttribute>(*dictionary.GetAttribute(SGE_STR("dict")));
	ASSERT_NE(pinner, nullptr);
	pinner->UpsertAttribute<IntegerAttribute>(SGE_STR("int"), 42);

	Attribute* pclone = dictionary.Clone();
	ASSERT_NE(pclone, nullptr);
	ASSERT_NE(pclone, &dictionary);
	DictionaryAttribute* pdict = DynamicCast<DictionaryAttribute>(*pclone);
	ASSERT_NE(pdict, nullptr);

	SizeType count = 0;
	pdict->Visit([&](const String& key, Attribute* pattr) {
		ASSERT_NE(pattr, nullptr);
		if (key == SGE_STR("int"))
			ASSERT_EQ(DynamicCast<IntegerAttribute>(*pattr)->GetValue(), 42);
		else if (key == SGE_STR("str"))
			ASSERT_EQ(DynamicCast<StringAttribute>(*pattr)->GetValue(), SGE_STR("Hello"));
		else if (key == SGE_STR("dict"))
		{
			DictionaryAttribute* pinner2 = DynamicCast<DictionaryAttribute>(*pattr);
			ASSERT_NE(pinner2, nullptr);
			ASSERT_NE(pinner2, pinner);
			Attribute* pinner_int = pinner2->GetAttribute(SGE_STR("int"));
			ASSERT_NE(pinner_int, nullptr);
			ASSERT_NE(pinner_int, pinner->GetAttribute(SGE_STR("int")));
			ASSERT_EQ(DynamicCast<IntegerAttribute>(*pinner_int)->GetValue(), 42);
		}
		else
			ASSERT_TRUE(false);
		count += 1;
	});
	ASSERT_EQ(count, 3);

	DynamicCast<IntegerAttribute>(*(pdict->GetAttribute(SGE_STR("int"))))->SetValue(100);
	ASSERT_EQ(DynamicCast<IntegerAttribute>(*dictionary.GetAttribute(SGE_STR("int")))->GetValue(), 42);

	DefaultAllocator::Delete(pclone);
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

TEST(DictionaryAttribute, GetTypeIdTest)
{
	DictionaryAttribute dictionary;

	ASSERT_EQ(dictionary.GetTypeId(), GetTypeId<DictionaryAttribute>());
}