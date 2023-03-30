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
#pragma once
#include "IntermediateRepresentation/Operation.h"

/*!
@ingroup SpaceLanguage
@{
*/

namespace SpaceGameEngine::SpaceLanguage::IntermediateRepresentation
{
	class SPACE_LANGUAGE_API BasicBlock
	{
	public:
		BasicBlock() = delete;

		BasicBlock(const Vector<Operation>& operations);

		const Vector<Operation>& GetContent() const;

		bool operator==(const BasicBlock& bb) const;
		bool operator!=(const BasicBlock& bb) const;

	private:
		Vector<Operation> m_Content;
	};

	struct InvalidBasicBlockError
	{
		inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The BasicBlock is invalid.");
		static SPACE_LANGUAGE_API bool Judge(const BasicBlock& bb);
	};
}

/*!
@}
*/