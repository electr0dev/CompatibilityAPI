#include <windows.h>
#include <direct.h>
//normalde konsol uygulamas� olarak yazacakt�m, konsol penceresi a�madan �al��t��� i�in bu daha mant�kl� geldi.

DWORD majorVersion = 10;  // CurrentMajorVersion i�in say�sal de�er
DWORD minorVersion = 0;   // CurrentMinorVersion i�in say�sal de�er

void mesajver(LPCWSTR cumle) {
    MessageBox(NULL, cumle, L"CompatibilityAPI Setup", MB_OK | MB_ICONINFORMATION);
}

void olustur_klasor() {
    _mkdir("C:\\CompatibilityAPI");
    _mkdir("C:\\CompatibilityAPI\\tmprun");
    mesajver(L"Klasorler olusturuldu.");
}

void kopyala_dosya(){
	CopyFileA("coak32.dll", "C:\\CompatibilityAPI\\coak32.dll", FALSE);
    CopyFileA("cx86k32.dll", "C:\\CompatibilityAPI\\cx86k32.dll", FALSE);
    CopyFileA("coau32.dll", "C:\\CompatibilityAPI\\coau32.dll", FALSE);
	CopyFileA("cx86u32.dll", "C:\\CompatibilityAPI\\cx86u32.dll", FALSE);
    CopyFileA("coaload.exe", "C:\\CompatibilityAPI\\coaload.exe", FALSE);
	CopyFileA("user.bat", "C:\\CompatibilityAPI\\user.bat", FALSE);
	CopyFileA("coauninst.exe", "C:\\CompatibilityAPI\\coauninst.exe", FALSE);
	CopyFileA("uiatcor.dll", "C:\\CompatibilityAPI\\uiatcor.dll", FALSE);
	CopyFileA("ui86atcor.dll", "C:\\CompatibilityAPI\\ui86atcor.dll", FALSE);
	CopyFileA("MinHook.x86.dll", "C:\\CompatibilityAPI\\MinHook.x86.dll", FALSE);
	CopyFileA("MinHook.x64.dll", "C:\\CompatibilityAPI\\MinHook.x64.dll", FALSE);//MinHook projesine te�ekk�rler.
	CopyFileA("coasetup.exe", "C:\\CompatibilityAPI\\coasetup.exe", FALSE);
	DWORD kopyalamaislemihataKodu = GetLastError();
	LPWSTR kopyalahatakodyazdirilabilir = nullptr;
	FormatMessageW(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    NULL, kopyalamaislemihataKodu,
    0, (LPWSTR)&kopyalahatakodyazdirilabilir, 0, NULL);
	mesajver(kopyalahatakodyazdirilabilir); //buradan sonra bir cikis mesaji eklemedim, e�er "sistem belirtilen dosyay� bulam�yor" ise setubun bulundu�u klas�rde kopyalanacak dosyalar� bulamam��t�r.
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {

	LANGID dilid = GetUserDefaultUILanguage();
    LPCWSTR EN = L"Are you sure to continue with the setup?";
	LPCWSTR TR = L"Kuruluma devam etmek istedi�inize emin misiniz?";
    LPCWSTR dilstring = nullptr;


	if (dilid == 0x041f){ //t�rk�e dil kodu
		dilstring = TR;
	 }
      
	else {
		dilstring = EN; //�ngilizceyi de kendi dil koduyla ekleyip birka� dil daha eklerdim de gerek g�rmedim �u anl�k. Belki eklerim.
	 }

    int msgid = MessageBox(NULL, dilstring, L"CompatibilityAPI Setup", MB_OKCANCEL | MB_ICONWARNING);

	switch (msgid)
    {
    case IDCANCEL:

        return FALSE; //0

    case IDOK:
    HKEY hKey;
    BOOL bitMimari = 0;
    
    // Sistemin bit mimarisini kontrol et
	HKEY hKey1, hKey2;
	BOOL is64Bit = FALSE;

	// Sistem mimarisini kontrol et
	if (IsWow64Process(GetCurrentProcess(), &is64Bit) && is64Bit) {
		// === 1. CompatibilityAPI i�in 64-bit uninstall anahtar� ===
		if (RegCreateKeyExA(HKEY_LOCAL_MACHINE,
			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\CompatibilityAPI",
			0, NULL, 0,
			KEY_WRITE | KEY_WOW64_64KEY,
			NULL, &hKey1, NULL) == ERROR_SUCCESS) {
			RegSetValueExA(hKey1, "DisplayName", 0, REG_SZ, (BYTE*)"CompatibilityAPI WinAPI Extensions For Windows 8.1", 65);
			RegSetValueExA(hKey1, "Publisher", 0, REG_SZ, (BYTE*)"electr0dev | CompatibilityAPI Project", 40);
			RegSetValueExA(hKey1, "UninstallString", 0, REG_SZ, (BYTE*)"C:\\CompatibilityAPI\\coauninst.exe", 33);
			RegSetValueExA(hKey1, "DisplayVersion", 0, REG_SZ, (BYTE*)"V2 rc0.2", 10);
			RegCloseKey(hKey1);
		}

		// === 2. Windows NT CurrentVersion alt�ndaki girdileri g�ncelle ===
		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
			"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
			0,
			KEY_SET_VALUE | KEY_WOW64_64KEY,
			&hKey2) == ERROR_SUCCESS) {

			RegSetValueExA(hKey2, "CurrentMajorVersionNumber", 0, REG_DWORD, (BYTE*)&majorVersion, sizeof(majorVersion));
			RegSetValueExA(hKey2, "CurrentMinorVersionNumber", 0, REG_DWORD, (BYTE*)&minorVersion, sizeof(minorVersion));
			RegCloseKey(hKey2);
		}

		mesajver(L"64-bit registry i�lemleri ba�ar�l�.");

	}
	else {
		// === 1. CompatibilityAPI i�in 32-bit uninstall anahtar� ===
		if (RegCreateKeyExA(HKEY_LOCAL_MACHINE,
			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\CompatibilityAPI",
			0, NULL, 0,
			KEY_WRITE | KEY_WOW64_32KEY,
			NULL, &hKey1, NULL) == ERROR_SUCCESS) {
			RegSetValueExA(hKey1, "DisplayName", 0, REG_SZ, (BYTE*)"CompatibilityAPI WinAPI Extensions For Windows 8.1", 65);
			RegSetValueExA(hKey1, "Publisher", 0, REG_SZ, (BYTE*)"electr0dev | CompatibilityAPI Project", 40);
			RegSetValueExA(hKey1, "UninstallString", 0, REG_SZ, (BYTE*)"C:\\CompatibilityAPI\\coauninst.exe", 33);
			RegSetValueExA(hKey1, "DisplayVersion", 0, REG_SZ, (BYTE*)"V2 rc0.2", 10);
			RegCloseKey(hKey1);
		}

		// === 2. Windows NT CurrentVersion alt�ndaki girdileri g�ncelle ===
		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
			"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
			0,
			KEY_SET_VALUE | KEY_WOW64_32KEY,
			&hKey2) == ERROR_SUCCESS) {

			RegSetValueExA(hKey2, "CurrentMajorVersionNumber", 0, REG_DWORD, (BYTE*)&majorVersion, sizeof(majorVersion));
			RegSetValueExA(hKey2, "CurrentMinorVersionNumber", 0, REG_DWORD, (BYTE*)&minorVersion, sizeof(minorVersion));
			RegCloseKey(hKey2);
		}

		mesajver(L"32-bit registry i�lemleri ba�ar�l�.");
	}
//uninstall girdileri. rc0.2 den itibaren bulunur.
		olustur_klasor();
		const char* kisayolkomut = 
        "powershell \"$s=(New-Object -ComObject WScript.Shell).CreateShortcut([Environment]::GetFolderPath('Desktop')+'\\CompatibilityAPI shell.lnk');"
        "$s.TargetPath='C:\\CompatibilityAPI\\user.bat';"
        "$s.Save()\"";
		system(kisayolkomut);
		kopyala_dosya();
    }
	   
  }
//G�zel g�nler ge�ti �abucak, yak�nda bir f�rt�na kopacak, g�vendi�in da�lar �nce seni yutacak



