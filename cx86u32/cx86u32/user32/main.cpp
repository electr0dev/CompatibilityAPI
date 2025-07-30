#include "coau32.h"
HINSTANCE gDllhInstance = NULL;

BOOL ProcessAttach();
BOOL ProcessDetach();

typedef void* DPI_AWARENESS_CONTEXT;//Sadece Windows 10'da bulunduğu için bir taklit fonksiyon.

#ifndef PROCESS_DPI_AWARENESS
typedef enum PROCESS_DPI_AWARENESS {
	DPI_AWARENESS_UNAWARE = 0,
	DPI_AWARENESS_SYSTEM_AWARE = 1,
	DPI_AWARENESS_PER_MONITOR_AWARE = 2
} PROCESS_DPI_AWARENESS;
#endif

HMODULE hShcore = LoadLibraryW(L"shcore.dll");

typedef HRESULT(WINAPI *GETPROCESSDPIAWARENESS)(HANDLE hProcess, PROCESS_DPI_AWARENESS *value); //pGetProcessDpiAwareness <- shcore.dll/GetProcessDpiAwareness
GETPROCESSDPIAWARENESS pGetProcessDpiAwareness =
(GETPROCESSDPIAWARENESS)GetProcAddress(hShcore, "GetProcessDpiAwareness");

HRESULT(WINAPI *pSetProcessDpiAwareness)(int) =
(HRESULT(WINAPI *)(int))GetProcAddress(hShcore, "SetProcessDpiAwareness"); //pSetProcessDpiAwareness <- shcore.dll/SetProcessDpiAwareness

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD dwReason, PVOID pvReserved)
{
	UNREFERENCED_PARAMETER(pvReserved);

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		gDllhInstance = hInstDLL;
		return ProcessAttach();
	case DLL_PROCESS_DETACH:
		return ProcessDetach();
	}
	return TRUE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI AdjustWindowRectExForDpi(
	LPRECT lpRect,
	DWORD dwStyle,
	BOOL bMenu,
	DWORD dwExStyle,
	UINT dpi)
{
	testbox(L"C_USR_1");
	// adjustwindowrectex() fonksiyonunu kullanacağız.
	return AdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle);
}

extern "C" __declspec(dllexport)
INT WINAPI GetSystemMetricsForDpi(int nIndex, UINT dpi) //Fonksiyonun Dpi olmayan eski versiyona göre farkı ölçeğe göre değer vermesi. Örneğin 96 DPI = X ise, 144 DPI = %150X şeklinde.
{
	testbox(L"C_USR_2");
	int base = GetSystemMetrics(nIndex); //Değerin index numarası
	return MulDiv(base, dpi, 96); //96 Windowsda %100 ölçek anlamına geliyor. Bu apide zaten "(base.dpi)/96" yada "base.(dpi/96)" şeklinde çalışıyor. Biz ilk formülü kullanıyoruz.
}

extern "C" __declspec(dllexport)
BOOL WINAPI SystemParametersInfoForDpi(
	UINT uiAction,
	UINT uiParam,
	PVOID pvParam,
	UINT fWinIni,
	UINT dpi // hedef DPI
)
{
	testbox(L"C_USR_3");
	// Ölçekle: 96 DPI temel alındı
	UINT scaledParam = MulDiv(uiParam, dpi, 96);

	return SystemParametersInfo(
		uiAction,
		scaledParam,
		pvParam,
		fWinIni
	);
}


extern "C" __declspec(dllexport)
UINT WINAPI GetDpiForWindow(HWND hwnd)
{
	testbox(L"C_USR_4");
	(void)hwnd;  // kullanılmıyor
				 // Daha yeni bir API olan GetDpiForMonitor da kullanabilirdim ancak bu daha kolay geldi.
				 // Ekranın (tüm sistemin) cihaz bağlamını al
	HDC hdc = GetDC(NULL);  // NULL → ekranın DC'si

							// DPI değerlerini al (yatay ve dikey)
	int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);  // genelde 96, 120, 144 vs.
	int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);  // genelde aynıdır

												// İşimiz bitti, DC'yi serbest bırak
	ReleaseDC(NULL, hdc);

	return dpiX;
}

extern "C" __declspec(dllexport)
BOOL WINAPI IsValidDpiAwarenessContext(DPI_AWARENESS_CONTEXT context)
{
	testbox(L"C_USR_5");
	// Eğer NULL değilse true döndürüyoruz.
	return (context != NULL);
}

extern "C" __declspec(dllexport)
BOOL WINAPI AreDpiAwarenessContextsEqual(DPI_AWARENESS_CONTEXT context1, DPI_AWARENESS_CONTEXT context2) {
	testbox(L"C_USR_6");
	return context1 == context2; // Basit karşılaştırma (==)
}

