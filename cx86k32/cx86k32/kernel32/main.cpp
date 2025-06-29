/*
 electr0dev 16/06/25

 Kernel32.dll
 DllMain.cpp
 
 Bu dosya Win8.1 Kernel32'si için bir proxy niteliğinde olan coawow64k32 isimli kitaplığın ana dosyasıdır.
*/

#include "coak32.h"

HINSTANCE gDllhInstance = NULL;

BOOL ProcessAttach();
BOOL ProcessDetach();
extern "C" __declspec(dllexport)
typedef BOOL (WINAPI* LPFN_ISWOW64PROCESS2)(HANDLE hProcess, USHORT* pProcessMachine, USHORT* pNativeMachine);//IsWow64Process2 Hook Sabit 64bit(typedef)

// NtAllocateVirtualMemory ve RtlNtStatusToDosError için tanımlar
typedef NTSTATUS (NTAPI *NtAllocateVirtualMemory_t)(
    HANDLE ProcessHandle,
    PVOID *BaseAddress,
    ULONG ZeroBits,
    PSIZE_T RegionSize,
    ULONG AllocationType,
    ULONG Protect
);

typedef ULONG (WINAPI *RtlNtStatusToDosError_t)(NTSTATUS Status);


//CPU Frekansı gerekli olan API(ler) için cacheleniyor.

static CRITICAL_SECTION g_cs;
static std::map<DWORD, std::wstring> g_threadDescriptions;


ULONGLONG GetEstimatedCPUFrequency() {
    LARGE_INTEGER qpcFreq, qpcStart, qpcEnd;
    unsigned __int64 tscStart, tscEnd;

    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);
    tscStart = __rdtsc();

    Sleep(100); // 100ms bekle

    tscEnd = __rdtsc();
    QueryPerformanceCounter(&qpcEnd);

    // Geçen süreyi saniye cinsine çevir
    double elapsedSec = (double)(qpcEnd.QuadPart - qpcStart.QuadPart) / qpcFreq.QuadPart;

    // Frekans = sayaç farkı / geçen süre
    return (ULONGLONG)((tscEnd - tscStart) / elapsedSec);
}

