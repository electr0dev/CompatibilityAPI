#include <windows.h>
#include <winreg.h>

void mesajver(LPCWSTR metin){
		MessageBoxW(
        NULL,                          // Ebeveyn pencere (yoksa NULL)
        metin,             // Mesaj metni (Unicode - wide string)
        L"CompatibilityAPI Uninstaller",                     // Ba�l�k (caption)
        MB_OK | MB_ICONINFORMATION     // Butonlar ve ikon tipi
        );
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    // Klas�r� sil
    system("rmdir /s /q C:\\CompatibilityAPI");

	mesajver(L"Dosya islemi tamamlandi.");

    // Registry anahtar�n� alt anahtarlarla birlikte sil
    BOOL bitMimari = FALSE;
    HKEY hKey;
    REGSAM accessFlags;

    // Sistem mimarisi kontrol�
    if (IsWow64Process(GetCurrentProcess(), &bitMimari) && bitMimari) {
        // 64-bit sistem, 64-bit registry'ye eri�im
        accessFlags = KEY_WRITE | KEY_WOW64_64KEY;

		mesajver(L"64 bit sistem, 64 bit registry kullanilacak.");
    } else {
        // 32-bit sistem veya 32-bit uygulama
        accessFlags = KEY_WRITE | KEY_WOW64_32KEY;

		mesajver(L"32 bit sistem, 32 bit registry kullanilacak.");
    }

    // Registry anahtar�n� silme i�lemi
    RegDeleteKeyExA(
        HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\CompatibilityAPI",
        accessFlags,
        0
    ); //CurrentMajorVersion ve CurrentMinorVersion keyleri silinmiyor, u�ra�mad���mdan kaynakl�, kendiniz silebilirsiniz.
	mesajver(L"Registry islemi tamamlandi.");

}
