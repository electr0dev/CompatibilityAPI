#include "wldp.h"

extern "C" __declspec(dllexport) HRESULT WldpQueryWindowsLockdownMode(
	PWLDP_LOCKDOWN_MODE pLockdownMode
	) {
	if (pLockdownMode) {
		*pLockdownMode = WLDP_LOCKDOWN_OFF; // Sabit deðer, 1'a eþit, lockdown Windows 8.1'de yok.
	}
	return S_OK;
}