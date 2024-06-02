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
#include "Logger.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;

LogWriter<BindConsoleLogWriterCore<FileLogWriterCore>>& SpaceGameEngine::SpaceLanguage::GetSpaceLanguageLogWriter()
{
	static GlobalVariable<LogWriter<BindConsoleLogWriterCore<FileLogWriterCore>>> g_SpaceLanguageLogWriter(GetDefaultLogDirectoryPath() / Path(SGE_STR("SpaceLanguage")));
	return g_SpaceLanguageLogWriter.Get();
}

Logger<BindConsoleLogWriterCore<FileLogWriterCore>>& SpaceGameEngine::SpaceLanguage::GetSpaceLanguageLogger()
{
	static GlobalVariable<Logger<BindConsoleLogWriterCore<FileLogWriterCore>>> g_SpaceLanguageLogger(GetSpaceLanguageLogWriter(), LogLevel::All);
	return g_SpaceLanguageLogger.Get();
}
