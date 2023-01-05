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
#include "Utility/DebugInformation.h"

using namespace SpaceGameEngine;

SpaceGameEngine::DebugInformation::DebugInformation(const TChar* file_name, const TChar* func_name, UInt32 line_number)
{
	m_pFileName = file_name;
	m_pFunctionName = func_name;
	m_LineNumber = line_number;
}