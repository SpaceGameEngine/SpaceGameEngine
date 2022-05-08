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
#define CAST_MEM_R_ARG(type, x) (*((type*)(R_ARG(x))))

#define TYPE_LOAD(type, type_name)                                                                                           \
	ec.AddExternalCallFunction(CoreModule::Id, CoreModule::Functions::type_name##Load, [](Registers& regs) -> RegisterType { \
		return static_cast<RegisterType>(CAST_MEM_R_ARG(type, 0));                                                           \
	});

#define TYPE_STORE(type, type_name)                                                                                           \
	ec.AddExternalCallFunction(CoreModule::Id, CoreModule::Functions::type_name##Store, [](Registers& regs) -> RegisterType { \
		CAST_MEM_R_ARG(type, 0) = static_cast<type>(R_ARG(1));                                                                \
		return 0;                                                                                                             \
	});

#define TYPE_COPY(type, type_name)                                                                                           \
	ec.AddExternalCallFunction(CoreModule::Id, CoreModule::Functions::type_name##Copy, [](Registers& regs) -> RegisterType { \
		CAST_MEM_R_ARG(type, 0) = CAST_MEM_R_ARG(type, 1);                                                                   \
		return 0;                                                                                                            \
	});

#define TYPE_OPERATOR(type, type_name, oper, oper_name)                                                                           \
	ec.AddExternalCallFunction(CoreModule::Id, CoreModule::Functions::type_name##oper_name, [](Registers& regs) -> RegisterType { \
		CAST_MEM_R_ARG(type, 0) = CAST_MEM_R_ARG(type, 1) oper CAST_MEM_R_ARG(type, 2);                                           \
		return 0;                                                                                                                 \
	});

#define TYPE_NOT_OPERATOR(type, type_name)                                                                                  \
	ec.AddExternalCallFunction(CoreModule::Id, CoreModule::Functions::type_name##Not, [](Registers& regs) -> RegisterType { \
		CAST_MEM_R_ARG(type, 0) = ~CAST_MEM_R_ARG(type, 1);                                                                 \
		return 0;                                                                                                           \
	});

#define TYPE_JUDGE_OPERATOR(type, type_name, oper, oper_name)                                                                     \
	ec.AddExternalCallFunction(CoreModule::Id, CoreModule::Functions::type_name##oper_name, [](Registers& regs) -> RegisterType { \
		return (CAST_MEM_R_ARG(type, 0) oper CAST_MEM_R_ARG(type, 1)) ? 1 : 0;                                                    \
	});

#define BASE_TYPE_MATH_OPERATORS(type, type_name) \
	TYPE_OPERATOR(type, type_name, +, Add)        \
	TYPE_OPERATOR(type, type_name, -, Subtract)   \
	TYPE_OPERATOR(type, type_name, *, Multiply)   \
	TYPE_OPERATOR(type, type_name, /, Divide)

#define INTEGER_TYPE_MATH_OPERATORS(type, type_name) \
	BASE_TYPE_MATH_OPERATORS(type, type_name)        \
	TYPE_OPERATOR(type, type_name, %, Mod)           \
	TYPE_OPERATOR(type, type_name, &, And)           \
	TYPE_OPERATOR(type, type_name, |, Or)            \
	TYPE_OPERATOR(type, type_name, ^, Xor)           \
	TYPE_NOT_OPERATOR(type, type_name)               \
	TYPE_OPERATOR(type, type_name, <<, ShiftLeft)    \
	TYPE_OPERATOR(type, type_name, >>, ShiftRight)

#define TYPE_JUDGE_OPERATORS(type, type_name)           \
	TYPE_JUDGE_OPERATOR(type, type_name, ==, Equal)     \
	TYPE_JUDGE_OPERATOR(type, type_name, !=, NotEqual)  \
	TYPE_JUDGE_OPERATOR(type, type_name, <, Less)       \
	TYPE_JUDGE_OPERATOR(type, type_name, <=, LessEqual) \
	TYPE_JUDGE_OPERATOR(type, type_name, >, Greater)    \
	TYPE_JUDGE_OPERATOR(type, type_name, >=, GreaterEqual)

#define INTEGER_TYPE_FUNCTIONS(type, type_name)  \
	TYPE_LOAD(type, type_name)                   \
	TYPE_STORE(type, type_name)                  \
	TYPE_COPY(type, type_name)                   \
	INTEGER_TYPE_MATH_OPERATORS(type, type_name) \
	TYPE_JUDGE_OPERATORS(type, type_name)

#define DECIMAL_TYPE_FUNCTIONS(type, type_name) \
	TYPE_LOAD(type, type_name)                  \
	TYPE_STORE(type, type_name)                 \
	TYPE_COPY(type, type_name)                  \
	BASE_TYPE_MATH_OPERATORS(type, type_name)   \
	TYPE_JUDGE_OPERATORS(type, type_name)

