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
#include "TypeDefinition.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	/*!
	@brief a sample of the iterator which defines a group of iterator,but can not be used directly.
	@note every container which want to support this iterator must specialize it.
	*/
	template<template<typename...> class Container, typename T>
	class Iterator
	{
	public:
		/*!
		@brief iterator must have a constructor,but its concrete form need to be designed according
		to the concrete container.
		*/
		inline Iterator(...){};

		inline Iterator(const Iterator&){};

		inline Iterator& operator=(const Iterator&){};

		inline Iterator& operator+(SizeType){};

		inline Iterator& operator+=(SizeType){};

		inline Iterator& operator-(SizeType){};

		inline Iterator& operator-=(SizeType){};

		inline T* operator->(){};

		inline T& operator*(){};

	private:
	};

	/*!
	@brief get iterator type from a template container class instance.
	@note the reason why we need this mpl-function is that the simple container typename in its
	definition scope will be interpreted into a template container class instance instead of a
	template.But what the Iterator type needs is a template.However,it is seemed that this little
	question is not easy for people to realize,so I write this mpl-function to provide people
	an easier way to get the result,instead of using things like GetTemplate by giving the simple
	container typename in its definition scope as the template container class instance to get
	the template.
	*/
	template<template<template<typename...> class, typename> class IteratorType, typename Container, typename T>
	struct GetIteratorTypeInstance
	{
	};

	template<template<template<typename...> class, typename> class IteratorType, template<typename... Args> class Container, typename T, typename... Args>
	struct GetIteratorTypeInstance<IteratorType, Container<Args...>, T>
	{
		using Result = IteratorType<Container, T>;
	};
	/*!
	@}
	*/
}