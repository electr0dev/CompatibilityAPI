#include <Windows.h>
#include <shcore.h>
void testbox(LPCWSTR test);
#ifndef DISABLE_TESTBOX
void disabletestbox();
#endif

typedef HANDLE HSYNTHETICPOINTERDEVICE;

typedef enum tagPOINTER_FEEDBACK_MODE {
	POINTER_FEEDBACK_DEFAULT = 1,
	POINTER_FEEDBACK_INDIRECT = 2,
	POINTER_FEEDBACK_NONE = 3
} POINTER_FEEDBACK_MODE;

//test için eklendi