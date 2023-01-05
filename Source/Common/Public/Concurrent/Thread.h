/*
Copyright 2023 creatorlxd

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

#include "TypeDefinition.hpp"
#include "Time/TimeCounter.h"
#include "CommonAPI.h"
#include <functional>
#include <thread>
#include <atomic>

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
namespace std	 // declaration for dll export
{
	class COMMON_API thread;
}
#endif

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{

	/*!
	@file
	@todo use our own thread instead of std's.
	*/

	using ThreadID = std::thread::id;

	/*!\brief Class SpaceGameEngine::Thread is a RAII wrapper of a std::thread.
	 *
	 * Class SpaceGameEngine::Thread has the most functions of std::thread, which are renamed to match the naming
	 * standard. Static functions of std::thread and from namespace std::this_thread are included too. Check out
	 * c++ thread reference for more details.
	 *
	 * However, there ARE some noticeable differences between SGE::Thread and std::thread:
	 * 1. Thread is a RAII wrapper. That means it initialize a std::thread when being constructed, and detach the
	 * std::thread (if possible) when being deconstructed.
	 * 2. operator=() has been deleted.
	 * 3. SpaceGameEngine::Thread::Detach() will check the joinable state before invoking std::thread::detach() to
	 * avoid crash.
	 *
	 * \todo Add interruption support
	 */
	class COMMON_API Thread
	{
	public:
		Thread();

		Thread(Thread&&) noexcept;

		template<class Func, class... Args>
		explicit Thread(Func&& f, Args&&... args)
			: m_ThreadImpl(std::forward<Func>(f),
						   std::forward<Args>(args)...)
		{
		}

		Thread(const Thread&) = delete;

		~Thread();

		Thread& operator=(Thread&&) noexcept;

		Thread& operator=(const Thread&) = delete;

		bool IsJoinable() const noexcept;

		void Join();

		void Detach();

		void Swap(Thread& other) noexcept;

		ThreadID GetThreadID() const noexcept;

		/*!
		@brief Query the hardware concurrency of the machine, may return 0 if failed to get this value.
		*/
		static SizeType QueryHardwareConcurrency() noexcept;

		static void YieldCurrentThread() noexcept;

		static ThreadID GetCurrentThreadId() noexcept;

		template<UInt64 TimeUnit, typename T>
		static void Sleep(const TimeDuration<TimeUnit, T>& sleep_duration)
		{
			SleepFor(sleep_duration);
		}

	private:
		std::thread m_ThreadImpl;
	};

	COMMON_API void Swap(SpaceGameEngine::Thread& lhs, SpaceGameEngine::Thread& rhs) noexcept;

}

/*!
@}
*/