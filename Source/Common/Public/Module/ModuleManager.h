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
#include <type_traits>
#include "Module.h"
#include "SGEString.hpp"
#include "Container/Map.hpp"
#include "Container/Stack.hpp"
#include "Function.hpp"
#include "System/DllLoader.h"
#include "Utility/Singleton.hpp"

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	struct EmptyModuleNameError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Module's name is empty.");
		static COMMON_API bool Judge(const String& name);
	};

	struct ConflictingModuleNameError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("Module's name is conflicting.");
		static COMMON_API bool Judge(const Map<String, Module*>& m, const String& name);
	};

#if defined(SGE_WINDOWS) && defined(SGE_MSVC)
	template class COMMON_API List<Module*>;
	template class COMMON_API Stack<Module*>;
	template struct COMMON_API MapImplement::RedBlackTree<String, Module*>::Node;
	template class COMMON_API MapImplement::RedBlackTree<String, Module*>;
	template class COMMON_API Map<String, Module*>;
	template class COMMON_API List<DllHandle>;
	template class COMMON_API Stack<DllHandle>;
#endif

	class COMMON_API ModuleManager : public UncopyableAndUnmovable, public Singleton<ModuleManager>
	{
	public:
		friend DefaultAllocator;

		template<typename T, typename>
		friend class StaticModule;

		~ModuleManager();

		/*!
		@brief load the module into the module manager.
		@note if the module has been loaded, this function will do nothing.
		*/
		void LoadModule(const String& name);

	private:
		ModuleManager();

		void AddModule(const String& name, Module* pmod);

	private:
		Map<String, Module*> m_StaticModules;
		Stack<Module*> m_Modules;
		Map<String, Module*> m_ModuleMap;
		Stack<DllHandle> m_DllHandles;
	};

	template<typename T, typename = std::enable_if_t<std::is_base_of_v<Module, T>, void>>
	class StaticModule
	{
	public:
		inline StaticModule(const String& name)
		{
			SGE_ASSERT(EmptyModuleNameError, name);
			SGE_ASSERT(ConflictingModuleNameError, ModuleManager::GetSingleton().m_StaticModules, name);
			ModuleManager::GetSingleton().m_StaticModules.Insert(name, DefaultAllocator::New<T>());
		}
	};

#define SGE_STATIC_MODULE(module_name, module_class) SpaceGameEngine::StaticModule<module_class> g_Static##module_class(module_name);
#define SGE_DYNAMIC_MODULE(module_class)                           \
	extern "C" SGE_DLL_EXPORT SpaceGameEngine::Module* GetModule() \
	{                                                              \
		return DefaultAllocator::New<module_class>();              \
	}

#if defined(SGE_USE_DLL) && defined(SGE_WINDOWS)
#define SGE_MODULE(name) SGE_DYNAMIC_MODULE(name##Module);
#else
#define SGE_MODULE(name) SGE_STATIC_MODULE(SGE_STR(#name), name##Module);
#endif
	/*!
	@}
	*/
}