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
#include "Platform.hpp"

#ifdef SGE_WINDOWS
#ifdef CreateFile
#pragma push_macro("CreateFile")
#undef CreateFile
#endif

#ifdef DeleteFile
#pragma push_macro("DeleteFile")
#undef DeleteFile
#endif

#ifdef CopyFile
#pragma push_macro("CopyFile")
#undef CopyFile
#endif
#endif