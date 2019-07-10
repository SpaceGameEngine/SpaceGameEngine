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

#pragma once

#include <mutex>
#include <shared_mutex>
#include <condition_variable>

namespace SpaceGameEngine
{
	class Mutex
	{
	public:
		Mutex();

		Mutex( const Mutex &other ) = delete;

		~Mutex() = default;

		Mutex &operator=( const Mutex &&other ) = delete;

		friend class ReentrantLock;

		template<class ... MutexType>
		friend
		class ScopedLock;

	private:
		std::recursive_timed_mutex mutexImpl;
	};

	class Condition
	{
	public:
		Condition( const Condition & ) = delete;

		Condition &operator=( const Condition & ) = delete;

		void NodifyOne();

		void NodifyAll();

		void Wait();

		void Wait( std::function<bool()> );

		template<class Rep, class Period>
		bool WaitFor( const std::chrono::duration<Rep, Period> &rel_time )
		{
			return conditionImpl.wait_for( rel_time ) == std::cv_status::no_timeout;
		}

		template<class Rep, class Period>
		bool WaitFor( const std::chrono::duration<Rep, Period> &rel_time,
					  std::function<bool()> pred )
		{
			return conditionImpl.wait_for( rel_time, pred );
		}

		friend class ReentrantLock;

	private:
		Condition( std::unique_lock<std::recursive_timed_mutex>& lock );

		std::unique_lock<std::recursive_timed_mutex>& lockImpl;
		std::condition_variable_any conditionImpl;
	};

	class ReentrantLock
	{
	public:
		ReentrantLock( const ReentrantLock & ) = delete;

		ReentrantLock( ReentrantLock &&other ) noexcept;

		explicit ReentrantLock( Mutex & );

		~ReentrantLock() = default;

		ReentrantLock &operator=( const ReentrantLock & ) = delete;

		ReentrantLock &operator=( ReentrantLock &&other ) noexcept;

		void Lock();

		void TryLock();

		Condition newCondition();

		template<class Rep, class Period>
		bool TryLock( const std::chrono::duration<Rep, Period> &timeout_duration )
		{
			lockImpl.try_lock_for( timeout_duration );
		}

		void Unlock();

	private:
		std::unique_lock<std::recursive_timed_mutex> lockImpl;
	};

	template<class ... MutexType>
	class ScopedLock
	{
	public:

		explicit ScopedLock( MutexType &... mutex ) : lockImpl(( mutex.mutexImpl )... )
		{
			static_assert((std::is_same_v<Mutex, MutexType> && ...), "ScopedLock requires SGE::Mutex" );
		}

		ScopedLock( const ScopedLock & ) = delete;

		~ScopedLock() = default;

		ScopedLock &operator=( const ScopedLock &&other ) = delete;

	private:
		std::scoped_lock<MutexType ...> lockImpl;
	};
}
