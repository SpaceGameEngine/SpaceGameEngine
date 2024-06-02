/*
Copyright 2024 creatorlxd

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
#include "CommonAPI.h"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{

	using TimeType = UInt64;

	/*!
	@brief return the now time represented by microsecond.
	*/
	COMMON_API TimeType GetNowTime();

	/*!
	@brief sleep for the time represented by microsecond.
	*/
	COMMON_API void SleepFor(TimeType microsecond);

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
	public:
		inline static constexpr const UInt64 TimeUnitValue = TimeUnit;
		using ValueType = T;

		inline TimeStamp()
			: m_Time(0), m_Value(0)
		{
		}

		/*!
		@brief construction of the TimeStamp.
		@param t the time represented by the microsecond.
		*/
		inline TimeStamp(const TimeType t)
			: m_Time(t), m_Value(((T)t) / ((T)TimeUnit))
		{
		}

		inline operator TimeType() const
		{
			return m_Time;
		}

		T GetValue() const
		{
			return m_Value;
		}

		/*!
		@brief get the time represented by the microsecond.
		*/
		TimeType GetTime() const
		{
			return m_Time;
		}

		/*!
		@brief set the timestamp's time.
		@param t the time represented by the microsecond.
		*/
		void SetTime(const TimeType t)
		{
			m_Time = t;
			m_Value = ((T)t) / ((T)TimeUnit);
		}

	private:
		TimeType m_Time;
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
	public:
		inline static constexpr const UInt64 TimeUnitValue = TimeUnit;
		using ValueType = T;

		inline TimeDuration()
			: m_Time(0), m_Value(0)
		{
		}

		/*!
		@brief construction of the TimeDuration.
		@param t the time represented by the microsecond.
		*/
		inline TimeDuration(const TimeType t)
			: m_Time(t), m_Value(((T)t) / ((T)TimeUnit))
		{
		}

		inline operator TimeType() const
		{
			return m_Time;
		}

		inline TimeDuration operator+(const TimeDuration& t) const
		{
			return TimeDuration(m_Time + t.m_Time);
		}

		inline TimeDuration& operator+=(const TimeDuration& t)
		{
			m_Time += t.m_Time;
			m_Value = ((T)m_Time) / ((T)TimeUnit);
			return *this;
		}

		inline TimeDuration operator-(const TimeDuration& t) const
		{
			return TimeDuration(m_Time - t.m_Time);
		}

		inline TimeDuration& operator-=(const TimeDuration& t)
		{
			m_Time -= t.m_Time;
			m_Value = ((T)m_Time) / ((T)TimeUnit);
			return *this;
		}

		T GetValue() const
		{
			return m_Value;
		}

		/*!
		@brief get the time represented by the microsecond.
		*/
		TimeType GetTime() const
		{
			return m_Time;
		}

		/*!
		@brief set the timestamp's time.
		@param t the time represented by the microsecond.
		*/
		void SetTime(const TimeType t)
		{
			m_Time = t;
			m_Value = ((T)t) / ((T)TimeUnit);
		}

	private:
		T m_Value;
		TimeType m_Time;
	};

	template<UInt64 TimeUnit, typename T = TimeType>
	TimeDuration<TimeUnit, T> MakeTimeDuration(const T val)
	{
		return TimeDuration<TimeUnit, T>((TimeType)(val * TimeUnit));
	}

	template<UInt64 TimeUnit, typename T>
	inline TimeDuration<TimeUnit, T> operator-(const TimeStamp<TimeUnit, T>& t1, const TimeStamp<TimeUnit, T>& t2)
	{
		return TimeDuration<TimeUnit, T>(t1.GetTime() - t2.GetTime());
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
			return m_DeltaTime.GetValue();
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

}

/*!
@}
*/