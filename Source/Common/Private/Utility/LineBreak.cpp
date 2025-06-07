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
#include "Utility/LineBreak.h"

using namespace SpaceGameEngine;

bool SpaceGameEngine::UnknownLineBreakError::Judge(LineBreak lb)
{
	return lb == LineBreak::Unknown || (UInt8)lb > 3;
}

LineBreak SpaceGameEngine::GetLineBreakCore<Char16, UCS2Trait>::Get(Char16 c1, Char16 c2)
{
	if (c1 == SGE_WSTR('\r') && c2 == SGE_WSTR('\n'))
		return LineBreak::CRLF;
	else if (c1 == SGE_WSTR('\n'))
		return LineBreak::LF;
	else if (c1 == SGE_WSTR('\r'))
		return LineBreak::CR;
	else
		return LineBreak::Unknown;
}

LineBreak SpaceGameEngine::GetLineBreakCore<Char8, UTF8Trait>::Get(const Char8* pc1, const Char8* pc2)
{
	SGE_ASSERT(NullPointerError, pc1);
	SGE_ASSERT(StringImplement::InvalidUTF8CharError, pc1);
	SGE_ASSERT(NullPointerError, pc2);
	SGE_ASSERT(StringImplement::InvalidUTF8CharError, pc2);
	if ((*pc1) == SGE_U8STR('\r') && (*pc2) == SGE_U8STR('\n'))
		return LineBreak::CRLF;
	else if ((*pc1) == SGE_U8STR('\n'))
		return LineBreak::LF;
	else if ((*pc1) == SGE_U8STR('\r'))
		return LineBreak::CR;
	else
		return LineBreak::Unknown;
}

LineBreak SpaceGameEngine::GetSystemLineBreak()
{
#ifdef SGE_WINDOWS
	return LineBreak::CRLF;
// #elif defined(SGE_MACOS)
//	return FileLineBreak::CR;
// #elif defined(SGE_LINUX)
//	return FileLineBreak::LF;
#elif defined(SGE_UNIX)
	return LineBreak::LF;
#else
#error this os has not been supported.
#endif
}