ULONGLONG cachedcpufrequency = GetEstimatedCPUFrequency(); //ULONG tanımlama. ULONGLONG olmasının sebebi ise karakter byte sınırı.

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD dwReason, PVOID pvReserved)
{
    UNREFERENCED_PARAMETER(pvReserved);

    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        gDllhInstance =hInstDLL; 
		InitializeCriticalSection(&g_cs);
        return ProcessAttach();
    case DLL_PROCESS_DETACH:
        return ProcessDetach();
    }
    return TRUE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI IsWow64Process2(HANDLE hProcess, USHORT* pProcessMachine, USHORT* pNativeMachine) {
    //Her zaman 32-bit olarak kabul edilir
    if (pProcessMachine)
        *pProcessMachine = IMAGE_FILE_MACHINE_I386; // Sabit: 32 bit
    if (pNativeMachine)
		*pNativeMachine = IMAGE_FILE_MACHINE_I386;
	testbox(L"C1");
    return TRUE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI GetVersionExW(OSVERSIONINFOW* vi)
{
    if (!vi || vi->dwOSVersionInfoSize != sizeof(OSVERSIONINFOW))
        return FALSE;
    vi->dwMajorVersion = 10;
    vi->dwMinorVersion = 0;
    vi->dwBuildNumber = 19045;
    vi->dwPlatformId = VER_PLATFORM_WIN32_NT;
    vi->szCSDVersion[0] = 0;
	testbox(L"C2");
    return TRUE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI GetVersionExA(OSVERSIONINFOA* vi)
{
    if (!vi || vi->dwOSVersionInfoSize != sizeof(OSVERSIONINFOA))
        return FALSE;
    vi->dwMajorVersion = 10;
    vi->dwMinorVersion = 0;
    vi->dwBuildNumber = 19045;
    vi->dwPlatformId = VER_PLATFORM_WIN32_NT;
    vi->szCSDVersion[0] = 0;
	testbox(L"C3");
    return TRUE;
}

/*extern "C" __declspec(dllexport)
BOOL WINAPI Wow64GetThreadContext(HANDLE hThread, PWOW64_CONTEXT lpContext)
{
    if (!lpContext) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    memset(lpContext, 0, sizeof(WOW64_CONTEXT));
    lpContext->ContextFlags = 0x00010007; // CONTEXT_FULL
    lpContext->Eip = 0xDEADBEEF;
    lpContext->Eax = 0x41414141;
    lpContext->Esp = 0x13371337;
	//sahte adres gonderiyoruz
    return TRUE; 
}*/

extern "C" __declspec(dllexport)
BOOL WINAPI GetSystemWow64Directory2W(
    LPWSTR lpBuffer,
    UINT uSize,
    DWORD dwFlags
) {
	testbox(L"C5");
    // dwFlags şu anlık dikkate alınmıyor
    return GetSystemWow64DirectoryW(lpBuffer, uSize);
}

extern "C" __declspec(dllexport)
BOOL WINAPI GetSystemWow64Directory2A(
    LPSTR lpBuffer,
    UINT uSize,
    DWORD dwFlags
) {
	testbox(L"C6");
    // dwFlags şu anlık dikkate alınmıyor
    return GetSystemWow64DirectoryA(lpBuffer, uSize);
}

extern "C" __declspec(dllexport)
BOOL WINAPI IsWow64GuestMachineSupported(
    USHORT WowGuestMachine,
    PBOOL MachineIsSupported
) {
	testbox(L"C7");
    if (MachineIsSupported)
        *MachineIsSupported = TRUE;

    return TRUE;
} //x86 da x64 de destekliyor.

extern "C" __declspec(dllexport)
BOOL WINAPI COAWINAPI(UINT Status, UINT MainVer, UINT Ver2, UINT Ver3){
	if(Status){
		//RC=0 STABLE=1
		Status = 0;
		MainVer = 2; //Ana sürüm
		Ver2 = 0;//Bir alt sürüm
		Ver3 = 0;//Bir alt sürümün altındaki sürüm
	}
	testbox(L"C8");
	return TRUE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI QueryAuxiliaryCounterFrequency(PULONGLONG pCPUFREQ) {
    if (pCPUFREQ) {
        *pCPUFREQ = cachedcpufrequency;
		testbox(L"C9");
        return TRUE;
    }
    return FALSE;
}

/*extern "C" __declspec(dllexport)
BOOL WINAPI dVerifyVersionInfoW(LPOSVERSIONINFOEXW lpVerInfoW, DWORD dwTypeMaskW, DWORDLONG dwlCondtMaskW)
{
    // Sahte sürüm bilgisi (örneğin Windows 10.0)
    OSVERSIONINFOEXW slpVerInfoW = *lpVerInfoW;
    slpVerInfoW.dwMajorVersion = 10;
    slpVerInfoW.dwMinorVersion = 0;
	slpVerInfoW.wServicePackMajor = 0;
	testbox(L"C10");
    return TRUE;
}*/

extern "C" __declspec(dllexport)
BOOL WINAPI VerifyVersionInfoA(LPOSVERSIONINFOEXA lpVerInfoA, DWORD dwTypeMaskA, DWORDLONG dwlCondtMaskA)
{
	testbox(L"C11");
    return TRUE;
}



extern "C" __declspec(dllexport)
HRESULT WINAPI dfSetThreadDescription(HANDLE hThread, PCWSTR lpThreadDescription)
{
	if (!hThread || !lpThreadDescription)
		return E_INVALIDARG;

	DWORD tid = GetThreadId(hThread);
	if (tid == 0)
		return HRESULT_FROM_WIN32(GetLastError());

	EnterCriticalSection(&g_cs); // Mutex yerine Critical Section kullanıyoruz

	g_threadDescriptions[tid] = lpThreadDescription;

	LeaveCriticalSection(&g_cs); // Mutex yerine Critical Section kullanıyoruz
	testbox(L"C12");
	return S_OK;
}

extern "C" __declspec(dllexport)
HRESULT WINAPI dfGetThreadDescription(HANDLE hThread, PWSTR* ppszThreadDescription)
{
	testbox(L"C13");
	if (!hThread || !ppszThreadDescription)
		return E_INVALIDARG;

	DWORD tid = GetThreadId(hThread);
	if (tid == 0)
		return HRESULT_FROM_WIN32(GetLastError());

	EnterCriticalSection(&g_cs);

	std::map<DWORD, std::wstring>::iterator it = g_threadDescriptions.find(tid);
	if (it == g_threadDescriptions.end())
	{
		LeaveCriticalSection(&g_cs);
		return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
	}

	size_t len = it->second.length() + 1;
	*ppszThreadDescription = (PWSTR)LocalAlloc(LMEM_ZEROINIT, len * sizeof(WCHAR));
	if (!*ppszThreadDescription)
	{
		LeaveCriticalSection(&g_cs);
		return E_OUTOFMEMORY;
	}

	wcscpy_s(*ppszThreadDescription, len, it->second.c_str());

	LeaveCriticalSection(&g_cs);
	return S_OK;
}

BOOL WINAPI VerifyVersionInfoW(
	LPOSVERSIONINFOEXW lpVersionInformation,
	DWORD dwTypeMask,
	DWORDLONG dwlConditionMask
	) {
	testbox(L"C10");
	// Sadece her zaman "bu sistem uygundur" de
	return TRUE;
}

/*ULONGLONG WINAPI VerSetConditionMask(
	ULONGLONG ConditionMask,
	DWORD TypeMask,
	BYTE Condition
	) {
	return 0xC0AC0AC0AC0AC0A; // C0A C0A C0A C0A C0A
}*/

/*extern "C" __declspec(dllexport)
FARPROC WINAPI dGetProcAddress(HMODULE hModule, LPCSTR lpProcName) // Eğer process direkt kernel32.dll den çağırmaya zorluyorsa GetProcAdress üzerinde bulup çağrıyı değiştiriyoruz.
{
	testbox(L"C14");
	if (strcmp(lpProcName, "SetThreadDescription") == 0) {
		return (FARPROC)&dfSetThreadDescription;
	}
	else if (strcmp(lpProcName, "GetThreadDescription") == 0){
		return (FARPROC)&dfGetThreadDescription;
	}
	else{
		return RealGetProcAddr(hModule, lpProcName);
	}//Eğer çağırılan fonksiyon yukarıdakilerden biri değilse gerçek fonksiyonu döndürür.
}*/


void cpufreqmsgbox(){
	std::wstringstream wss;
	wss << cachedcpufrequency;
	std::wstring wstr = wss.str();
    LPCWSTR lpcwstrCPUFREQ = wstr.c_str();
	LPCWSTR LCPUFREQMSG = L"Cached CPU Frequency: ";
	std::wstring BIRLESIK = LCPUFREQMSG; // LCPUFREQMSG
    BIRLESIK += lpcwstrCPUFREQ;
	LPCWSTR CpuFreqResult = BIRLESIK.c_str();
    int msgboxID = MessageBoxW(
    NULL,
    (LPCWSTR)CpuFreqResult,
    (LPCWSTR)L"COA 8.1 Kernel32 Debug Window",
    MB_ICONWARNING | MB_OKCANCEL | MB_DEFBUTTON2
    );
    switch (msgboxID)
    {
    case IDCANCEL:
        break;
	case IDOK:
		break;
    }
}


BOOL ProcessAttach()
{ 
	cpufreqmsgbox();
    return TRUE;
}

BOOL ProcessDetach()
{
	DeleteCriticalSection(&g_cs);
    return TRUE;
}


#define C_DLL_EXPORT             extern "C" __declspec(dllexport)
#define CALLING_CONVENTION       __stdcall

