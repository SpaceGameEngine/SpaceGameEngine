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

using namespace SpaceGameEngine;

#if defined(SGE_WINDOWS) && defined(SGE_MSVC) && defined(SGE_USE_DLL)
template class SGE_DLL_EXPORT StringImplement::Storage<Char16, DefaultAllocator>;
template class SGE_DLL_EXPORT StringImplement::Storage<Char8, DefaultAllocator>;
template class SGE_DLL_EXPORT StringCore<Char16, UCS2Trait, DefaultAllocator>;
template class SGE_DLL_EXPORT StringCore<Char8, UTF8Trait, DefaultAllocator>;
#endif