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
#include "ContainerConcept.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/
	/*!
	@brief a adaptor for a bidirectional iterator type to get its reverse iterator type.
	@param T a bidirectional iterator type.
	*/
	template<typename T>
	class ReverseIteratorImpl
	{
	};

	template<template<typename> class IteratorImpl, typename T>
	class ReverseIteratorImpl<IteratorImpl<T>>
	{
		static_assert((IsBidirectionalIterator<IteratorImpl<T>, T>::Result), "only the bidirectional iterator type can be passed to get the reverse iterator.");
	};
	/*!
	@}
	*/
}