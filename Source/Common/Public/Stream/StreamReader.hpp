/*
Copyright 2025 creatorlxd

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
#include "Stream.h"
#include "Serializer/Serializer.h"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	template<typename T>
	concept IsStreamReader = requires(InputStream& stream, T reader, int i) {
		{
			T(stream)
		} -> std::same_as<T>;
		{
			reader >> i
		} -> std::same_as<T&>;
		{
			static_cast<bool>(reader)
		} -> std::same_as<bool>;
	};

	/*!
	@brief A stream reader that reads data from an input stream using a serializer.
	@note Users should specialize this class for their own serializer types.
	*/
	template<IsSerializer Serializer>
	class StreamReader
	{
	};
}

/*!
@}
*/