#include <Windows.h>
#include <ZxHook/Hook.h>
#include <RxSeraph/VFS.h>


static auto StartHook() -> void
{
	ZQF::RxSeraph::VFS::ScriptHook(0x0040C570, 0x0045E180, 0x0045DC80, 0x0045E160, 0x0045E014, 0x0045E100);
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
        ZQF::ZxHook::Detours::AfterWith();
		::StartHook();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

void __declspec(dllexport) DirA() {};
