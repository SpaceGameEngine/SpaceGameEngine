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

#include <atomic>
#include <chrono>

#include "Error.h"
#include "SGEString.h"
#include "Concurrent/Lock.h"

namespace SpaceGameEngine
{

	/*!
	 * \brief ExecutionError is thrown when trying to get result of a failed async procedure
	 */
	struct ExecutionError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR( "Execution Error" );

		static bool Judge();
	};

	/*!
	 * \brief TimeoutError is thrown when it took longer than expected to wait for a result
	 */
	struct TimeoutError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR( "Timeout" );

		static bool Judge();
	};

	/*!
	 * \brief class Future respects the state of a async procedure
	 * Future cannot be copied nor moved. You cannot directly construct a Future.
	 * You can only get a Future by running a async procedure, for example, submitting task to
	 * an Executor or doing async I/O.
	 * \tparam ResultType the type of result of the corresponding async procedure
	 */
	template<typename ResultType>
	class Future
	{
		Mutex m_Mutex;
		ResultType m_Result;
		std::atomic_int8_t m_State;
		Condition m_Condition;

		Future() = default;

	public:
		Future( const Future &other ) = delete;

		Future &operator=( const Future &other ) = delete;

		enum class FutureState : int8_t
		{
			Working = 0, Succeed = 1, Failed = 2
		};

		/*!
		 * \brief Wait for the async procedure ending
		 */
		void Wait()
		{
			RecursiveLock lock( m_Mutex );
			m_Condition.Wait( lock,
							  [&]
							  {
								  return m_State.load() != FutureState::Working;
							  } );
		}

		/*!
		 * \brief Wait for the async procedure ending in a limited time
		 * \param rel_time the max waiting time
		 * \return true if the async procedure ends in time
		 */
		template<class Rep, class Period>
		bool Wait( const std::chrono::duration<Rep, Period> &rel_time )
		{
			RecursiveLock lock( m_Mutex );
			return m_Condition.WaitFor( lock, rel_time,
										[&]
										{
											return m_State.load() != FutureState::Working;
										} );
		}

		/*!
		 * \brief Get the result of the async procedure
		 * Block the current thread until the async procedure ends.
		 * Throw ExecutionError if it fails.
		 * \return the result of the async procedure
		 */
		ResultType Get()
		{
			Wait();
			if (GetState() == FutureState::Failed)
			{
				SGE_CHECK( ExecutionError );
			}
			return m_Result;
		}

		/*!
		 * \brief Get the result of the async procedure in a limited time
		 * Block the current thread until the async procedure ends.
		 * Throw ExecutionError if it fails.
		 * Throw TimeoutError if blocking too long.
		 * \param rel_time the max blocking time
		 * \return the result of the async procedure
		 */
		template<class Rep, class Period>
		ResultType Get( const std::chrono::duration<Rep, Period> &rel_time )
		{
			if (!Wait( rel_time ))
			{
				SGE_CHECK( TimeoutError );
			}
			if (GetState() == FutureState::Failed)
			{
				SGE_CHECK( ExecutionError );
			}

			return m_Result;
		}

		/*!
		 * \brief Get the state of the async procedure.
		 * \return the state of the async procedure
		 */
		FutureState GetState()
		{
			return m_State.load();
		}
	};
}