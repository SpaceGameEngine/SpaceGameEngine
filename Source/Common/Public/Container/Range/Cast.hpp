/*
Copyright 2019 creatorlxd

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
#include "TransformCore.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/
	template<typename IteratorType, typename SentinelType = IteratorType, typename Allocator = DefaultAllocator>
	inline Vector<typename IteratorType::ValueType, Allocator> CastToVector(const Transform<IteratorType, SentinelType>& transform)
	{
		AutoReleaseBuffer arbuff;
		auto re_range = transform.m_Function(arbuff);
		Vector<typename IteratorType::ValueType, Allocator> re;
		for (auto iter = re_range.GetBegin(); re_range.GetEnd() != iter; iter += 1)
			re.EmplaceBack(std::move(*iter));
		return re;
	}
	/*!
	@}
	*/
}