#include <Windows.h>
#include <intrin.h>
#include <stdio.h>
#include <sstream>
#include <winnt.h>
#include <map>

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>

#include "ntstatus.h"
#define WIN32_NO_STATUS
#include "windef.h"
#include "winbase.h"
#include "winnls.h"
#include "winternl.h"
#include "winerror.h"
#include <string>
#include <iostream>
//#pragma comment(lib, "detours.lib")
#define nullptr 0
void testbox(LPCWSTR test);

//eski hook fonksiyonlarý
//BOOL coahook();
//BOOL coadetachhook()