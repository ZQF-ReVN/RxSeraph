#include <RxSeraph/Hook/VFS.h>
#include <ZxHook/Hook.h>


namespace ZQF::RxSeraph::VFS
{
    typedef void(__stdcall* Fn_ScriptLoad)();

	static uint32_t** sg_ppScript = nullptr;
	static uint32_t** sg_ppScriptTemp = nullptr;
	static uint32_t* sg_pCurCodeSlot = nullptr;
	static uint32_t* sg_pPackReadSeq = nullptr;
	static uint32_t* sg_pPackReadMod = nullptr;
	static Fn_ScriptLoad sg_fnScriptLoad = nullptr;


	static void __stdcall ScriptLoad_Hook()
	{
		if (*sg_pPackReadMod == 0)
		{
			*sg_pCurCodeSlot ? (void)(*sg_ppScript = *sg_ppScriptTemp) : (void)(*sg_pCurCodeSlot = 0);
			//wchar_t path[64];
			//::wsprintfW(path, L"ScnPac/%08d.scn", *sg_pPackReadSeq);
			//Rut::RxFile::Binary ifs{ path, Rut::RIO_READ };
			//ifs.Read(*sg_ppScriptTemp, ifs.GetSize());
		}
		else
		{
			return sg_fnScriptLoad();
		}
	}


	void ScriptHook(uint32_t fnScriptLoad, uint32_t nScriptPtr, uint32_t nScriptTempPtr, uint32_t nCurCodeSlot, uint32_t nPackReadSeq, uint32_t nPackReadMod)
	{
		sg_ppScript = (uint32_t**)nScriptPtr;
		sg_ppScriptTemp = (uint32_t**)nScriptTempPtr;
		sg_pCurCodeSlot = (uint32_t*)nCurCodeSlot;
		sg_pPackReadSeq = (uint32_t*)nPackReadSeq;
		sg_pPackReadMod = (uint32_t*)nPackReadMod;
		sg_fnScriptLoad = (Fn_ScriptLoad)fnScriptLoad;

        ZxHook::Detours::AttachDirectly(&sg_fnScriptLoad, VFS::ScriptLoad_Hook);
	}
}
