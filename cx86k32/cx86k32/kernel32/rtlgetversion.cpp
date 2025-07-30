#include "coak32.h"
#include "MinHook.h"

HMODULE hntdll = GetModuleHandleW(L"ntdll.dll");

typedef NTSTATUS(WINAPI* RtlGetVersion_t)(PRTL_OSVERSIONINFOW);
RtlGetVersion_t Original = nullptr;

NTSTATUS WINAPI HookRtlGetVersionTest(PRTL_OSVERSIONINFOW osverinfo) {
	NTSTATUS rtlgetverstatus = Original(osverinfo);
	osverinfo->dwMajorVersion = 10;
	osverinfo->dwMinorVersion = 0;
	return rtlgetverstatus;
}

LPVOID* orjinaladres = reinterpret_cast<LPVOID*>(&Original); // Original Long Pointer Pointer oldu�u i�in  MinHook'un kabul etti�i t�r olan Long Pointer Pointer Void e(yani LPVOID*) d�n��t�rd�k.

void starthook() {
	if (!hntdll) {
		return;
	}
	testbox(L"Hook ba�lat�l�yor.");
	MH_Initialize();
	MH_CreateHook(GetProcAddress(hntdll, "RtlGetVersion"),
		&HookRtlGetVersionTest, orjinaladres); //Pointer de�eri vermek i�in HookRtlGetVersionTest "&" ile yaz�ld�.
	MH_EnableHook(MH_ALL_HOOKS);
}