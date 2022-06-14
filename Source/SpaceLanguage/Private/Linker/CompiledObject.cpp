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
#include "Linker/CompiledObject.h"

using namespace SpaceGameEngine;
using namespace SpaceGameEngine::SpaceLanguage;

SpaceGameEngine::SpaceLanguage::CompiledGlobalVariable::CompiledGlobalVariable(SizeType size, SizeType alignment)
	: m_Size(size), m_Alignment(alignment)
{
}

SizeType SpaceGameEngine::SpaceLanguage::CompiledGlobalVariable::GetSize() const
{
	return m_Size;
}

SizeType SpaceGameEngine::SpaceLanguage::CompiledGlobalVariable::GetAlignment() const
{
	return m_Alignment;
}

bool SpaceGameEngine::SpaceLanguage::CompiledGlobalVariable::operator==(const CompiledGlobalVariable& cgv) const
{
	return m_Size == cgv.m_Size && m_Alignment == cgv.m_Alignment;
}

bool SpaceGameEngine::SpaceLanguage::CompiledGlobalVariable::operator!=(const CompiledGlobalVariable& cgv) const
{
	return m_Size != cgv.m_Size || m_Alignment != cgv.m_Alignment;
}

SpaceGameEngine::SpaceLanguage::CompiledFunction::CompiledFunction(const Vector<UInt8>& instrs, const HashMap<UInt64, Vector<UInt64>>& gv_reqs, const HashMap<UInt64, Vector<UInt64>>& func_reqs)
	: m_Instructions(instrs), m_GlobalVariableRequests(gv_reqs), m_FunctionRequests(func_reqs)
{
	SGE_ASSERT(InvalidCompiledFunctionError, *this);
}

const Vector<UInt8>& SpaceGameEngine::SpaceLanguage::CompiledFunction::GetInstructions() const
{
	return m_Instructions;
}

const HashMap<UInt64, Vector<UInt64>>& SpaceGameEngine::SpaceLanguage::CompiledFunction::GetGlobalVariableRequests() const
{
	return m_GlobalVariableRequests;
}

const HashMap<UInt64, Vector<UInt64>>& SpaceGameEngine::SpaceLanguage::CompiledFunction::GetFunctionRequests() const
{
	return m_FunctionRequests;
}

bool SpaceGameEngine::SpaceLanguage::CompiledFunction::operator==(const CompiledFunction& cfunc) const
{
	return m_Instructions == cfunc.m_Instructions && m_GlobalVariableRequests == cfunc.m_GlobalVariableRequests && m_FunctionRequests == cfunc.m_FunctionRequests;
}

bool SpaceGameEngine::SpaceLanguage::CompiledFunction::operator!=(const CompiledFunction& cfunc) const
{
	return m_Instructions != cfunc.m_Instructions || m_GlobalVariableRequests != cfunc.m_GlobalVariableRequests || m_FunctionRequests != cfunc.m_FunctionRequests;
}

bool SpaceGameEngine::SpaceLanguage::InvalidCompiledFunctionError::Judge(const CompiledFunction& cfunc)
{
	const auto& instrs = cfunc.GetInstructions();
	const auto& gv_reqs = cfunc.GetGlobalVariableRequests();
	const auto& func_reqs = cfunc.GetFunctionRequests();

	HashMap<UInt64, bool> goto_map;
	HashMap<UInt64, bool> val_map;

	SizeType instr_idx = 0;
	while (instr_idx < instrs.GetSize())
	{
		if (instrs[instr_idx] >= InstructionSetSize)
			return true;
		UInt8 instr_size = InstructionSet::GetSingleton().Get(instrs[instr_idx]).m_Size;
		if (instr_size == 9)
		{
			if (instrs[instr_idx] == InstructionTypeIndex::Goto)
				goto_map.Insert(instr_idx + 1, true);
		}
		else if (instr_size == 10)
		{
			if (instrs[instr_idx] == InstructionTypeIndex::If)
				goto_map.Insert(instr_idx + 2, true);
			else if (instrs[instr_idx] == InstructionTypeIndex::Set)
				val_map.Insert(instr_idx + 2, true);
		}
		instr_idx += instr_size;
	}
	if (instr_idx != instrs.GetSize())
		return true;

	for (auto iter = gv_reqs.GetConstBegin(); iter != gv_reqs.GetConstEnd(); ++iter)
	{
		for (auto iiter = iter->m_Second.GetConstBegin(); iiter != iter->m_Second.GetConstEnd(); ++iiter)
		{
			if (val_map.Find(*iiter) == val_map.GetConstEnd())
				return true;
		}
	}

	for (auto iter = func_reqs.GetConstBegin(); iter != func_reqs.GetConstEnd(); ++iter)
	{
		for (auto iiter = iter->m_Second.GetConstBegin(); iiter != iter->m_Second.GetConstEnd(); ++iiter)
		{
			if ((goto_map.Find(*iiter) == goto_map.GetConstEnd()) && (val_map.Find(*iiter) == val_map.GetConstEnd()))
				return true;
		}
	}

	return false;
}

void SpaceGameEngine::SpaceLanguage::CompiledObject::AddCompiledGlobalVariable(UInt64 id, const CompiledGlobalVariable& cgv)
{
	SGE_ASSERT(CompiledGlobalVariableAlreadyExistError, m_CompiledGlobalVariables.Find(id), m_CompiledGlobalVariables.GetConstEnd());
	m_CompiledGlobalVariables.Insert(id, cgv);
}

void SpaceGameEngine::SpaceLanguage::CompiledObject::AddCompiledFunction(UInt64 id, const CompiledFunction& cfunc)
{
	SGE_ASSERT(CompiledFunctionAlreadyExistError, m_CompiledFunctions.Find(id), m_CompiledFunctions.GetConstEnd());
	m_CompiledFunctions.Insert(id, cfunc);
}

