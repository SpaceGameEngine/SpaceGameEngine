/*
Copyright 2023 creatorlxd

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
#include "SGEString.hpp"

namespace SpaceGameEngine
{
	template class COMMON_API_TEMPLATE_DEFINE StringImplement::Storage<Char16, DefaultAllocator>;
	template class COMMON_API_TEMPLATE_DEFINE StringImplement::Storage<Char8, DefaultAllocator>;
	template class COMMON_API_TEMPLATE_DEFINE StringCore<Char16, UCS2Trait, DefaultAllocator>;
	template class COMMON_API_TEMPLATE_DEFINE StringCore<Char8, UTF8Trait, DefaultAllocator>;
}
