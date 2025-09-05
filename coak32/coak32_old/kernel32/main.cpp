/*
electr0dev 16/06/25

Kernel32.dll
DllMain.cpp

Bu dosya Win8.1 Kernel32'si için bir proxy niteliğinde olan coak32 isimli kitaplığın ana dosyasıdır.
*/

#include "coak32.h"

HINSTANCE gDllhInstance = NULL;
BOOL ProcessAttach();
BOOL ProcessDetach();

typedef struct COA_MEM_ADD_REQ {
	PVOID LowestStartingAddress;
	PVOID HighestEndingAddress;
	SIZE_T Alignment;
} COA_MEM_ADDR_REQ, *PCOA_MEM_ADDR_REQ;

typedef NTSTATUS(NTAPI* pNtAllocateVirtualMemory)(
	HANDLE ProcessHandle,
	PVOID* BaseAddress,
	ULONG_PTR ZeroBits,             // Alignment burada uygulanır
	PSIZE_T RegionSize,
	ULONG AllocationType,
	ULONG Protect
);

HMODULE h_ntdll = GetModuleHandleW(L"ntdll.dll");//ntdll yukleme
auto NtAlloc = (pNtAllocateVirtualMemory)GetProcAddress(h_ntdll, "NtAllocateVirtualMemory"); // Yapi tanimlama

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

typedef struct _COA_CPU_SET_INFORMATION { //CPU SET INFORMATION struct'u.
	ULONG Size;                            // Yapının boyutu
	ULONG Type;                            // CPU set bilgisi tipi (basit)
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
	} CoaCpuSet;                   // Gerçek zamanlı mı?
} SYSTEM_CPU_SET_INFORMATION, *PSYSTEM_CPU_SET_INFORMATION;

void CoaCpuSetInformation(
PSYSTEM_CPU_SET_INFORMATION Struct_v,
ULONG Req_size)
{
	Struct_v->Size = Req_size;
	Struct_v->Type = 0;
	Struct_v->CoaCpuSet.Id = 0;                      // CPU küme ID: 0
	Struct_v->CoaCpuSet.Group = 0;                   // CPU grubu 0 (varsayılan)
	Struct_v->CoaCpuSet.LogicalProcessorIndex = 0;   // İşlemci 0
	Struct_v->CoaCpuSet.CoreIndex = 0;
	Struct_v->CoaCpuSet.LastLevelCacheIndex = 0;
	Struct_v->CoaCpuSet.NumaNodeIndex = 0;
	Struct_v->CoaCpuSet.SchedulingClass = 0;

	Struct_v->CoaCpuSet.Parked = FALSE;              // İşlemci aktif mi? (evet)
	Struct_v->CoaCpuSet.Allocated = TRUE;            // Kullanılabilir mi? (evet)
	Struct_v->CoaCpuSet.AllocatedToTargetProcess = TRUE; // Bu işleme atanmış mı?
	Struct_v->CoaCpuSet.RealTime = FALSE;            // Gerçek zamanlı değil.

}

//CPU Frekansı gerekli olan API(ler) için cacheleniyor.

static CRITICAL_SECTION g_cs;
static std::map<DWORD, std::wstring> g_threadDescriptions;

