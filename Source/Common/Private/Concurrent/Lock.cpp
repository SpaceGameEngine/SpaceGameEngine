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

#include "Concurrent/Lock.h"

#include <utility>

SpaceGameEngine::Mutex::Mutex() : m_MutexImpl() {}

SpaceGameEngine::ReentrantLock::ReentrantLock( SpaceGameEngine::Mutex &mutex )
		: m_LockImpl( mutex.m_MutexImpl, std::defer_lock ) {}

SpaceGameEngine::ReentrantLock::ReentrantLock( SpaceGameEngine::ReentrantLock &&other ) noexcept
		: m_LockImpl( std::move( other.m_LockImpl )) {}

SpaceGameEngine::ReentrantLock &
SpaceGameEngine::ReentrantLock::operator=( SpaceGameEngine::ReentrantLock &&other ) noexcept
{
	m_LockImpl = std::move( other.m_LockImpl );
	return *this;
}

void SpaceGameEngine::ReentrantLock::Lock()
{
	m_LockImpl.lock();
}

bool SpaceGameEngine::ReentrantLock::TryLock()
{
	return m_LockImpl.try_lock();
}

void SpaceGameEngine::ReentrantLock::Unlock()
{
	m_LockImpl.unlock();
}

SpaceGameEngine::Condition::Condition() : m_ConditionImpl() {}

void SpaceGameEngine::Condition::NodifyOne()
{
	m_ConditionImpl.notify_one();
}

void SpaceGameEngine::Condition::NodifyAll()
{
	m_ConditionImpl.notify_all();
}

void SpaceGameEngine::Condition::Wait( SpaceGameEngine::ReentrantLock &lock )
{
	m_ConditionImpl.wait( lock.m_LockImpl );
}

void SpaceGameEngine::Condition::Wait( SpaceGameEngine::ReentrantLock &lock, std::function<bool()> prec )
{
	m_ConditionImpl.wait( lock.m_LockImpl, std::move( prec ));
}
