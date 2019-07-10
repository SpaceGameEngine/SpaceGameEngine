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

#include "Concurrent/Thread.h"

SpaceGameEngine::Thread::~Thread()
{
	if (threadImpl.joinable())
	{
		threadImpl.detach();
	}
}

bool SpaceGameEngine::Thread::isJoinable() const noexcept
{
	return threadImpl.joinable();
}

void SpaceGameEngine::Thread::Join()
{
	threadImpl.join();
}

void SpaceGameEngine::Thread::Detach()
{
	if (threadImpl.joinable())
	{
		threadImpl.detach();
	}
}

void SpaceGameEngine::Thread::Swap( SpaceGameEngine::Thread &other ) noexcept
{
	std::swap( threadImpl, other.threadImpl );
}

SpaceGameEngine::ThreadID SpaceGameEngine::Thread::GetThreadID() const noexcept
{
	return threadImpl.get_id();
}

SpaceGameEngine::UInt32 SpaceGameEngine::Thread::HardwareConcurrency() noexcept
{
	return std::thread::hardware_concurrency();
}

void SpaceGameEngine::Thread::YieldCurrentThread() noexcept
{
	std::this_thread::yield();
}

SpaceGameEngine::ThreadID SpaceGameEngine::Thread::GetCurrentThreadID() noexcept
{
	return std::this_thread::get_id();
}

void swap( SpaceGameEngine::Thread &lhs, SpaceGameEngine::Thread &rhs ) noexcept
{
	lhs.Swap( rhs );
}
