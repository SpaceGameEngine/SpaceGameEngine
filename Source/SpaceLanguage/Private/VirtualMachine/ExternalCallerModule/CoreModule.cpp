/*
Copyright 2022 creatorlxd

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
#include "VirtualMachine/ExternalCallerModule/CoreModule.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;
using namespace SpaceGameEngine::SpaceLanguage::ExternalCallerModule;

#define R_ARG(x) regs.Get(Register::Argument(x))

void SpaceGameEngine::SpaceLanguage::ExternalCallerModule::CoreModule::LoadIntoExternalCaller(ExternalCaller& ec)
{
	//Allocate
	ec.AddExternalCallFunction(CoreModule::Id, 0, [](Registers& regs) -> RegisterType {
		return (RegisterType)DefaultAllocator::RawNew(R_ARG(0), R_ARG(1));
	});

	//Free
	ec.AddExternalCallFunction(CoreModule::Id, 1, [](Registers& regs) -> RegisterType {
		DefaultAllocator::RawDelete((void*)R_ARG(0));
		return 0;
	});
}

void SpaceGameEngine::SpaceLanguage::ExternalCallerModule::CoreModule::LoadIntoAssembler(Assembler& assembler)
{
	assembler.RegisterExternalCallerModule(SGE_STR("CoreModule"), CoreModule::Id, {
																					  Pair<const String, UInt32>(SGE_STR("Allocate"), 0),
																					  Pair<const String, UInt32>(SGE_STR("Free"), 1),
																				  });
}
