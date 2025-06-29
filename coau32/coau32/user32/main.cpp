#include <Windows.h>

HINSTANCE gDllhInstance = NULL; 

BOOL ProcessAttach();
BOOL ProcessDetach();



BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD dwReason, PVOID pvReserved)
{
    UNREFERENCED_PARAMETER(pvReserved);

    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        gDllhInstance =hInstDLL;
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
	// adjustwindowrectex() fonksiyonunu kullanacağız.
	return AdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle);
}

extern "C" __declspec(dllexport)
INT WINAPI GetSystemMetricsForDpi(int nIndex, UINT dpi) //Fonksiyonun Dpi olmayan eski versiyona göre farkı ölçeğe göre değer vermesi. Örneğin 96 DPI = X ise, 144 DPI = %150X şeklinde.
{
	int base = GetSystemMetrics(nIndex); //Değerin index numarası
	return MulDiv(base, dpi, 96); //96 Windowsda %100 ölçek anlamına geliyor. Bu apide zaten "(base.dpi)/96" yada "base.(dpi/96)" şeklinde çalışıyor. Biz ilk formülü kullanıyoruz.
}

BOOL ProcessAttach()
{
    return TRUE;
}

BOOL ProcessDetach()
{
    return TRUE;
}


#define C_DLL_EXPORT             extern "C" __declspec(dllexport)
#define CALLING_CONVENTION       __stdcall

//Yine gözümüz yükseklerde, hayat geçiyo' perde perde, doydum artık bana müsade, bir yer bulalım dünyadan üzak, bir yer bulalım dünyadan uzak

