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
#include <cstdint>
#include "Platform.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	/*!
	@brief 8-bit int
	*/
	using Int8 = int8_t;

	/*!
	@brief 8-bit unsigned int
	*/
	using UInt8 = uint8_t;

	/*!
	@brief 16-bit int
	*/
	using Int16 = int16_t;

	/*!
	@brief 16-bit unsigned int
	*/
	using UInt16 = uint16_t;

	/*!
	@brief 32-bit int
	*/
	using Int32 = int32_t;

	/*!
	@brief 32-bit unsigned int
	*/
	using UInt32 = uint32_t;

	/*!
	@brief 64-bit int
	*/
	using Int64 = int64_t;

	/*!
	@brief 64-bit unsigned int
	*/
	using UInt64 = uint64_t;

	/*!
	@brief a byte
	*/
	using Byte = UInt8;

	/*!
	@brief size type
	*/
	using SizeType = UInt64;

	/*!
	@brief address type
	*/
#ifdef SGE_X64
	using AddressType = UInt64;
#elif defined(SGE_X86)
	using AddressType = UInt32;
#else
#error this arch has not been supported.
#endif

	/*!
	@}
	*/
}