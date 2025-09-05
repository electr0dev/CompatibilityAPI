#include "coak32.h"

BOOL dstb = 0;
void disabletestbox(){
	dstb += 1;
}
void testbox(LPCWSTR test){
	if (dstb){
		
	}
	else {
		MessageBoxW(
			NULL,                 // Parent window
			test,     // Mesaj metni
			L"COA Testbuild Debug",             // Baþlýk çubuðu
			MB_OK | MB_ICONINFORMATION // Butonlar ve ikon
			);
	}
	
}
void coabekle(int a) {
	if (a == 2) {
		Sleep(250);
		testbox(L"coabekle(2)");
	}
	else if (a == 1) {
		testbox(L"coabekle(1)");
		Sleep(250);
	}
	else if (a == 3) {
		testbox(L"coabekle(3)");
		Sleep(250);
	}
}