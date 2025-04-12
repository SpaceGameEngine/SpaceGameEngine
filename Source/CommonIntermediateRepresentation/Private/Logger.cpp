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
using namespace SpaceGameEngine::CommonIntermediateRepresentation;

LogWriter<BindConsoleLogWriterCore<FileLogWriterCore>>& SpaceGameEngine::CommonIntermediateRepresentation::GetCommonIntermediateRepresentationLogWriter()
{
	static GlobalVariable<LogWriter<BindConsoleLogWriterCore<FileLogWriterCore>>> g_CommonIntermediateRepresentationLogWriter(GetDefaultLogDirectoryPath() / Path(SGE_STR("CommonIntermediateRepresentation")));
	return g_CommonIntermediateRepresentationLogWriter.Get();
}

Logger<BindConsoleLogWriterCore<FileLogWriterCore>>& SpaceGameEngine::CommonIntermediateRepresentation::GetCommonIntermediateRepresentationLogger()
{
	static GlobalVariable<Logger<BindConsoleLogWriterCore<FileLogWriterCore>>> g_CommonIntermediateRepresentationLogger(GetCommonIntermediateRepresentationLogWriter(), LogLevel::All);
	return g_CommonIntermediateRepresentationLogger.Get();
}
