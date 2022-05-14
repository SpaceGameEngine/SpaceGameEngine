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
#pragma once
#include "VirtualMachine/ExternalCaller.h"
#include "VirtualMachine/Assembler.h"

/*!
@ingroup SpaceLanguage
@{
*/

namespace SpaceGameEngine::SpaceLanguage::ExternalCallerModule
{

	namespace CoreModule
	{
		inline constexpr const UInt32 Id = 0;

		namespace Functions
		{
			inline constexpr const UInt32 Allocate = 0;
			inline constexpr const UInt32 Free = 1;
			inline constexpr const UInt32 Int8Load = 2;
			inline constexpr const UInt32 Int8Store = 3;
			inline constexpr const UInt32 Int8Copy = 4;
			inline constexpr const UInt32 Int8Add = 5;
			inline constexpr const UInt32 Int8Subtract = 6;
			inline constexpr const UInt32 Int8Multiply = 7;
			inline constexpr const UInt32 Int8Divide = 8;
			inline constexpr const UInt32 Int8Mod = 9;
			inline constexpr const UInt32 Int8And = 10;
			inline constexpr const UInt32 Int8Or = 11;
			inline constexpr const UInt32 Int8Xor = 12;
			inline constexpr const UInt32 Int8Not = 13;
			inline constexpr const UInt32 Int8ShiftLeft = 14;
			inline constexpr const UInt32 Int8ShiftRight = 15;
			inline constexpr const UInt32 Int8Equal = 16;
			inline constexpr const UInt32 Int8NotEqual = 17;
			inline constexpr const UInt32 Int8Less = 18;
			inline constexpr const UInt32 Int8LessEqual = 19;
			inline constexpr const UInt32 Int8Greater = 20;
			inline constexpr const UInt32 Int8GreaterEqual = 21;
			inline constexpr const UInt32 UInt8Load = 22;
			inline constexpr const UInt32 UInt8Store = 23;
			inline constexpr const UInt32 UInt8Copy = 24;
			inline constexpr const UInt32 UInt8Add = 25;
			inline constexpr const UInt32 UInt8Subtract = 26;
			inline constexpr const UInt32 UInt8Multiply = 27;
			inline constexpr const UInt32 UInt8Divide = 28;
			inline constexpr const UInt32 UInt8Mod = 29;
			inline constexpr const UInt32 UInt8And = 30;
			inline constexpr const UInt32 UInt8Or = 31;
			inline constexpr const UInt32 UInt8Xor = 32;
			inline constexpr const UInt32 UInt8Not = 33;
			inline constexpr const UInt32 UInt8ShiftLeft = 34;
			inline constexpr const UInt32 UInt8ShiftRight = 35;
			inline constexpr const UInt32 UInt8Equal = 36;
			inline constexpr const UInt32 UInt8NotEqual = 37;
			inline constexpr const UInt32 UInt8Less = 38;
			inline constexpr const UInt32 UInt8LessEqual = 39;
			inline constexpr const UInt32 UInt8Greater = 40;
			inline constexpr const UInt32 UInt8GreaterEqual = 41;
			inline constexpr const UInt32 Int16Load = 42;
			inline constexpr const UInt32 Int16Store = 43;
			inline constexpr const UInt32 Int16Copy = 44;
			inline constexpr const UInt32 Int16Add = 45;
			inline constexpr const UInt32 Int16Subtract = 46;
			inline constexpr const UInt32 Int16Multiply = 47;
			inline constexpr const UInt32 Int16Divide = 48;
			inline constexpr const UInt32 Int16Mod = 49;
			inline constexpr const UInt32 Int16And = 50;
			inline constexpr const UInt32 Int16Or = 51;
			inline constexpr const UInt32 Int16Xor = 52;
			inline constexpr const UInt32 Int16Not = 53;
			inline constexpr const UInt32 Int16ShiftLeft = 54;
			inline constexpr const UInt32 Int16ShiftRight = 55;
			inline constexpr const UInt32 Int16Equal = 56;
			inline constexpr const UInt32 Int16NotEqual = 57;
			inline constexpr const UInt32 Int16Less = 58;
			inline constexpr const UInt32 Int16LessEqual = 59;
			inline constexpr const UInt32 Int16Greater = 60;
			inline constexpr const UInt32 Int16GreaterEqual = 61;
			inline constexpr const UInt32 UInt16Load = 62;
			inline constexpr const UInt32 UInt16Store = 63;
			inline constexpr const UInt32 UInt16Copy = 64;
			inline constexpr const UInt32 UInt16Add = 65;
			inline constexpr const UInt32 UInt16Subtract = 66;
			inline constexpr const UInt32 UInt16Multiply = 67;
			inline constexpr const UInt32 UInt16Divide = 68;
			inline constexpr const UInt32 UInt16Mod = 69;
			inline constexpr const UInt32 UInt16And = 70;
			inline constexpr const UInt32 UInt16Or = 71;
			inline constexpr const UInt32 UInt16Xor = 72;
			inline constexpr const UInt32 UInt16Not = 73;
			inline constexpr const UInt32 UInt16ShiftLeft = 74;
			inline constexpr const UInt32 UInt16ShiftRight = 75;
			inline constexpr const UInt32 UInt16Equal = 76;
			inline constexpr const UInt32 UInt16NotEqual = 77;
			inline constexpr const UInt32 UInt16Less = 78;
			inline constexpr const UInt32 UInt16LessEqual = 79;
			inline constexpr const UInt32 UInt16Greater = 80;
			inline constexpr const UInt32 UInt16GreaterEqual = 81;
			inline constexpr const UInt32 Int32Load = 82;
			inline constexpr const UInt32 Int32Store = 83;
			inline constexpr const UInt32 Int32Copy = 84;
			inline constexpr const UInt32 Int32Add = 85;
			inline constexpr const UInt32 Int32Subtract = 86;
			inline constexpr const UInt32 Int32Multiply = 87;
			inline constexpr const UInt32 Int32Divide = 88;
			inline constexpr const UInt32 Int32Mod = 89;
			inline constexpr const UInt32 Int32And = 90;
			inline constexpr const UInt32 Int32Or = 91;
			inline constexpr const UInt32 Int32Xor = 92;
			inline constexpr const UInt32 Int32Not = 93;
			inline constexpr const UInt32 Int32ShiftLeft = 94;
			inline constexpr const UInt32 Int32ShiftRight = 95;
			inline constexpr const UInt32 Int32Equal = 96;
			inline constexpr const UInt32 Int32NotEqual = 97;
			inline constexpr const UInt32 Int32Less = 98;
			inline constexpr const UInt32 Int32LessEqual = 99;
			inline constexpr const UInt32 Int32Greater = 100;
			inline constexpr const UInt32 Int32GreaterEqual = 101;
			inline constexpr const UInt32 UInt32Load = 102;
			inline constexpr const UInt32 UInt32Store = 103;
			inline constexpr const UInt32 UInt32Copy = 104;
			inline constexpr const UInt32 UInt32Add = 105;
			inline constexpr const UInt32 UInt32Subtract = 106;
			inline constexpr const UInt32 UInt32Multiply = 107;
			inline constexpr const UInt32 UInt32Divide = 108;
			inline constexpr const UInt32 UInt32Mod = 109;
			inline constexpr const UInt32 UInt32And = 110;
			inline constexpr const UInt32 UInt32Or = 111;
			inline constexpr const UInt32 UInt32Xor = 112;
			inline constexpr const UInt32 UInt32Not = 113;
			inline constexpr const UInt32 UInt32ShiftLeft = 114;
			inline constexpr const UInt32 UInt32ShiftRight = 115;
			inline constexpr const UInt32 UInt32Equal = 116;
			inline constexpr const UInt32 UInt32NotEqual = 117;
			inline constexpr const UInt32 UInt32Less = 118;
			inline constexpr const UInt32 UInt32LessEqual = 119;
			inline constexpr const UInt32 UInt32Greater = 120;
			inline constexpr const UInt32 UInt32GreaterEqual = 121;
			inline constexpr const UInt32 Int64Load = 122;
			inline constexpr const UInt32 Int64Store = 123;
			inline constexpr const UInt32 Int64Copy = 124;
			inline constexpr const UInt32 Int64Add = 125;
			inline constexpr const UInt32 Int64Subtract = 126;
			inline constexpr const UInt32 Int64Multiply = 127;
			inline constexpr const UInt32 Int64Divide = 128;
			inline constexpr const UInt32 Int64Mod = 129;
			inline constexpr const UInt32 Int64And = 130;
			inline constexpr const UInt32 Int64Or = 131;
			inline constexpr const UInt32 Int64Xor = 132;
			inline constexpr const UInt32 Int64Not = 133;
			inline constexpr const UInt32 Int64ShiftLeft = 134;
			inline constexpr const UInt32 Int64ShiftRight = 135;
			inline constexpr const UInt32 Int64Equal = 136;
			inline constexpr const UInt32 Int64NotEqual = 137;
			inline constexpr const UInt32 Int64Less = 138;
			inline constexpr const UInt32 Int64LessEqual = 139;
			inline constexpr const UInt32 Int64Greater = 140;
			inline constexpr const UInt32 Int64GreaterEqual = 141;
			inline constexpr const UInt32 UInt64Load = 142;
			inline constexpr const UInt32 UInt64Store = 143;
			inline constexpr const UInt32 UInt64Copy = 144;
			inline constexpr const UInt32 UInt64Add = 145;
			inline constexpr const UInt32 UInt64Subtract = 146;
			inline constexpr const UInt32 UInt64Multiply = 147;
			inline constexpr const UInt32 UInt64Divide = 148;
			inline constexpr const UInt32 UInt64Mod = 149;
			inline constexpr const UInt32 UInt64And = 150;
			inline constexpr const UInt32 UInt64Or = 151;
			inline constexpr const UInt32 UInt64Xor = 152;
			inline constexpr const UInt32 UInt64Not = 153;
			inline constexpr const UInt32 UInt64ShiftLeft = 154;
			inline constexpr const UInt32 UInt64ShiftRight = 155;
			inline constexpr const UInt32 UInt64Equal = 156;
			inline constexpr const UInt32 UInt64NotEqual = 157;
			inline constexpr const UInt32 UInt64Less = 158;
			inline constexpr const UInt32 UInt64LessEqual = 159;
			inline constexpr const UInt32 UInt64Greater = 160;
			inline constexpr const UInt32 UInt64GreaterEqual = 161;
			inline constexpr const UInt32 FloatLoad = 162;
			inline constexpr const UInt32 FloatStore = 163;
			inline constexpr const UInt32 FloatCopy = 164;
			inline constexpr const UInt32 FloatAdd = 165;
			inline constexpr const UInt32 FloatSubtract = 166;
			inline constexpr const UInt32 FloatMultiply = 167;
			inline constexpr const UInt32 FloatDivide = 168;
			inline constexpr const UInt32 FloatEqual = 169;
			inline constexpr const UInt32 FloatNotEqual = 170;
			inline constexpr const UInt32 FloatLess = 171;
			inline constexpr const UInt32 FloatLessEqual = 172;
			inline constexpr const UInt32 FloatGreater = 173;
			inline constexpr const UInt32 FloatGreaterEqual = 174;
			inline constexpr const UInt32 DoubleLoad = 175;
			inline constexpr const UInt32 DoubleStore = 176;
			inline constexpr const UInt32 DoubleCopy = 177;
			inline constexpr const UInt32 DoubleAdd = 178;
			inline constexpr const UInt32 DoubleSubtract = 179;
			inline constexpr const UInt32 DoubleMultiply = 180;
			inline constexpr const UInt32 DoubleDivide = 181;
			inline constexpr const UInt32 DoubleEqual = 182;
			inline constexpr const UInt32 DoubleNotEqual = 183;
			inline constexpr const UInt32 DoubleLess = 184;
			inline constexpr const UInt32 DoubleLessEqual = 185;
			inline constexpr const UInt32 DoubleGreater = 186;
			inline constexpr const UInt32 DoubleGreaterEqual = 187;
		}

		SPACE_LANGUAGE_API void LoadIntoExternalCaller(ExternalCaller& ec);
		SPACE_LANGUAGE_API void LoadIntoAssembler(Assembler& assembler);
	}

}

/*!
@}
*/