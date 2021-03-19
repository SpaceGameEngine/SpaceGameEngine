/*
Copyright 2021 creatorlxd

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
@ingroup Common
@{
*/

#ifdef _WIN64
#define SGE_WINDOWS
#define SGE_X64
#elif defined(_WIN32)
#define SGE_WINDOWS
#define SGE_X86
#endif

#if defined(__linux__) || defined(__linux)
#define SGE_LINUX
#define SGE_UNIX
#endif

#if defined(__APPLE__) || defined(__MACOSX__)
#define SGE_MACOS
#define SGE_UNIX
#endif

#ifdef SGE_UNIX
#ifdef __x86_64__
#define SGE_X64
#elif defined(__i386__)
#define SGE_X86
#endif
#endif

#ifdef _MSC_VER
#define SGE_MSVC 1
#define SGE_COMPILER SGE_MSVC
#elif defined(__clang__)
#define SGE_CLANG 2
#define SGE_COMPILER SGE_CLANG
#elif defined(__GNUC__)
#define SGE_GCC 3
#define SGE_COMPILER SGE_GCC
#endif

/*!
@file
@todo check these platform macros.
*/

/*!
@}
*/