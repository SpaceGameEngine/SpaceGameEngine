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
#include "CommonAPI.h"

/*!
@ingroup SpaceLanguage
@{
*/

#ifdef SGE_USE_DLL
#ifdef SPACE_LANGUAGE_EXPORTS
#define SPACE_LANGUAGE_API SGE_DLL_EXPORT
#define SPACE_LANGUAGE_API_TEMPLATE_DECLARE
#define SPACE_LANGUAGE_API_TEMPLATE_DEFINE SGE_DLL_EXPORT
#else
#define SPACE_LANGUAGE_API SGE_DLL_IMPORT
#define SPACE_LANGUAGE_API_TEMPLATE_DECLARE SGE_DLL_IMPORT
#undef SPACE_LANGUAGE_API_TEMPLATE_DEFINE
#endif
#else
#define SPACE_LANGUAGE_API
#define SPACE_LANGUAGE_API_TEMPLATE
#define SPACE_LANGUAGE_API_TEMPLATE_DECLARE
#define SPACE_LANGUAGE_API_TEMPLATE_DEFINE
#endif

/*!
@}
*/