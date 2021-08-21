/*
Copyright 2021 creatorlxd

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
#include "gtest/gtest.h"
#include "Module/Module.h"
#include "Module/ModuleManager.h"

using namespace SpaceGameEngine;

class test_module_1 : public Module
{
public:
	inline static int sm_load_count = 0;
	inline static int sm_instance_count = 0;

	inline test_module_1()
		: Module(SGE_STR("test_module_1"))
	{
		sm_instance_count += 1;
	}

	inline virtual ~test_module_1()
	{
		sm_instance_count -= 1;
	}

	inline virtual void OnLoad() override
	{
		sm_load_count += 1;
	}

	inline virtual void OnUnload() override
	{
		sm_load_count -= 1;
	}
};

TEST(Module, InstanceTest)
{
	ASSERT_EQ(test_module_1::sm_instance_count, 0);
	Module* pmod = DefaultAllocator::New<test_module_1>();
	ASSERT_EQ(test_module_1::sm_instance_count, 1);

	ASSERT_EQ(pmod->GetName(), SGE_STR("test_module_1"));
	ASSERT_EQ(test_module_1::sm_load_count, 0);
	pmod->OnLoad();
	ASSERT_EQ(test_module_1::sm_load_count, 1);
	pmod->OnUnload();
	ASSERT_EQ(test_module_1::sm_load_count, 0);

	DefaultAllocator::Delete(pmod);
	ASSERT_EQ(test_module_1::sm_instance_count, 0);
}