PVOID AlignedNtAlloc(HANDLE process, PVOID preferredBase, SIZE_T size, SIZE_T alignment, ULONG allocType, ULONG protect) {
	// NtAllocateVirtualMemory fonksiyonunu al
	if (!h_ntdll) return FALSE;
	if (!NtAlloc) return FALSE;

	PVOID baseAddress = preferredBase;
	SIZE_T regionSize = size;
	ULONG_PTR zeroBits = 0;

	// alignment 2’nin kuvveti mi? kontrol et
	if (alignment && (alignment & (alignment - 1)) == 0) {
		// alignment kaç bit? onu bul (örneğin 0x100000 → 20)
		DWORD shift = 0;
		SIZE_T temp = alignment;
		while (temp > 1) {
			temp = temp / 2;   // her seferinde yarıya indir
			shift++;           // kaç defa böldüğümüzü say
		}
		zeroBits = shift;
	}
	else{
		//testbox(L"AlignedNtAlloc basarisiz. Alignment degeri ikinin kuvveti değil.");
		return FALSE;
	}

	// Belleği ayır
	NTSTATUS status = NtAlloc(
		process,
		&baseAddress,
		zeroBits,        // hizalama burada
		&regionSize,
		allocType,
		protect
	);

	if (status < 0) {
		//SetLastError(RtlNtStatusToDosError(status));
		//testbox(L"NtAllocateVirtualMemory Hata!");
		return FALSE;
	}

	return baseAddress;
}

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
		gDllhInstance = hInstDLL;
		InitializeCriticalSection(&g_cs);
		return ProcessAttach();
	case DLL_PROCESS_DETACH:
		return ProcessDetach();
	}
	return TRUE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI IsWow64Process2(HANDLE hProcess, USHORT* pProcessMachine, USHORT* pNativeMachine) {
	coabekle(1);
	//Her zaman 32-bit olarak kabul edilir
	if (pProcessMachine)
		*pProcessMachine = IMAGE_FILE_MACHINE_I386; // Sabit: 32 bit
	if (pNativeMachine)
		*pNativeMachine = IMAGE_FILE_MACHINE_I386;
	return TRUE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI GetVersionExW(OSVERSIONINFOW* vi)
{
	coabekle(2);//firefox test
	vi->dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
	vi->dwMajorVersion = 10;
	vi->dwMinorVersion = 0;
	vi->dwBuildNumber = 19045;
	vi->dwPlatformId = VER_PLATFORM_WIN32_NT;
	vi->szCSDVersion[0] = 0;
	return TRUE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI GetVersionExA(OSVERSIONINFOA* vi)
{
	coabekle(1); //firefox test
	if (!vi || vi->dwOSVersionInfoSize != sizeof(OSVERSIONINFOA))
		return FALSE;
	vi->dwMajorVersion = 10;
	vi->dwMinorVersion = 0;
	vi->dwBuildNumber = 19045;
	vi->dwPlatformId = VER_PLATFORM_WIN32_NT;
	vi->szCSDVersion[0] = 0;
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
	coabekle(1);
	// dwFlags şu anlık dikkate alınmıyor
	return GetSystemWow64DirectoryW(lpBuffer, uSize);
}

extern "C" __declspec(dllexport)
BOOL WINAPI GetSystemWow64Directory2A(
LPSTR lpBuffer,
UINT uSize,
DWORD dwFlags
) {
	coabekle(1);
	
	// dwFlags şu anlık dikkate alınmıyor
	return GetSystemWow64DirectoryA(lpBuffer, uSize);
}

extern "C" __declspec(dllexport)
BOOL WINAPI IsWow64GuestMachineSupported(
USHORT WowGuestMachine,
PBOOL MachineIsSupported
) {
	coabekle(1);
	
	if (MachineIsSupported)
		*MachineIsSupported = TRUE;

	return TRUE;
} //x86 da x64 de destekliyor.

extern "C" __declspec(dllexport)
BOOL WINAPI COAWINAPI(UINT Status, UINT MainVer, UINT Ver2, UINT Ver3){
	if (Status){
		//RC=0 STABLE=1
		Status = 0;
		MainVer = 2; //Ana sürüm
		Ver2 = 0;//Bir alt sürüm
		Ver3 = 0;//Bir alt sürümün altındaki sürüm
	}
	coabekle(1);
	
	return TRUE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI QueryAuxiliaryCounterFrequency(PULONGLONG pCPUFREQ) {
	coabekle(1);
	if (pCPUFREQ) {
		*pCPUFREQ = cachedcpufrequency;
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
	coabekle(1);
	return TRUE;
}



extern "C" __declspec(dllexport)
HRESULT WINAPI dfSetThreadDescription(HANDLE hThread, PCWSTR lpThreadDescription)
{
	coabekle(1);
	if (!hThread || !lpThreadDescription)
		return E_INVALIDARG;

	DWORD tid = GetThreadId(hThread);
	if (tid == 0)
		return HRESULT_FROM_WIN32(GetLastError());

	EnterCriticalSection(&g_cs); // Mutex yerine Critical Section kullanıyoruz

	g_threadDescriptions[tid] = lpThreadDescription;

	LeaveCriticalSection(&g_cs); // Mutex yerine Critical Section kullanıyoruz
	return S_OK;
}

extern "C" __declspec(dllexport)
HRESULT WINAPI dfGetThreadDescription(HANDLE hThread, PWSTR* ppszThreadDescription)
{
	coabekle(1);
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
extern "C" __declspec(dllexport)
BOOL WINAPI VerifyVersionInfoW(
	LPOSVERSIONINFOEXW lpVersionInformation,
	DWORD dwTypeMask,
	DWORDLONG dwlConditionMask
	) {
	coabekle(1);
	// Sadece her zaman "bu sistem uygundur" de
	return TRUE;
}

extern "C" __declspec(dllexport)
BOOL AdjustWindowRectExForDpi(
	LPRECT lpRect,
	DWORD dwStyle,
	BOOL bMenu,
	DWORD dwExStyle,
	UINT dpi)
{
	coabekle(1);
	// adjustwindowrectex() fonksiyonunu kullanacağız.
	return AdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle);
}

extern "C" __declspec(dllexport)
BOOL WINAPI CheckIsElevationEnabled() //UAC sürekli açık. Eğer kendiniz sisteminizi UAC kapalı şekilde kullanıyorsanız false olarak duzenleyip derleyin. Kullanıcı kitlesinin çoğu UAC ayarını olduğu gibi bırakıyor.
{
	coabekle(1);
	return TRUE; //Üsttede açıklama yaptım ama bu fonksiyon ileride değiştirilebilir bunu da ekleyeyim uygulamaya göre kontrol yapan bir şey yazmayı planlıyorum.
}

extern "C" __declspec(dllexport)
BOOL WINAPI GetSystemCpuSetInformation(
	PSYSTEM_CPU_SET_INFORMATION Struct,
	ULONG BufferLength,
	PULONG ReturnedLength,
	HANDLE Process,
	ULONG Flags
	) {
	coabekle(1);
	// Eğer dönecek bilgi yoksa hata döndür.
	if (ReturnedLength == NULL) return FALSE;

	// Emülasyonun boyutunu alalım (emülasyona uygun)
	ULONG requiredSize = sizeof(_COA_CPU_SET_INFORMATION);
	*ReturnedLength = requiredSize;
	
	// Eğer sağlanan buffer yeterli değilse hata döndür.
	if (Struct == NULL || BufferLength < requiredSize) {
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		return FALSE;
	}

	// Belleği temizle (sıfırla).
	ZeroMemory(Struct, requiredSize);

	CoaCpuSetInformation(Struct, requiredSize);
	return TRUE;
}

extern "C" __declspec(dllexport)
PVOID WINAPI VirtualAlloc2(
	HANDLE Process,
	PVOID BaseAddress,
	SIZE_T Size,
	ULONG AllocationType,
	ULONG PageProtection,
	MEM_EXTENDED_PARAMETER* ExtendedParameters,
	ULONG ParameterCount
)
{
	coabekle(1);
	COA_MEM_ADDR_REQ AddressRequirements; //MEM_ADDRESS_REQUIREMENTS
	//testbox(L"VirtualAlloc2 kullaniliyor!");
	ULONG numaNode = NUMA_NO_PREFERRED_NODE; // default
	BOOL hasNuma = FALSE;
	BOOL hasNonPaged = FALSE;
	BOOL hasLargePage = FALSE;
	BOOL hasPhysical = FALSE;
	BOOL hasTopDown = FALSE;
	BOOL hasAddrReq = FALSE;
	BOOL hasAddrReqAlignment = FALSE;
	// Extended parameter'ları kontrol et
	for (ULONG i = 0; i < ParameterCount; ++i) {
		switch (ExtendedParameters[i].Type) {
		case MemExtendedParameterNumaNode:
			numaNode = ExtendedParameters[i].ULong;
			hasNuma = TRUE;
			//testbox(L"MemExtendedParameterNumaNode kullanıldı!");
			break;

		case MemExtendedParameterAddressRequirements: {
			// Windows 8.1 desteklemiyor, yoksay
			//Son 3 bit birinde 111, diğerinde 000 ise 0 döner, buda çift sayılarda geçerlidir. Çift sayılardan 1 eksiltince bit değeri 0xXXX111 gibi olurken stok halinde 0xXXX000 gibidir.
			hasAddrReq = TRUE;
			PCOA_MEM_ADDR_REQ reqs = (PCOA_MEM_ADDR_REQ)ExtendedParameters[i].Pointer;
			AddressRequirements = *reqs;
			if (AddressRequirements.Alignment) {
				hasAddrReqAlignment = TRUE;
			}
			//testbox(L"MemExtendedParameterAddressRequirements kullanıldı (LowestStartingAddress ve HighestEndingAdress desteklenmiyor)!"); //Alignment için destek eklemek zor değildi ancak deneysel olduğu için şimdilik kalsın şeklinde hareket ediyorum.
			break;
		}

		case MemExtendedParameterAttributeFlags:
			// Non-paged bellek bayraklarını kontrol et
			if (ExtendedParameters[i].ULong64 & MEM_EXTENDED_PARAMETER_NONPAGED) { //örneğin toplam 0x3 ise Nonpaged biti set edilmiş mi ona bakar. && operatörü ile karıştırılmamalıdır. 
				hasNonPaged = TRUE;
				//testbox(L"Non-paged memory oluşturuluyor!");
			}
			if (ExtendedParameters[i].ULong64 & MEM_EXTENDED_PARAMETER_NONPAGED_LARGE) {
				hasNonPaged = TRUE;
				//testbox(L"Büyük non-paged memory oluşturuluyor!");
			}
			if (ExtendedParameters[i].ULong64 & MEM_EXTENDED_PARAMETER_NONPAGED_HUGE) {
				hasNonPaged = TRUE;
				//testbox(L"Devasa non-paged memory oluşturuluyor!");
			}
			break;

		case MemExtendedParameterPhysicalMemory:
			hasPhysical = TRUE;
			//testbox(L"Fiziksel memory oluşturuluyor!");
			break;

		case MemExtendedParameterTopDown:
			hasTopDown = TRUE;
			//testbox(L"Top-down memory oluşturuluyor!");
			break;

		default:
			// Bilinmeyen parametre türü
			//testbox(L"Unknown MemExtendedParameter Type kullanıldı!");
			SetLastError(ERROR_INVALID_PARAMETER);
			return NULL;
		}
	}

	if (Process == NULL)
		Process = GetCurrentProcess();

	// Non-paged bellek istendiğinde, uygun şekilde allocate et
	if (hasNonPaged) {
		AllocationType |= MEM_RESERVE | MEM_COMMIT;  // Non-paged bellek için bu bayrakları ekliyoruz
	}

	if (hasNuma) {
		// NUMA-aware allocate et
		return VirtualAllocExNuma(
			Process,
			BaseAddress,
			Size,
			AllocationType,
			PageProtection,
			numaNode
		);
	}
	else if (hasAddrReqAlignment) {
		return AlignedNtAlloc(
			Process,
			BaseAddress,
			Size,
			AddressRequirements.Alignment,
			AllocationType,
			PageProtection
		);
	}
	else {
		// NUMA bilgisi yoksa normal allocate et
			return VirtualAllocEx(Process, BaseAddress, Size, AllocationType, PageProtection);
	}
}

extern "C" __declspec(dllexport)
PVOID WINAPI VirtualAlloc2FromApp(
HANDLE                 Process,
PVOID                  BaseAddress,
SIZE_T                 Size,
ULONG                  AllocationType,
ULONG                  PageProtection,
MEM_EXTENDED_PARAMETER *ExtendedParameters,
ULONG                  ParameterCount //dotexe1337'nin implementasyonuna göre gerekli düzenlemeler yapıldı, teşekkürler dotexe1337.
){
	coabekle(1);
	if (PageProtection & (PAGE_EXECUTE | PAGE_EXECUTE_READ |
		PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) {
		//setlasterror kullanmadığımın farkındayım. deneysel.
		return NULL;
	}
	return VirtualAlloc2(
		Process,
		BaseAddress,
		Size,
		AllocationType,
		PageProtection,
		ExtendedParameters,
		ParameterCount
	);
}


// Stub for Windows 8.1, using QueryUnbiasedInterruptTime fallback
extern "C" __declspec(dllexport)
void WINAPI QueryUnbiasedInterruptTimePrecise(PULONGLONG lpUnbiasedTime) {
	coabekle(3);
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

/*	HANDLE Process,
	PVOID BaseAddress,
	SIZE_T Size,
	ULONG AllocationType,
	ULONG PageProtection,
	MEM_EXTENDED_PARAMETER* ExtendedParameters,
	ULONG ParameterCount*/

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
	coabekle(1);
	//disabletestbox();
	starthook(); //belki komple MinHook'a geçebilirim, sadece test şuanlık
	return TRUE;
}

BOOL ProcessDetach()
{
	DeleteCriticalSection(&g_cs);
	return TRUE;
}


#define C_DLL_EXPORT             extern "C" __declspec(dllexport)
#define CALLING_CONVENTION       __stdcall

