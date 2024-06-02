/*
Copyright 2024 creatorlxd

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
#include <iostream>
#include "Time/Date.h"
#include "gtest/gtest.h"

using namespace SpaceGameEngine;

TEST(Date, GetLocalDateTest)
{
	Date d = GetLocalDate();
	ASSERT_FALSE(InvalidDateError::Judge(d));
	std::cout << (unsigned int)d.m_Year << " " << (unsigned int)d.m_Month << " " << (unsigned int)d.m_Day << " " << (unsigned int)d.m_Hour << " " << (unsigned int)d.m_Minute << " " << (unsigned int)d.m_Second << std::endl;
}