void SpaceGameEngine::SpaceLanguage::ExternalCallerModule::CoreModule::LoadIntoExternalCaller(ExternalCaller& ec)
{
	//Allocate
	ec.AddExternalCallFunction(CoreModule::Id, CoreModule::Functions::Allocate, [](Registers& regs) -> RegisterType {
		return (RegisterType)DefaultAllocator::RawNew(R_ARG(0), R_ARG(1));
	});

	//Free
	ec.AddExternalCallFunction(CoreModule::Id, CoreModule::Functions::Free, [](Registers& regs) -> RegisterType {
		DefaultAllocator::RawDelete((void*)R_ARG(0));
		return 0;
	});

	INTEGER_TYPE_FUNCTIONS(Int8, Int8);
	INTEGER_TYPE_FUNCTIONS(UInt8, UInt8);
	INTEGER_TYPE_FUNCTIONS(Int16, Int16);
	INTEGER_TYPE_FUNCTIONS(UInt16, UInt16);
	INTEGER_TYPE_FUNCTIONS(Int32, Int32);
	INTEGER_TYPE_FUNCTIONS(UInt32, UInt32);
	INTEGER_TYPE_FUNCTIONS(Int64, Int64);
	INTEGER_TYPE_FUNCTIONS(UInt64, UInt64);
	DECIMAL_TYPE_FUNCTIONS(float, Float);
	DECIMAL_TYPE_FUNCTIONS(double, Double);
}

#define ASSEMBLER_FUNCTION(func_name) Pair<const String, UInt32>(SGE_STR(#func_name), CoreModule::Functions::func_name)

#define ASSEMBLER_BASE_TYPE_MATH_OPERATORS(type_name) \
	ASSEMBLER_FUNCTION(type_name##Add),               \
		ASSEMBLER_FUNCTION(type_name##Subtract),      \
		ASSEMBLER_FUNCTION(type_name##Multiply),      \
		ASSEMBLER_FUNCTION(type_name##Divide)

#define ASSEMBLER_INTEGER_TYPE_MATH_OPERATORS(type_name) \
	ASSEMBLER_BASE_TYPE_MATH_OPERATORS(type_name),       \
		ASSEMBLER_FUNCTION(type_name##Mod),              \
		ASSEMBLER_FUNCTION(type_name##And),              \
		ASSEMBLER_FUNCTION(type_name##Or),               \
		ASSEMBLER_FUNCTION(type_name##Xor),              \
		ASSEMBLER_FUNCTION(type_name##Not),              \
		ASSEMBLER_FUNCTION(type_name##ShiftLeft),        \
		ASSEMBLER_FUNCTION(type_name##ShiftRight)

#define ASSEMBLER_TYPE_JUDGE_OPERATORS(type_name) \
	ASSEMBLER_FUNCTION(type_name##Equal),         \
		ASSEMBLER_FUNCTION(type_name##NotEqual),  \
		ASSEMBLER_FUNCTION(type_name##Less),      \
		ASSEMBLER_FUNCTION(type_name##LessEqual), \
		ASSEMBLER_FUNCTION(type_name##Greater),   \
		ASSEMBLER_FUNCTION(type_name##GreaterEqual)

#define ASSEMBLER_INTEGER_TYPE_FUNCTIONS(type_name)       \
	ASSEMBLER_FUNCTION(type_name##Load),                  \
		ASSEMBLER_FUNCTION(type_name##Store),             \
		ASSEMBLER_FUNCTION(type_name##Copy),              \
		ASSEMBLER_INTEGER_TYPE_MATH_OPERATORS(type_name), \
		ASSEMBLER_TYPE_JUDGE_OPERATORS(type_name)

#define ASSEMBLER_DECIMAL_TYPE_FUNCTIONS(type_name)    \
	ASSEMBLER_FUNCTION(type_name##Load),               \
		ASSEMBLER_FUNCTION(type_name##Store),          \
		ASSEMBLER_FUNCTION(type_name##Copy),           \
		ASSEMBLER_BASE_TYPE_MATH_OPERATORS(type_name), \
		ASSEMBLER_TYPE_JUDGE_OPERATORS(type_name)

void SpaceGameEngine::SpaceLanguage::ExternalCallerModule::CoreModule::LoadIntoAssembler(Assembler& assembler)
{
	assembler.RegisterExternalCallerModule(SGE_STR("CoreModule"), CoreModule::Id, {
																					  ASSEMBLER_FUNCTION(Allocate),
																					  ASSEMBLER_FUNCTION(Free),
																					  ASSEMBLER_INTEGER_TYPE_FUNCTIONS(Int8),
																					  ASSEMBLER_INTEGER_TYPE_FUNCTIONS(UInt8),
																					  ASSEMBLER_INTEGER_TYPE_FUNCTIONS(Int16),
																					  ASSEMBLER_INTEGER_TYPE_FUNCTIONS(UInt16),
																					  ASSEMBLER_INTEGER_TYPE_FUNCTIONS(Int32),
																					  ASSEMBLER_INTEGER_TYPE_FUNCTIONS(UInt32),
																					  ASSEMBLER_INTEGER_TYPE_FUNCTIONS(Int64),
																					  ASSEMBLER_INTEGER_TYPE_FUNCTIONS(UInt64),
																					  ASSEMBLER_DECIMAL_TYPE_FUNCTIONS(Float),
																					  ASSEMBLER_DECIMAL_TYPE_FUNCTIONS(Double),
																				  });
}
