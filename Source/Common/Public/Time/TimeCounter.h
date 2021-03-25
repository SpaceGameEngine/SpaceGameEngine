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
#include "Platform.hpp"
#include "TypeDefinition.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	using TimeType = UInt64;

	/*!
	@brief return the now time represented by microsecond.
	*/
	TimeType GetNowTime();

	/*!
	@brief sleep for the time represented by microsecond.
	*/
	void SleepFor(TimeType microsecond);

	inline constexpr const UInt64 Microsecond = 1;
	inline constexpr const UInt64 Millisecond = 1000;
	inline constexpr const UInt64 Second = 1000000;
	inline constexpr const UInt64 Minute = 60000000;
	inline constexpr const UInt64 Hour = 3600000000;
	inline constexpr const UInt64 Day = 86400000000;
	inline constexpr const UInt64 Week = 604800000000;
	inline constexpr const UInt64 Year = 31536000000000;

	/*
	@brief The TimeStamp is used to mark the time point.
	@warning can not use the TimeStamp to get the current time information(such as the current year).
	*/
	template<UInt64 TimeUnit, typename T = TimeType>
	struct TimeStamp
	{
		inline static constexpr const UInt64 TimeUnitValue = TimeUnit;
		using ValueType = T;

		inline TimeStamp()
			: m_Value(0)
		{
		}

		inline TimeStamp(const TimeType t)
			: m_Value(((T)t) / ((T)TimeUnit))
		{
		}

		inline operator TimeType()
		{
			return (TimeType)(m_Value * TimeUnit);
		}

		T m_Value;
	};

	template<UInt64 TimeUnit, typename T = TimeType>
	inline TimeStamp<TimeUnit, T> GetNowTimeStamp()
	{
		return TimeStamp<TimeUnit, T>(GetNowTime());
	}

	template<UInt64 TimeUnit, typename T = TimeType>
	struct TimeDuration
	{
		inline static constexpr const UInt64 TimeUnitValue = TimeUnit;
		using ValueType = T;

		inline TimeDuration()
			: m_Value(0)
		{
		}

		inline TimeDuration(const T t)
			: m_Value(t)
		{
		}

		inline operator TimeType()
		{
			return (TimeType)(m_Value * TimeUnit);
		}

		inline TimeDuration operator+(const TimeDuration& t) const
		{
			return TimeDuration(m_Value + t.m_Value);
		}

		inline TimeDuration& operator+=(const TimeDuration& t)
		{
			m_Value += t.m_Value;
			return *this;
		}

		inline TimeDuration operator-(const TimeDuration& t) const
		{
			return TimeDuration(m_Value - t.m_Value);
		}

		inline TimeDuration& operator-=(const TimeDuration& t)
		{
			m_Value -= t.m_Value;
			return *this;
		}

		T m_Value;
	};

	template<UInt64 TimeUnit, typename T>
	inline TimeDuration<TimeUnit, T> operator-(const TimeStamp<TimeUnit, T>& t1, const TimeStamp<TimeUnit, T>& t2)
	{
		return TimeDuration<TimeUnit, T>(t1.m_Value - t2.m_Value);
	}

	template<UInt64 TimeUnit, typename T = TimeType>
	class TimeCounter
	{
	public:
		inline static constexpr const UInt64 TimeUnitValue = TimeUnit;
		using ValueType = T;
		using TimeStampType = TimeStamp<TimeUnit, T>;
		using TimeDurationType = TimeDuration<TimeUnit, T>;

		inline TimeCounter()
		{
		}

		inline T GetDeltaTime() const
		{
			return m_DeltaTime.m_Value;
		}

		inline void Start()
		{
			m_CurrentTimeStamp = GetNowTimeStamp<TimeUnit, T>();
			m_PreviousTimeStamp = m_CurrentTimeStamp;
		}

		/*!
		@warning must call Tick() every frame.
		*/
		inline void Tick()
		{
			m_CurrentTimeStamp = GetNowTimeStamp<TimeUnit, T>();
			m_DeltaTime = m_CurrentTimeStamp - m_PreviousTimeStamp;
			m_PreviousTimeStamp = m_CurrentTimeStamp;
		}

	private:
		TimeStampType m_CurrentTimeStamp;
		TimeStampType m_PreviousTimeStamp;
		TimeDurationType m_DeltaTime;
	};

	/*!
	@}
	*/
}