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
#include "MinHook.h"
//#pragma comment(lib, "detours.lib")

// Parametre türleri (Type alanýna yazýlýr)
#define MemExtendedParameterNumaNode               1
#define MemExtendedParameterAddressRequirements    2
#define MemExtendedParameterAttributeFlags         3
#define MemExtendedParameterAlignment              4
#define MemExtendedParameterPhysicalMemory         5
#define MemExtendedParameterTopDown                6

// AttributeFlags bayraklarý (ULong64 alanýnda kullanýlýr)
#define MEM_EXTENDED_PARAMETER_NONPAGED         0x02
#define MEM_EXTENDED_PARAMETER_NONPAGED_LARGE   0x08
#define MEM_EXTENDED_PARAMETER_NONPAGED_HUGE    0x10
#define MEM_EXTENDED_PARAMETER_EC_CODE          0x40

// NUMA default deðeri
#define NUMA_NO_PREFERRED_NODE     ((ULONG)(-1))

void testbox(LPCWSTR test);
void disabletestbox();
void starthook();

//eski hook fonksiyonlarý
//BOOL coahook();
//BOOL coadetachhook()