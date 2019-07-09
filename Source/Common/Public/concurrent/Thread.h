/*
Copyright 2019 Chenxi Xu (@xsun2001)

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

#ifndef SPACEGAMEENGINE_THREAD_H
#define SPACEGAMEENGINE_THREAD_H

#include "TypeDefination.hpp"

#include <functional>
#include <thread>
#include <atomic>

namespace SpaceGameEngine
{
	using ThreadID = std::thread::id;

	/*!\brief Class SpaceGameEngine::Thread is a RAII manager of a std::thread.
	 *
	 * Class SpaceGameEngine::Thread has the most functions of std::thread, which are renamed to match the naming
	 * standard. Static functions of std::thread and from namespace std::this_thread are included too. Check out
	 * c++ thread reference for more details.
	 *
	 * However, there ARE some noticeable differences between SGE::Thread and std::thread:
	 * 1. Thread is a RAII manager. That means it initialize a std::thread when being constructed, and detach the
	 * std::thread (if possible) when being deconstructed.
	 * 2. operator=() has been deleted.
	 * 3. SpaceGameEngine::Thread::Detach() will check the joinable state before invoking std::thread::detach() to
	 * avoid crash.
	 *
	 * \todo Add interruption support
	 */
	class Thread
	{
	public:
		template<class Function, class... Args>
		explicit Thread( Function &&f, Args &&... args ) : internal( f, args... ) {}

		~Thread()
		{
			if (internal.joinable())
			{
				internal.detach();
			}
		}

		Thread &operator=( Thread &&other ) noexcept = delete;

		bool isJoinable() const noexcept
		{
			return internal.joinable();
		}

		void Join()
		{
			internal.join();
		}

		void Detach()
		{
			if (internal.joinable())
			{
				internal.detach();
			}
		}

		void Swap( Thread &other ) noexcept
		{
			std::swap( internal, other.internal );
		}

		ThreadID GetThreadID() const noexcept
		{
			return internal.get_id();
		}

		static UInt32 HardwareConcurrency() noexcept
		{
			return std::thread::hardware_concurrency();
		}

		static void Yield() noexcept
		{
			std::this_thread::yield();
		}

		static ThreadID GetCurrentThreadID() noexcept
		{
			return std::this_thread::get_id();
		}

		template<class Rep, class Period>
		static void Sleep( const std::chrono::duration<Rep, Period> &sleep_duration )
		{
			std::this_thread::sleep_for( sleep_duration );
		}

	private:
		std::thread internal;
	};

}

void swap( SpaceGameEngine::Thread &lhs, SpaceGameEngine::Thread &rhs ) noexcept
{
	lhs.Swap( rhs );
}

#endif //SPACEGAMEENGINE_THREAD_H