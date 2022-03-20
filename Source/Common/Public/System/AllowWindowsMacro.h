/*
Copyright 2022 creatorlxd

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
#include "Platform.hpp"

#ifdef SGE_WINDOWS
#ifndef CreateDirectory
#pragma pop_macro("CreateDirectory")
#endif

#ifndef MoveFile
#pragma pop_macro("MoveFile")
#endif

#ifndef CopyFile
#pragma pop_macro("CopyFile")
#endif

#ifndef DeleteFile
#pragma pop_macro("DeleteFile")
#endif

#ifndef CreateFile
#pragma pop_macro("CreateFile")
#endif
#endif