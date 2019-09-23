/*
Copyright 2019 creatorlxd

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
#include "MetaData/MetaData.h"
#include "MetaData/MetaObject.hpp"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

struct test_metadata_class
{
	test_metadata_class() { i = 1; }
	~test_metadata_class() { i = 0; }
	test_metadata_class( const test_metadata_class& obj ) { i = 2; }
	test_metadata_class( test_metadata_class&& obj ) { i = 3; }

	test_metadata_class& operator=( const test_metadata_class& obj )
	{
		i = 4;
		return *this;
	}
	test_metadata_class& operator=( test_metadata_class&& obj )
	{
		i = 5;
		return *this;
	}

	int i;
};

bool operator==( const test_metadata_class& t1, const test_metadata_class& t2 ) { return t1.i == t2.i; }

TEST( MetaData, GetMetaDataTest )
{
	decltype( auto ) test = GetMetaData<int>();
	ASSERT_STREQ( test.m_pName, typeid( int ).name() );
	ASSERT_EQ( test.m_Size, sizeof( int ) );
	ASSERT_EQ( test.m_Alignment, alignof( int ) );
	ASSERT_NE( test.m_pDefaultConstructor, nullptr );
	ASSERT_NE( test.m_pCopyConstructor, nullptr );
	ASSERT_NE( test.m_pMoveConstructor, nullptr );
	ASSERT_NE( test.m_pCopyAssignment, nullptr );
	ASSERT_NE( test.m_pMoveAssignment, nullptr );
	ASSERT_NE( test.m_pDestructor, nullptr );
	ASSERT_NE( test.m_pComparison, nullptr );
}

TEST( MetaData, MDFunctionsTest )
{
	decltype( auto ) test = GetMetaData<test_metadata_class>();
	test_metadata_class t1, t2;
	t1.i = 10;
	test.m_pDefaultConstructor( &t1 );
	ASSERT_EQ( t1.i, 1 );
	test.m_pCopyConstructor( &t1, &t2 );
	ASSERT_EQ( t1.i, 2 );
	test.m_pMoveConstructor( &t1, &t2 );
	ASSERT_EQ( t1.i, 3 );
	test.m_pCopyAssignment( &t1, &t2 );
	ASSERT_EQ( t1.i, 4 );
	test.m_pMoveAssignment( &t1, &t2 );
	;
	ASSERT_EQ( t1.i, 5 );
	t1.i = t2.i;
	ASSERT_TRUE( test.m_pComparison( &t1, &t2 ) );
	t1.i = 10;
	ASSERT_FALSE( test.m_pComparison( &t1, &t2 ) );
}

TEST( MetaData, ComparisionTest )
{
	MetaData test1 = GetMetaData<int>();
	MetaData test2 = test1;
	MetaData test3 = GetMetaData<char>();
	ASSERT_TRUE( test1 == test2 );
	ASSERT_FALSE( test1 == test3 );
}

TEST( MetaObject, CopyConstructionTest )
{
	MetaObject test( GetMetaData<test_metadata_class>() );
	MetaObject test2( test );
	ASSERT_EQ( test2.Get<test_metadata_class>().i, 2 );
	MetaObject test3( std::move( test2 ) );
	ASSERT_EQ( test3.Get<test_metadata_class>().i, 3 );
	test2 = test;
	ASSERT_EQ( test2.Get<test_metadata_class>().i, 4 );
	test2 = std::move( test3 );
	ASSERT_EQ( test2.Get<test_metadata_class>().i, 5 );
	test_metadata_class _test;
	MetaObject test4( GetMetaData<test_metadata_class>(), _test );
	ASSERT_EQ( test4.Get<test_metadata_class>().i, 2 );
	MetaObject test5( GetMetaData<test_metadata_class>(), std::move( _test ) );
	ASSERT_EQ( test5.Get<test_metadata_class>().i, 3 );
}

TEST( MetaObject, GetMetaDataTest )
{
	MetaObject test( GetMetaData<test_metadata_class>() );
	ASSERT_EQ( test.GetMetaData(), GetMetaData<test_metadata_class>() );
}

TEST( MetaObject, GetDataTest )
{
	MetaObject test( GetMetaData<test_metadata_class>() );
	int i1 = 1;
	ASSERT_EQ( memcmp( test.GetData(), &i1, sizeof( i1 ) ), 0 );
	ASSERT_TRUE( (std::is_same_v<decltype( std::declval<MetaObject<>>().GetData() ), void*>) );
	ASSERT_TRUE( (std::is_same_v<decltype( std::declval<const MetaObject<>>().GetData() ), const void*>) );
}

TEST( MetaObject, GetTest )
{
	MetaObject test( GetMetaData<test_metadata_class>() );
	ASSERT_EQ( test.Get<test_metadata_class>().i, 1 );
	ASSERT_TRUE( (std::is_same_v<decltype( std::declval<MetaObject<>>().Get<int>() ), int&>) );
	ASSERT_TRUE( (std::is_same_v<decltype( std::declval<const MetaObject<>>().Get<int>() ), const int&>) );
}

TEST( MetaObject, ComparisionTest )
{
	MetaObject test( GetMetaData<test_metadata_class>() );
	MetaObject test2( GetMetaData<test_metadata_class>() );
	ASSERT_TRUE( test == test2 );
}