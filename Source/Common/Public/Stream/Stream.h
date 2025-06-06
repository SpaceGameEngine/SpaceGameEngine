/*
Copyright 2025 creatorlxd

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
#include "CommonAPI.h"
#include "TypeDefinition.hpp"
#include "Utility/MemoryData.h"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	class COMMON_API InputStream
	{
	public:
		virtual ~InputStream() = default;

		/*!
		@brief Read data from the stream into the provided MemoryData object.
		@note The MemoryData object should be pre-allocated, and the InputStream will fill it with data.
		@param data The MemoryData object to read data into.
		@return True if the read operation was successful, false otherwise.
		*/
		virtual bool Read(MemoryData& data) = 0;
	};

	class COMMON_API OutputStream
	{
	public:
		virtual ~OutputStream() = default;

		/*!
		@brief Write data from the provided MemoryData object to the stream.
		@param data The MemoryData object containing the data to write.
		@return True if the write operation was successful, false otherwise.
		*/
		virtual bool Write(const MemoryData& data) = 0;
	};

	struct InvalidMemoryDataForInputStreamReadError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The MemoryData is invalid for InputStream::Read.");
		static COMMON_API bool Judge(const MemoryData& data);
	};
}

/*!
@}
*/