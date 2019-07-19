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

	struct ExecutionError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR( "Execution Error" );

		static bool Judge();
	};

	struct TimeoutError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR( "Timeout" );

		static bool Judge();
	};

	template<typename ResultType>
	class Future
	{
		Mutex m_Mutex;
		ResultType m_Result;
		std::atomic_int8_t m_State;
		Condition m_Condition;
	public:
		enum class FutureState : int8_t
		{
			Working = 0, Succeed = 1, Failed = 2
		};

		void Wait()
		{
			RecursiveLock lock( m_Mutex );
			m_Condition.Wait( lock,
							  [&]
							  {
								  return m_State.load() != FutureState::Working;
							  } );
		}

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

		ResultType Get()
		{
			Wait();
			if (GetState() == FutureState::Failed)
			{
				SGE_CHECK( ExecutionError );
			}
			return m_Result;
		}

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

		FutureState GetState()
		{
			return m_State.load();
		}
	};
}