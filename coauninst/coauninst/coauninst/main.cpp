#include <windows.h>
#include <winreg.h>

void mesajver(LPCWSTR metin){
		MessageBoxW(
        NULL,                          // Ebeveyn pencere (yoksa NULL)
        metin,             // Mesaj metni (Unicode - wide string)
        L"CompatibilityAPI Uninstaller",                     // Baþlýk (caption)
        MB_OK | MB_ICONINFORMATION     // Butonlar ve ikon tipi
        );
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    // Klasörü sil
    system("rmdir /s /q C:\\CompatibilityAPI");

	mesajver(L"Dosya islemi tamamlandi.");

    // Registry anahtarýný alt anahtarlarla birlikte sil
    BOOL bitMimari = FALSE;
    HKEY hKey;
    REGSAM accessFlags;

    // Sistem mimarisi kontrolü
    if (IsWow64Process(GetCurrentProcess(), &bitMimari) && bitMimari) {
        // 64-bit sistem, 64-bit registry'ye eriþim
        accessFlags = KEY_WRITE | KEY_WOW64_64KEY;

		mesajver(L"64 bit sistem, 64 bit registry kullanilacak.");
    } else {
        // 32-bit sistem veya 32-bit uygulama
        accessFlags = KEY_WRITE | KEY_WOW64_32KEY;

		mesajver(L"32 bit sistem, 32 bit registry kullanilacak.");
    }

    // Registry anahtarýný silme iþlemi
    RegDeleteKeyExA(
        HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\CompatibilityAPI",
        accessFlags,
        0
    ); //CurrentMajorVersion ve CurrentMinorVersion keyleri silinmiyor, uðraþmadýðýmdan kaynaklý, kendiniz silebilirsiniz.
	mesajver(L"Registry islemi tamamlandi.");

}
