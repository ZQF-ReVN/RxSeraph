#include "VFS.h"
#include <Rut/RxFile.h>
#include <RxHook/Hook.h>


namespace Seraph::VFS
{
	static uint32_t** sg_ppCode = (uint32_t**)0x0045E180;
	static uint32_t** sg_ppCodeBuffer = (uint32_t**)0x0045DC80;
	static uint32_t* sg_nCurCodeSlotPtr = (uint32_t*)0x0045E160;
	static uint32_t* sg_nPackReadSeqPtr = (uint32_t*)0x0045E014;
	static uint32_t* sg_nPackReadModePtr = (uint32_t*)0x0045E100;

	typedef void(__stdcall* Fn_ScriptLoad)();
	static Fn_ScriptLoad sg_fnScriptLoad = (Fn_ScriptLoad)0x0040C570;

	static void __stdcall ScriptLoad_Hook()
	{
		if (*sg_nPackReadModePtr == 0)
		{
			if (*sg_nCurCodeSlotPtr)
			{
				*sg_ppCode = *sg_ppCodeBuffer;
			}
			*sg_nCurCodeSlotPtr = 0;

			wchar_t path[256];
			::wsprintfW(path, L"ScnPac/%d", *sg_nPackReadSeqPtr);
			Rut::RxFile::Binary ifs{ path, Rut::RIO_READ };
			ifs.Read(*sg_ppCodeBuffer, ifs.GetSize());
		}
		else
		{
			return sg_fnScriptLoad();
		}
	}

	void SetHook()
	{
		Rut::RxHook::Detours::AttachDirectly(&sg_fnScriptLoad, ScriptLoad_Hook);
	}
}