const HashMap<UInt64, CompiledGlobalVariable>& SpaceGameEngine::SpaceLanguage::CompiledObject::GetCompiledGlobalVariables() const
{
	return m_CompiledGlobalVariables;
}

const HashMap<UInt64, CompiledFunction>& SpaceGameEngine::SpaceLanguage::CompiledObject::GetCompiledFunctions() const
{
	return m_CompiledFunctions;
}

void SpaceGameEngine::SpaceLanguage::CompiledObject::Replace(const CompiledObject& cobj, const HashMap<UInt64, UInt64>& cgv_tran, const HashMap<UInt64, UInt64>& cfunc_tran)
{
	SGE_ASSERT(InvalidCompiledObjectReplacementError, *this, cobj, cgv_tran, cfunc_tran);

	for (auto iter = cgv_tran.GetConstBegin(); iter != cgv_tran.GetConstEnd(); ++iter)
		m_CompiledGlobalVariables.Find(iter->m_Second)->m_Second = cobj.m_CompiledGlobalVariables.Find(iter->m_First)->m_Second;

	for (auto iter = cfunc_tran.GetConstBegin(); iter != cfunc_tran.GetConstEnd(); ++iter)
		m_CompiledFunctions.Find(iter->m_Second)->m_Second = cobj.m_CompiledFunctions.Find(iter->m_First)->m_Second;
}

bool SpaceGameEngine::SpaceLanguage::CompiledGlobalVariableAlreadyExistError::Judge(const HashMap<UInt64, CompiledGlobalVariable>::ConstIterator& citer, const HashMap<UInt64, CompiledGlobalVariable>::ConstIterator& cend)
{
	return citer != cend;
}

bool SpaceGameEngine::SpaceLanguage::CompiledFunctionAlreadyExistError::Judge(const HashMap<UInt64, CompiledFunction>::ConstIterator& citer, const HashMap<UInt64, CompiledFunction>::ConstIterator& cend)
{
	return citer != cend;
}

bool SpaceGameEngine::SpaceLanguage::InvalidCompiledObjectReplacementError::Judge(const CompiledObject& self, const CompiledObject& cobj, const HashMap<UInt64, UInt64>& cgv_tran, const HashMap<UInt64, UInt64>& cfunc_tran)
{
	if (&self == &cobj)
		return true;

	if (cgv_tran.GetSize() != cobj.GetCompiledGlobalVariables().GetSize())
		return true;
	if (cfunc_tran.GetSize() != cobj.GetCompiledFunctions().GetSize())
		return true;

	HashMap<UInt64, bool> self_count;
	HashMap<UInt64, bool> cobj_count;
	const auto& cobj_cgv = cobj.GetCompiledGlobalVariables();
	const auto& self_cgv = self.GetCompiledGlobalVariables();
	const auto& cobj_cfunc = cobj.GetCompiledFunctions();
	const auto& self_cfunc = self.GetCompiledFunctions();

	for (auto iter = cgv_tran.GetConstBegin(); iter != cgv_tran.GetConstEnd(); ++iter)
	{
		if (cobj_cgv.Find(iter->m_First) == cobj_cgv.GetConstEnd())
			return true;
		if (self_cgv.Find(iter->m_Second) == self_cgv.GetConstEnd())
			return true;

		if (cobj_count.Find(iter->m_First) != cobj_count.GetConstEnd())
			return true;
		else
			cobj_count.Insert(iter->m_First, true);
		if (self_count.Find(iter->m_Second) != self_count.GetConstEnd())
			return true;
		else
			self_count.Insert(iter->m_Second, true);
	}

	self_count.Clear();
	cobj_count.Clear();

	for (auto iter = cfunc_tran.GetConstBegin(); iter != cfunc_tran.GetConstEnd(); ++iter)
	{
		if (cobj_cfunc.Find(iter->m_First) == cobj_cfunc.GetConstEnd())
			return true;
		if (self_cfunc.Find(iter->m_Second) == self_cfunc.GetConstEnd())
			return true;

		if (cobj_count.Find(iter->m_First) != cobj_count.GetConstEnd())
			return true;
		else
			cobj_count.Insert(iter->m_First, true);
		if (self_count.Find(iter->m_Second) != self_count.GetConstEnd())
			return true;
		else
			self_count.Insert(iter->m_Second, true);
	}

	return false;
}

bool SpaceGameEngine::SpaceLanguage::IsValidCompiledObject(const CompiledObject& cobj)
{
	const auto& cobj_cgv = cobj.GetCompiledGlobalVariables();
	const auto& cobj_cfunc = cobj.GetCompiledFunctions();

	for (auto iter = cobj_cfunc.GetConstBegin(); iter != cobj_cfunc.GetConstEnd(); ++iter)
	{
		const auto& gv_reqs = iter->m_Second.GetGlobalVariableRequests();
		const auto& func_reqs = iter->m_Second.GetFunctionRequests();

		for (auto giter = gv_reqs.GetConstBegin(); giter != gv_reqs.GetConstEnd(); ++giter)
		{
			if (cobj_cgv.Find(giter->m_First) == cobj_cgv.GetConstEnd())
				return false;
		}
		for (auto fiter = func_reqs.GetConstBegin(); fiter != func_reqs.GetConstEnd(); ++fiter)
		{
			if (cobj_cfunc.Find(fiter->m_First) == cobj_cfunc.GetConstEnd())
				return false;
		}
	}

	return true;
}

bool SpaceGameEngine::SpaceLanguage::InvalidCompiledObjectError::Judge(const CompiledObject& cobj)
{
	return !IsValidCompiledObject(cobj);
}
