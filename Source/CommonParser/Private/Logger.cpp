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
#include "Logger.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::CommonParser;

LogWriter<BindConsoleLogWriterCore<FileLogWriterCore>>& SpaceGameEngine::CommonParser::GetCommonParserLogWriter()
{
	static GlobalVariable<LogWriter<BindConsoleLogWriterCore<FileLogWriterCore>>> g_CommonParserLogWriter(GetDefaultLogDirectoryPath() / Path(SGE_STR("CommonParser")));
	return g_CommonParserLogWriter.Get();
}

Logger<BindConsoleLogWriterCore<FileLogWriterCore>>& SpaceGameEngine::CommonParser::GetCommonParserLogger()
{
	static GlobalVariable<Logger<BindConsoleLogWriterCore<FileLogWriterCore>>> g_CommonParserLogger(GetCommonParserLogWriter(), LogLevel::All);
	return g_CommonParserLogger.Get();
}
