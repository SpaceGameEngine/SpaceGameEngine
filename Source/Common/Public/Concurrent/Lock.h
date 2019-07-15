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
	/*!\brief Class SpaceGameEngine::Mutex is a wrapper of a std::recursive_timed_mutex
	 *
	 * SpaceGameEngine::Mutex does not include functions like lock()/unlock(). The only way to
	 * lock it is to use SpaceGameEngine::RecursiveLock or SpaceGameEngine::ScopedLock.
	 *
	 * Checkout c++ reference for more details.
	 */
	class Mutex
	{
	public:
		Mutex();

		Mutex( const Mutex &other ) = delete;

		~Mutex() = default;

		Mutex &operator=( const Mutex &other ) = delete;

		friend class RecursiveLock;

		template<class ... MutexType>
		friend
		class ScopedLock;

	private:
		std::recursive_timed_mutex m_MutexImpl;
	};

	/*!\brief Class SpaceGameEngine::RecursiveLock is a wrapper of a std::unique_lock<std::recursive_timed_mutex>
	 *
	 * Unlike std::unique_lock, this class's constructor will not lock the mutex, which is same as using
	 * std::defer_lock in std::unique_lock's constructor.
	 *
	 * Checkout c++ reference for more details.
	 */
	class RecursiveLock
	{
	public:
		RecursiveLock( const RecursiveLock & ) = delete;

		RecursiveLock( RecursiveLock &&other ) noexcept;

		explicit RecursiveLock( Mutex & );

		~RecursiveLock() = default;

		RecursiveLock &operator=( const RecursiveLock & ) = delete;

		RecursiveLock &operator=( RecursiveLock &&other ) noexcept;

		void Lock();

		bool TryLock();

		template<class Rep, class Period>
		bool TryLock( const std::chrono::duration<Rep, Period> &timeout_duration )
		{
			return m_LockImpl.try_lock_for( timeout_duration );
		}

		void Unlock();

		friend class Condition;

	private:
		std::unique_lock<std::recursive_timed_mutex> m_LockImpl;
	};

	/*!\brief Class SpaceGameEngine::Condition is a wrapper of a std::condition_variable_any
	 *
	 * SGE::Condition's Wait/WaitFor functions only accept SGE::RecursiveLock instead of any Lockable
	 * object in std::condition_variable_any's corresponding functions.
	 *
	 * Checkout c++ reference for more details.
	 */
	class Condition
	{
	public:
		Condition();

		~Condition() = default;

		Condition( const Condition & ) = delete;

		Condition &operator=( const Condition & ) = delete;

		void NodifyOne();

		void NodifyAll();

		void Wait( RecursiveLock &lock );

		void Wait( RecursiveLock &lock, std::function<bool()> );

		template<class Rep, class Period>
		bool WaitFor( RecursiveLock &lock, const std::chrono::duration<Rep, Period> &rel_time )
		{
			return m_ConditionImpl.wait_for( lock, rel_time ) == std::cv_status::no_timeout;
		}

		template<class Rep, class Period>
		bool WaitFor( RecursiveLock &lock, const std::chrono::duration<Rep, Period> &rel_time,
					  std::function<bool()> pred )
		{
			return m_ConditionImpl.wait_for( lock, rel_time, pred );
		}

	private:
		std::condition_variable_any m_ConditionImpl;
	};

	/*!\brief Class SpaceGameEngine::ScopedLock is a warpper of a std::scoped_lock<SpaceGameEngine::Mutex...>
	 *
	 * This class's constructor only takes SGE::Mutex.
	 *
	 * Checkout c++ reference for more details.
	 *
	 * @tparam MutexType each of these must be SpaceGameEngine::Mutex
	 */
	template<class ... MutexType>
	class ScopedLock
	{
	public:

		explicit ScopedLock( MutexType &... mutex ) : m_LockImpl(( mutex.m_MutexImpl )... )
		{
			static_assert((std::is_same_v<Mutex, MutexType> && ...), "ScopedLock requires SGE::Mutex" );
		}

		ScopedLock( const ScopedLock & ) = delete;

		~ScopedLock() = default;

		ScopedLock &operator=( const ScopedLock &&other ) = delete;

	private:
		std::scoped_lock<decltype( std::declval<MutexType>().m_MutexImpl )...> m_LockImpl;
	};
}
