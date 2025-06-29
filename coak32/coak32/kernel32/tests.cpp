#include "coak32.h"

void testbox(LPCWSTR test){
	MessageBoxW(
		NULL,                 // Parent window
		test,     // Mesaj metni
		L"COA Testbuild Debug",             // Baþlýk çubuðu
		MB_OK | MB_ICONINFORMATION // Butonlar ve ikon
		);
}