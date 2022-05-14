/*
Copyright 2022 creatorlxd

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

/*!
@defgroup Bootstrap Bootstrap Module
*/

/*!
@ingroup Bootstrap
@{
*/

namespace SpaceGameEngine
{

	/*!
	@brief do some essential works to bootstrap the whole engine.
	@warning the Bootstrap Module can only be linked as static library.
	@warning the whole program can only have one Bootstrap instance as a global variable in main.cpp.
	*/
	class Bootstrap
	{
	public:
		Bootstrap();
		~Bootstrap();
	};

}

/*!
@}
*/