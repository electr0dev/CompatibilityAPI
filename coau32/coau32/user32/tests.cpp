#include "coau32.h"

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
			L"COA Testbuild Debug",             // Ba�l�k �ubu�u
			MB_OK | MB_ICONINFORMATION // Butonlar ve ikon
			);
	}
	
}
