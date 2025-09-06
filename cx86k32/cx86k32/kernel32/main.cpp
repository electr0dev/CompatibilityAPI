/*
electr0dev 16/06/25

Kernel32.dll
DllMain.cpp

Bu dosya Win8.1 Kernel32'si için bir proxy niteliğinde olan coak32 isimli kitaplığın ana dosyasıdır.
*/

#include "coak32.h"
#include <map>
#include <string>
#include <sstream>

HINSTANCE gDllhInstance = NULL;
BOOL ProcessAttach();
BOOL ProcessDetach();

typedef struct COA_MEM_ADD_REQ {
    PVOID LowestStartingAddress;
    PVOID HighestEndingAddress;
    SIZE_T Alignment;
} COA_MEM_ADDR_REQ, * PCOA_MEM_ADDR_REQ;

typedef NTSTATUS(NTAPI* pNtAllocateVirtualMemory)(
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T RegionSize,
    ULONG AllocationType,
    ULONG Protect
    );

// Global değişkenler - thread safe initialization
static HMODULE h_ntdll = NULL;
static pNtAllocateVirtualMemory NtAlloc = NULL;
static CRITICAL_SECTION g_cs;
static std::map<DWORD, std::wstring> g_threadDescriptions;
static ULONGLONG cachedcpufrequency = 0;

// Güvenli initialization
BOOL InitializeNtDll() {
    if (h_ntdll == NULL) {
        h_ntdll = GetModuleHandleW(L"ntdll.dll");
        if (h_ntdll == NULL) {
            return FALSE;
        }

        NtAlloc = (pNtAllocateVirtualMemory)GetProcAddress(h_ntdll, "NtAllocateVirtualMemory");
        if (NtAlloc == NULL) {
            return FALSE;
        }
    }
    return TRUE;
}

typedef struct _MEM_EXTENDED_PARAMETER {
    ULONG Type;
    union {
        ULONG64 ULong64;
        PVOID Pointer;
        SIZE_T Size;
        HANDLE Handle;
        ULONG ULong;
    };
} MEM_EXTENDED_PARAMETER;

typedef struct _COA_CPU_SET_INFORMATION {
    ULONG Size;
    ULONG Type;
    struct {
        ULONG Id;
        ULONG Group;
        ULONG LogicalProcessorIndex;
        ULONG CoreIndex;
        ULONG LastLevelCacheIndex;
        ULONG NumaNodeIndex;
        ULONG SchedulingClass;
        BOOLEAN Parked;
        BOOLEAN Allocated;
        BOOLEAN AllocatedToTargetProcess;
        BOOLEAN RealTime;
    } CoaCpuSet;
} SYSTEM_CPU_SET_INFORMATION, * PSYSTEM_CPU_SET_INFORMATION;

void CoaCpuSetInformation(PSYSTEM_CPU_SET_INFORMATION Struct_v, ULONG Req_size) {
    if (Struct_v == NULL) return;

    Struct_v->Size = Req_size;
    Struct_v->Type = 0;
    Struct_v->CoaCpuSet.Id = 0;
    Struct_v->CoaCpuSet.Group = 0;
    Struct_v->CoaCpuSet.LogicalProcessorIndex = 0;
    Struct_v->CoaCpuSet.CoreIndex = 0;
    Struct_v->CoaCpuSet.LastLevelCacheIndex = 0;
    Struct_v->CoaCpuSet.NumaNodeIndex = 0;
    Struct_v->CoaCpuSet.SchedulingClass = 0;
    Struct_v->CoaCpuSet.Parked = FALSE;
    Struct_v->CoaCpuSet.Allocated = TRUE;
    Struct_v->CoaCpuSet.AllocatedToTargetProcess = TRUE;
    Struct_v->CoaCpuSet.RealTime = FALSE;
}

