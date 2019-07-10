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

SpaceGameEngine::Mutex::Mutex() : mutexImpl() {}

void SpaceGameEngine::Condition::NodifyOne()
{
	conditionImpl.notify_one();
}

void SpaceGameEngine::Condition::NodifyAll()
{
	conditionImpl.notify_all();

}

void SpaceGameEngine::Condition::Wait()
{
	conditionImpl.wait( lockImpl );
}

void SpaceGameEngine::Condition::Wait( std::function<bool()> pred )
{
	conditionImpl.wait( lockImpl, std::move(pred) );
}

SpaceGameEngine::Condition::Condition( std::unique_lock<std::recursive_timed_mutex> &lock )
		: lockImpl( lock ) {}

SpaceGameEngine::ReentrantLock::ReentrantLock( SpaceGameEngine::Mutex &mutex ) : lockImpl( mutex.mutexImpl ) {}

SpaceGameEngine::ReentrantLock::ReentrantLock( SpaceGameEngine::ReentrantLock &&other ) noexcept
		: lockImpl( std::exchange( other.lockImpl, std::recursive_timed_mutex())) {}

SpaceGameEngine::ReentrantLock &
SpaceGameEngine::ReentrantLock::operator=( SpaceGameEngine::ReentrantLock &&other ) noexcept
{
	lockImpl = std::exchange( other.lockImpl, std::recursive_timed_mutex());
	return *this;
}

void SpaceGameEngine::ReentrantLock::Lock()
{
	lockImpl.lock();
}

void SpaceGameEngine::ReentrantLock::TryLock()
{
	lockImpl.try_lock();
}

void SpaceGameEngine::ReentrantLock::Unlock()
{
	lockImpl.unlock();
}
