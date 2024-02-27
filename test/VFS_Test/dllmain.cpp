#include <Windows.h>

#include <Seraph/VFS.h>


static DWORD g_dwExeBase = (DWORD)GetModuleHandleW(NULL);


void StartHook()
{
	Seraph::VFS::SetHook();
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		StartHook();
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