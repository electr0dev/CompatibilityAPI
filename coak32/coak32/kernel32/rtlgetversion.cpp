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

LPVOID* orjinaladres = reinterpret_cast<LPVOID*>(&Original); // Original Long Pointer Pointer olduðu için  MinHook'un kabul ettiði tür olan Long Pointer Pointer Void e(yani LPVOID*) dönüþtürdük.

void starthook() {
	if (!hntdll) {
		return;
	}
	testbox(L"Hook baþlatýlýyor.");
	MH_Initialize();
	MH_CreateHook(GetProcAddress(hntdll, "RtlGetVersion"),
		&HookRtlGetVersionTest, orjinaladres); //Pointer deðeri vermek için HookRtlGetVersionTest "&" ile yazýldý.
	MH_EnableHook(MH_ALL_HOOKS);
}