PVOID AlignedNtAlloc(HANDLE process, PVOID preferredBase, SIZE_T size, SIZE_T alignment, ULONG allocType, ULONG protect) {
    if (!InitializeNtDll()) {
        return NULL;
    }

    // Input validation
    if (size == 0 || process == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }

    PVOID baseAddress = preferredBase;
    SIZE_T regionSize = size;
    ULONG_PTR zeroBits = 0;

    // alignment kontrolü - 2'nin kuvveti olmalı
    if (alignment && (alignment & (alignment - 1)) == 0) {
        DWORD shift = 0;
        SIZE_T temp = alignment;
        while (temp > 1) {
            temp >>= 1;  // bit shift daha verimli
            shift++;
        }
        zeroBits = shift;
    }
    else if (alignment != 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }

    NTSTATUS status = NtAlloc(
        process,
        &baseAddress,
        zeroBits,
        &regionSize,
        allocType,
        protect
    );

    if (status < 0) {
        // RtlNtStatusToDosError kullanabilirsiniz
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }

    return baseAddress;
}

ULONGLONG GetEstimatedCPUFrequency() {
    LARGE_INTEGER qpcFreq, qpcStart, qpcEnd;

    if (!QueryPerformanceFrequency(&qpcFreq) || qpcFreq.QuadPart == 0) {
        return 0;
    }

    QueryPerformanceCounter(&qpcStart);
    unsigned __int64 tscStart = __rdtsc();

    Sleep(100); // 100ms bekle

    unsigned __int64 tscEnd = __rdtsc();
    QueryPerformanceCounter(&qpcEnd);

    // Overflow kontrolü
    if (qpcEnd.QuadPart <= qpcStart.QuadPart || tscEnd <= tscStart) {
        return 0;
    }

    double elapsedSec = (double)(qpcEnd.QuadPart - qpcStart.QuadPart) / qpcFreq.QuadPart;
    if (elapsedSec <= 0) {
        return 0;
    }

    return (ULONGLONG)((tscEnd - tscStart) / elapsedSec);
}

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD dwReason, PVOID pvReserved) {
    UNREFERENCED_PARAMETER(pvReserved);

    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        gDllhInstance = hInstDLL;
        InitializeCriticalSection(&g_cs);
        cachedcpufrequency = GetEstimatedCPUFrequency();
        return ProcessAttach();
    case DLL_PROCESS_DETACH:
        return ProcessDetach();
    }
    return TRUE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI IsWow64Process2(HANDLE hProcess, USHORT* pProcessMachine, USHORT* pNativeMachine) {

    // Input validation
    if (hProcess == NULL) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    if (pProcessMachine)
        *pProcessMachine = IMAGE_FILE_MACHINE_I386;
    if (pNativeMachine)
        *pNativeMachine = IMAGE_FILE_MACHINE_I386;
    return TRUE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI GetVersionExW(OSVERSIONINFOW* vi) {

    if (!vi || vi->dwOSVersionInfoSize != sizeof(OSVERSIONINFOW)) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
    }

    vi->dwMajorVersion = 10;
    vi->dwMinorVersion = 0;
    vi->dwBuildNumber = 19045;
    vi->dwPlatformId = VER_PLATFORM_WIN32_NT;
    vi->szCSDVersion[0] = L'\0';
    return TRUE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI GetVersionExA(OSVERSIONINFOA* vi) {

    if (!vi || vi->dwOSVersionInfoSize != sizeof(OSVERSIONINFOA)) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
    }

    vi->dwMajorVersion = 10;
    vi->dwMinorVersion = 0;
    vi->dwBuildNumber = 19045;
    vi->dwPlatformId = VER_PLATFORM_WIN32_NT;
    vi->szCSDVersion[0] = '\0';
    return TRUE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI GetSystemWow64Directory2W(LPWSTR lpBuffer, UINT uSize, DWORD dwFlags) {
    UNREFERENCED_PARAMETER(dwFlags);
    return GetSystemWow64DirectoryW(lpBuffer, uSize);
}

extern "C" __declspec(dllexport)
BOOL WINAPI GetSystemWow64Directory2A(LPSTR lpBuffer, UINT uSize, DWORD dwFlags) {
    UNREFERENCED_PARAMETER(dwFlags);
    return GetSystemWow64DirectoryA(lpBuffer, uSize);
}

extern "C" __declspec(dllexport)
BOOL WINAPI IsWow64GuestMachineSupported(USHORT WowGuestMachine, PBOOL MachineIsSupported) {
    UNREFERENCED_PARAMETER(WowGuestMachine);

    if (MachineIsSupported)
        *MachineIsSupported = TRUE;
    return TRUE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI COAWINAPI(UINT* Status, UINT* MainVer, UINT* Ver2, UINT* Ver3) {

    if (Status) *Status = 0;    // STABLE
    if (MainVer) *MainVer = 2;
    if (Ver2) *Ver2 = 0;
    if (Ver3) *Ver3 = 0;

    return TRUE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI QueryAuxiliaryCounterFrequency(PULONGLONG pCPUFREQ) {

    if (pCPUFREQ) {
        *pCPUFREQ = cachedcpufrequency;
        return TRUE;
    }

    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI VerifyVersionInfoA(LPOSVERSIONINFOEXA lpVerInfoA, DWORD dwTypeMaskA, DWORDLONG dwlCondtMaskA) {
    UNREFERENCED_PARAMETER(lpVerInfoA);
    UNREFERENCED_PARAMETER(dwTypeMaskA);
    UNREFERENCED_PARAMETER(dwlCondtMaskA);
    return TRUE;
}

extern "C" __declspec(dllexport)
HRESULT WINAPI dfSetThreadDescription(HANDLE hThread, PCWSTR lpThreadDescription) {

    if (!hThread || !lpThreadDescription) {
        return E_INVALIDARG;
    }

    DWORD tid = GetThreadId(hThread);
    if (tid == 0) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    EnterCriticalSection(&g_cs);

    try {
        g_threadDescriptions[tid] = lpThreadDescription;
    }
    catch (...) {
        LeaveCriticalSection(&g_cs);
        return E_OUTOFMEMORY;
    }

    LeaveCriticalSection(&g_cs);
    return S_OK;
}

extern "C" __declspec(dllexport)
HRESULT WINAPI dfGetThreadDescription(HANDLE hThread, PWSTR* ppszThreadDescription) {

    if (!hThread || !ppszThreadDescription) {
        return E_INVALIDARG;
    }

    DWORD tid = GetThreadId(hThread);
    if (tid == 0) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    EnterCriticalSection(&g_cs);

    auto it = g_threadDescriptions.find(tid);
    if (it == g_threadDescriptions.end()) {
        LeaveCriticalSection(&g_cs);
        return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

    size_t len = it->second.length() + 1;
    *ppszThreadDescription = (PWSTR)LocalAlloc(LMEM_ZEROINIT, len * sizeof(WCHAR));
    if (!*ppszThreadDescription) {
        LeaveCriticalSection(&g_cs);
        return E_OUTOFMEMORY;
    }

    wcscpy_s(*ppszThreadDescription, len, it->second.c_str());
    LeaveCriticalSection(&g_cs);
    return S_OK;
}

extern "C" __declspec(dllexport)
BOOL WINAPI VerifyVersionInfoW(LPOSVERSIONINFOEXW lpVersionInformation, DWORD dwTypeMask, DWORDLONG dwlConditionMask) {
    UNREFERENCED_PARAMETER(lpVersionInformation);
    UNREFERENCED_PARAMETER(dwTypeMask);
    UNREFERENCED_PARAMETER(dwlConditionMask);
    return TRUE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI AdjustWindowRectExForDpi(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi) {
    UNREFERENCED_PARAMETER(dpi);
    return AdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle);
}

extern "C" __declspec(dllexport)
BOOL WINAPI CheckIsElevationEnabled() {
    return TRUE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI GetSystemCpuSetInformation(PSYSTEM_CPU_SET_INFORMATION Struct, ULONG BufferLength, PULONG ReturnedLength, HANDLE Process, ULONG Flags) {
    UNREFERENCED_PARAMETER(Process);
    UNREFERENCED_PARAMETER(Flags);

    if (ReturnedLength == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    ULONG requiredSize = sizeof(_COA_CPU_SET_INFORMATION);
    *ReturnedLength = requiredSize;

    if (Struct == NULL || BufferLength < requiredSize) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
    }

    ZeroMemory(Struct, requiredSize);
    CoaCpuSetInformation(Struct, requiredSize);
    return TRUE;
}

extern "C" __declspec(dllexport)
PVOID WINAPI VirtualAlloc2(HANDLE Process, PVOID BaseAddress, SIZE_T Size, ULONG AllocationType, ULONG PageProtection, MEM_EXTENDED_PARAMETER* ExtendedParameters, ULONG ParameterCount) {

    if (Size == 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }

    COA_MEM_ADDR_REQ AddressRequirements = { 0 };
    ULONG numaNode = NUMA_NO_PREFERRED_NODE;
    BOOL hasNuma = FALSE;
    BOOL hasAddrReqAlignment = FALSE;

    // Extended parameter'ları güvenli şekilde kontrol et
    for (ULONG i = 0; i < ParameterCount && ExtendedParameters != NULL; ++i) {
        switch (ExtendedParameters[i].Type) {
        case MemExtendedParameterNumaNode:
            numaNode = ExtendedParameters[i].ULong;
            hasNuma = TRUE;
            break;

        case MemExtendedParameterAddressRequirements: {
            hasAddrReqAlignment = TRUE;
            if (ExtendedParameters[i].Pointer != NULL) {
                PCOA_MEM_ADDR_REQ reqs = (PCOA_MEM_ADDR_REQ)ExtendedParameters[i].Pointer;
                AddressRequirements = *reqs;
            }
            break;
        }

        case MemExtendedParameterAttributeFlags:
            if (ExtendedParameters[i].ULong64 & (MEM_EXTENDED_PARAMETER_NONPAGED | MEM_EXTENDED_PARAMETER_NONPAGED_LARGE | MEM_EXTENDED_PARAMETER_NONPAGED_HUGE)) {
                AllocationType |= MEM_RESERVE | MEM_COMMIT;
            }
            break;

        default:
            SetLastError(ERROR_INVALID_PARAMETER);
            return NULL;
        }
    }

    if (Process == NULL) {
        Process = GetCurrentProcess();
    }

    if (hasNuma) {
        return VirtualAllocExNuma(Process, BaseAddress, Size, AllocationType, PageProtection, numaNode);
    }
    else if (hasAddrReqAlignment && AddressRequirements.Alignment > 0) {
        return AlignedNtAlloc(Process, BaseAddress, Size, AddressRequirements.Alignment, AllocationType, PageProtection);
    }
    else {
        return VirtualAllocEx(Process, BaseAddress, Size, AllocationType, PageProtection);
    }
}

extern "C" __declspec(dllexport)
PVOID WINAPI VirtualAlloc2FromApp(HANDLE Process, PVOID BaseAddress, SIZE_T Size, ULONG AllocationType, ULONG PageProtection, MEM_EXTENDED_PARAMETER* ExtendedParameters, ULONG ParameterCount) {

    // Execute yetkilerini engelle (UWP uygulamaları için)
    if (PageProtection & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) {
        SetLastError(ERROR_ACCESS_DENIED);
        return NULL;
    }

    return VirtualAlloc2(Process, BaseAddress, Size, AllocationType, PageProtection, ExtendedParameters, ParameterCount);
}

extern "C" __declspec(dllexport)
void WINAPI QueryUnbiasedInterruptTimePrecise(PULONGLONG lpUnbiasedTime) {

    if (lpUnbiasedTime) {
        ULONGLONG t = 0;
        if (QueryUnbiasedInterruptTime(&t)) {
            *lpUnbiasedTime = t;
        }
        else {
            *lpUnbiasedTime = 0;
        }
    }
}

void cpufreqmsgbox() {
    try {
        std::wstringstream wss;
        wss << cachedcpufrequency;
        std::wstring message = L"Cached CPU Frequency: " + wss.str();

        MessageBoxW(NULL, message.c_str(), L"COA 8.1 Kernel32 Debug Window", MB_ICONINFORMATION | MB_OK);
    }
    catch (...) {
        // Hata durumunda sessizce geç
    }
}

BOOL ProcessAttach() {
    starthook();
    return TRUE;
}

BOOL ProcessDetach() {
    DeleteCriticalSection(&g_cs);
    return TRUE;
}
// 5.09.25 Kod optimize edilmek üzere elden geçirildi. Gereksiz kısımlar uzun işaretli boşlu- çöpe gönderildi. Testbox bellek sıkıntılarına neden olduğu için kaldırıldı.
#define C_DLL_EXPORT             extern "C" __declspec(dllexport)
#define CALLING_CONVENTION       __stdcall