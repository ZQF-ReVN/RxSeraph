#pragma once
#include <cstdint>


namespace Seraph::VFS
{
	void ScriptHook(uint32_t fnScriptLoad, uint32_t nScriptPtr, uint32_t nScriptTempPtr, uint32_t nCurCodeSlot, uint32_t nPackReadSeq, uint32_t nPackReadMod);
}