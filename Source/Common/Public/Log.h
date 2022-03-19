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
#include "CommonAPI.h"
#include "Utility/Utility.hpp"
#include "Concurrent/Atomic.hpp"
#include "Concurrent/Thread.h"
#include "Concurrent/Lock.h"
#include "SGEStringForward.h"
#include "Utility/FixedSizeBuffer.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	COMMON_API void WriteLogToConsole(const Char8* ptr, SizeType size);

	class COMMON_API LogWriter : public UncopyableAndUnmovable
	{
	public:
		LogWriter(void (*write_func)(const Char8*, SizeType) = WriteLogToConsole);
		~LogWriter();

	private:
		void Run();

	private:
		void (*m_WriteFunction)(const Char8*, SizeType);
		Atomic<bool> m_IsRunning;
		Thread m_Thread;
	};

	/*!
	@}
	*/
}