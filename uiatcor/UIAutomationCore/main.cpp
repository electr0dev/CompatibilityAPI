#include <Windows.h>

HINSTANCE gDllhInstance = NULL; // global storage for current dll handle

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

BOOL ProcessAttach()
{
    // Add your initialization code there
    return TRUE;
}

extern "C" __declspec(dllexport)
HRESULT WINAPI UiaRaiseNotificationEvent(
)
{
	// Windows 8.1'de native yok. Sessizce başarı dönebiliriz
	return S_OK;
}

extern "C" __declspec(dllexport)
HRESULT WINAPI UiaHostProviderFromHwnd(
)
{
	// Windows 8.1'de native yok. Sessizce başarı dönebiliriz
	return S_OK;
}

extern "C" __declspec(dllexport)
HRESULT WINAPI UiaRaiseAutomationEvent(
)
{
	// Windows 8.1'de native yok. Sessizce başarı dönebiliriz
	return S_OK;
}

extern "C" __declspec(dllexport)
HRESULT WINAPI UiaReturnRawElementProvider(
)
{
	// Windows 8.1'de native yok. Sessizce başarı dönebiliriz
	return S_OK;
}

extern "C" __declspec(dllexport)
HRESULT WINAPI UiaRaiseAutomationPropertyChangedEvent(
)
{
	// Windows 8.1'de native yok. Sessizce başarı dönebiliriz
	return S_OK;
}

extern "C" __declspec(dllexport)
HRESULT UiaClientsAreListening(BOOL* pbClientsListening) {
	if (pbClientsListening) {
		*pbClientsListening = FALSE;
	}
	return S_OK; //bir dönüş değeri beklendiği ve windows 8.1'de bu fonksiyona gerek olmadığı için false döndürüyoruz.
}

BOOL ProcessDetach()
{
    // Add your uninitialization code there
    return TRUE;
}


#define C_DLL_EXPORT             extern "C" __declspec(dllexport)
#define CALLING_CONVENTION       __stdcall

