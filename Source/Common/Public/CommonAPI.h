/*
Copyright 2024 creatorlxd

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
#include "Platform.hpp"

/*!
@ingroup Common
@{
*/

#define SGE_USE_DLL

#ifdef SGE_USE_DLL
#if defined(SGE_WINDOWS) && defined(SGE_MSVC)

#define SGE_DLL_IMPORT __declspec(dllimport)
#define SGE_DLL_EXPORT __declspec(dllexport)

#else

#define SGE_DLL_IMPORT
#define SGE_DLL_EXPORT __attribute__((visibility("default")))

#endif
#else
#define SGE_DLL_IMPORT
#define SGE_DLL_EXPORT
#endif

#ifdef SGE_USE_DLL
#ifdef COMMON_EXPORTS
#define COMMON_API SGE_DLL_EXPORT
#define COMMON_API_TEMPLATE_DECLARE
#define COMMON_API_TEMPLATE_DEFINE SGE_DLL_EXPORT
#else
#define COMMON_API SGE_DLL_IMPORT
#define COMMON_API_TEMPLATE_DECLARE SGE_DLL_IMPORT
#undef COMMON_API_TEMPLATE_DEFINE
#endif
#else
#define COMMON_API
#define COMMON_API_TEMPLATE_DECLARE
#define COMMON_API_TEMPLATE_DEFINE
#endif

/*!
@}
*/