extern "C" __declspec(dllexport)
DPI_AWARENESS_CONTEXT WINAPI GetWindowDpiAwarenessContext(HWND hwnd)
{
	testbox(L"C_USR_7");
	(void)hwnd;

	PROCESS_DPI_AWARENESS awareness = DPI_AWARENESS_UNAWARE; //0

	if (pGetProcessDpiAwareness)
		pGetProcessDpiAwareness(GetCurrentProcess(), &awareness);

	// awareness → DPI_AWARENESS_CONTEXT'e çevrilecek (taklit pointer)
	switch (awareness) {
	case DPI_AWARENESS_UNAWARE:
		return (DPI_AWARENESS_CONTEXT)-1; // DPI_AWARENESS_CONTEXT_UNAWARE
	case DPI_AWARENESS_SYSTEM_AWARE:
		return (DPI_AWARENESS_CONTEXT)-2; // DPI_AWARENESS_CONTEXT_SYSTEM_AWARE
	case DPI_AWARENESS_PER_MONITOR_AWARE:
		return (DPI_AWARENESS_CONTEXT)-3; // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE
	default:
		return (DPI_AWARENESS_CONTEXT)-1;
	}
}

extern "C" __declspec(dllexport)
DPI_AWARENESS_CONTEXT WINAPI GetThreadDpiAwarenessContext(void)
{
	testbox(L"C_USR_8");

	PROCESS_DPI_AWARENESS awareness = DPI_AWARENESS_UNAWARE;
	if (pGetProcessDpiAwareness)
		pGetProcessDpiAwareness(GetCurrentProcess(), &awareness);

	// awareness → DPI_AWARENESS_CONTEXT'e çevrilecek (taklit pointer)
	switch (awareness) {
	case DPI_AWARENESS_UNAWARE:
		return (DPI_AWARENESS_CONTEXT)-1; // DPI_AWARENESS_CONTEXT_UNAWARE
	case DPI_AWARENESS_SYSTEM_AWARE:
		return (DPI_AWARENESS_CONTEXT)-2; // DPI_AWARENESS_CONTEXT_SYSTEM_AWARE
	case DPI_AWARENESS_PER_MONITOR_AWARE:
		return (DPI_AWARENESS_CONTEXT)-3; // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE
	default:
		return (DPI_AWARENESS_CONTEXT)-1;
	}
}

extern "C" __declspec(dllexport)
BOOL WINAPI SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT value)
{
	testbox(L"C_USR_9");
	HRESULT hr = E_NOTIMPL;//LoadLibrary ile shcore.dll yükleme
	if (!hShcore)
		return FALSE; //shcore.dll kontrolü

	PROCESS_DPI_AWARENESS awareness;

	if (value == (DPI_AWARENESS_CONTEXT)-1)
		awareness = DPI_AWARENESS_UNAWARE; //DPI_AWARENESS_UNAWARE
	else if (value == (DPI_AWARENESS_CONTEXT)-2)
		awareness = DPI_AWARENESS_SYSTEM_AWARE; //DPI_AWARENESS_SYSTEM_AWARE
	else if (value == (DPI_AWARENESS_CONTEXT)-3)
		awareness = DPI_AWARENESS_PER_MONITOR_AWARE;//DPI_AWARENESS_PER_MONITOR_AWARE
	else
		awareness = DPI_AWARENESS_UNAWARE; //DPI_AWARENESS_UNAWARE | fallback

	BOOL result = FALSE;

	if (pSetProcessDpiAwareness)
		result = SUCCEEDED(pSetProcessDpiAwareness(awareness));

	FreeLibrary(hShcore);
	return result;
}  //Daha sonra get apilerine de gerekli düzenleme yapılacak.


extern "C" __declspec(dllexport)
BOOL WINAPI EnableNonClientDpiScaling(HWND hWnd)
{
	testbox(L"C_USR_10");
	return TRUE;
}

extern "C" __declspec(dllexport)
BOOL WINAPI IsWindowArranged(HWND hwnd)
{
	if (hwnd == NULL)
	{
		// Eğer geçersiz bir pencere handle'ı varsa, düzenlenmiş sayılmaz.
		return FALSE;
	}

	// Burada pencerenin düzenlendiği varsayılır.
	// Gerçek işlevsellik burada yapılabilir (örneğin pencere görünürlük durumu).

	// Pencerenin görünür olup olmadığını kontrol edelim.
	if (IsWindowVisible(hwnd))
	{
		// Eğer pencere görünürse, düzenlenmiş kabul edebiliriz.
		return TRUE;
	}

	// Eğer pencere görünür değilse, düzenlenmemiş kabul edebiliriz.
	return FALSE;
}

BOOL ProcessAttach()
{
	disabletestbox();
	return TRUE;
}

BOOL ProcessDetach()
{
	return TRUE;
}


#define C_DLL_EXPORT             extern "C" __declspec(dllexport)
#define CALLING_CONVENTION       __stdcall

//Yine gözümüz yükseklerde, hayat geçiyo' perde perde, doydum artık bana müsade, bir yer bulalım dünyadan üzak, bir yer bulalım dünyadan uzak
//
