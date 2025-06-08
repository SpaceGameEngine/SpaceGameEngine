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
#include "Stream/Stream.h"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	/*!
	@brief ConsoleStream is a stream that reads from and writes to the console.
	@note When reading, it will block until input is available.
	*/
	class COMMON_API ConsoleStream : public InputStream, public OutputStream
	{
	public:
		virtual bool Read(MemoryData& data) override;
		virtual bool Write(const MemoryData& data) override;
	};
}

/*!
@}
*/