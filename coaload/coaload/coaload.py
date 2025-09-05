import pefile
import sys
import os
import struct
import shutil
import subprocess
import time

arc = struct.calcsize("P") * 8

lri = 0

def patch_dll_name(input_path, output_path, dll_map, prog_dir, b=0): #b/i2/a/i şeklinde yönlendirilen işlem/işleme alınan satır numarası
    pe = pefile.PE(input_path)
    print(f"/P{b} |", arc, "Bit\n---------------------------") #önceden sabit p1 döndüren değer değiştirilip dinamik hale getirildi. Yine de daha sonra tekrar bu alanı farklı şekilde değerlendirmeyi düşünmüyorum değil.
    # Import Table kontrolü
    if not hasattr(pe, 'DIRECTORY_ENTRY_IMPORT'):
        print("Import tablosu bulunamadı.")
        return

    for entry in pe.DIRECTORY_ENTRY_IMPORT:
        orig_dll = entry.dll.decode('utf-8')
        if orig_dll.lower() in dll_map:
            new_dll = dll_map[orig_dll.lower()]
            print(f"İmport tablosu değiştiriliyor: {orig_dll} -> {new_dll}")

            # Yeni DLL adını byte dizisine çevir
            new_dll_bytes = new_dll.encode('utf-8') + b'\x00'

            dll_rva = entry.struct.Name
            dll_offset = pe.get_offset_from_rva(dll_rva)
            pe.set_bytes_at_offset(dll_offset, new_dll_bytes)
            print("Optional Header islemi tamamlandi.")
            #MajorSubsystemVersion ve MajorOperatingSystemVersion alanını değiştir
            offset_os = pe.OPTIONAL_HEADER.get_field_absolute_offset("MajorOperatingSystemVersion")
            # MajorSubsystemVersion offset
            offset_ss = pe.OPTIONAL_HEADER.get_field_absolute_offset("MajorSubsystemVersion")

            # Eğer 10 ise 6 yap
            
            pe.set_bytes_at_offset(offset_os, (6).to_bytes(2, byteorder="little"))

            
            pe.set_bytes_at_offset(offset_ss, (6).to_bytes(2, byteorder="little"))#DWORD(2byte)
              
    pe.write(output_path)
    pe.close()

    print(f"Yeni dosya yazıldı: {output_path}")
    prgtmpd = prog_dir + "\\" + output_path
    coak32tmpd = prog_dir + "\\" + "coak32.dll"
    coawow64k32tmpd = prog_dir + "\\" + "cx86k32.dll"
    coau32tmpd = prog_dir + "\\" + "coau32.dll"
    coawow64u32tmpd = prog_dir + "\\" + "cx86u32.dll"
    coa_uiatcor_tmpd = prog_dir + "\\" + "uiatcor.dll"
    coa_ui86atcor_tmpd = prog_dir + "\\" + "ui86atcor.dll"
    minhook86tmpd = prog_dir + "\\" + "MinHook.x86.dll"
    minhook64tmpd = prog_dir + "\\" + "MinHook.x64.dll"
    coamake_tmpd = prog_dir + "\\" + "coamake.exe" #coa_make_edits_on_files
    #output_path = output_file
    #K32/U32/Diğer dller için kopyalama işlemi
    #Daha eklenecek bir sürü dll var.
    shutil.move(output_path, prgtmpd)#Başlangıçta argv ile aldığımız yazılım ismi
    print(prgtmpd)
    shutil.copy("C:\\CompatibilityAPI\\coak32.dll", coak32tmpd)
    shutil.copy("C:\\CompatibilityAPI\\cx86k32.dll", coawow64k32tmpd)
    shutil.copy("C:\\CompatibilityAPI\\coau32.dll", coau32tmpd)
    shutil.copy("C:\\CompatibilityAPI\\cx86u32.dll", coawow64u32tmpd)
    shutil.copy("C:\\CompatibilityAPI\\uiatcor.dll", coa_uiatcor_tmpd)
    shutil.copy("C:\\CompatibilityAPI\\ui86atcor.dll", coa_ui86atcor_tmpd)
    shutil.copy("C:\\CompatibilityAPI\\MinHook.x86.dll", minhook86tmpd)
    shutil.copy("C:\\CompatibilityAPI\\MinHook.x64.dll", minhook64tmpd) # MinHook Projesine teşekkürler.
    shutil.copy("C:\\CompatibilityAPI\\coamake.exe", coamake_tmpd)

def start(input_file, output_file_unique=None, a=0):
    i2 = a + 1
    #output_file = "coa_" + os.path.basename(input_file)
    output_file = "coa_" + os.path.basename(input_file)
    #cfg_outputfile = "coa_" + os.path.basename(input_file)
    cfg_outputfile = "coa_" + os.path.basename(input_file)
    
    if output_file_unique:  # unique varsa onu kullan
        cfg_outputfile = output_file_unique #
    
    if not os.path.isabs(input_file):  # Eğer tam bir yol değilse, yani sadece dosya adıysa
        prog_dir = r"C:\\compatibilityapi\\tmprun"
    else: 
        prog_dir = os.path.dirname(input_file) # Eğer tam bir yol verilmişse, programın bulunduğu dizini al

    pe = pefile.PE(input_file)
    machine = pe.FILE_HEADER.Machine

    if machine == 0x14c: #32bit
        k32forarchstr = "cx86k32.dll"
        u32forarchstr = "cx86u32.dll"
        uiatcor_forarchstr = "uiatcor.dll"
        api_ms_win_crt_stdio_l1_1_0_fwd = "api-ms-win-crt-stdio-l1-1-0.dll"
        api_ms_win_crt_environment_l1_1_0_fwd = "api-ms-win-crt-environment-l1-1-0.dll"
        api_ms_win_crt_runtime_l1_1_0_fwd = "api-ms-win-crt-runtime-l1-1-0.dll"
        api_ms_win_crt_convert_l1_1_0_fwd = "api-ms-win-crt-convert-l1-1-0.dll"
        api_ms_win_crt_string_l1_1_0_fwd = "api-ms-win-crt-string-l1-1-0.dll"
        api_ms_win_crt_utility_l1_1_0_fwd = "api-ms-win-crt-utility-l1-1-0.dll"
        api_ms_win_crt_math_l1_1_0_fwd = "api-ms-win-crt-math-l1-1-0.dll"
        api_ms_win_crt_conio_l1_1_0_fwd = "api-ms-win-crt-conio-l1-1-0.dll"
        api_ms_win_crt_locale_l1_1_0_fwd = "api-ms-win-crt-locale-l1-1-0.dll"
        api_ms_win_crt_heap_l1_1_0_fwd = "api-ms-win-crt-heap-l1-1-0.dll"
        api_ms_win_crt_time_l1_1_0_fwd = "api-ms-win-crt-time-l1-1-0.dll"
        api_ms_win_crt_filesystem_l1_1_0_fwd = "api-ms-win-crt-filesystem-l1-1-0.dll"
        api_ms_win_core_processthreads_l1_1_0_fwd = "api-ms-win-core-processthreads-l1-1-0.dll"
        api_ms_win_core_file_l1_1_0_fwd = "api-ms-win-core-file-l1-1-0.dll"
        api_ms_win_core_memory_l1_1_0_fwd = "api-ms-win-core-memory-l1-1-0.dll"
        api_ms_win_core_handle_l1_1_0_fwd = "api-ms-win-core-handle-l1-1-0.dll"
        api_ms_win_core_psapi_l1_1_0_fwd = "api-ms-win-core-psapi-l1-1-0.dll"
        api_ms_win_core_synch_l1_1_0_fwd = "api-ms-win-core-synch-l1-1-0.dll"
        api_ms_win_core_errorhandling_l1_1_1_fwd = "api-ms-win-core-errorhandling-l1-1-1.dll"
        api_ms_win_core_libraryloader_l1_2_0_fwd = "api-ms-win-core-libraryloader-l1-2-0.dll"
        api_ms_win_core_util_l1_1_0_fwd = "api-ms-win-core-util-l1-1-0.dll"
        api_ms_win_core_processthreads_l1_1_1_fwd = "api-ms-win-core-processthreads-l1-1-1.dll"
        api_ms_win_core_localization_l1_2_0_fwd = "api-ms-win-core-localization-l1-2-0.dll"
        api_ms_win_core_processenvironment_l1_1_0_fwd = "api-ms-win-core-processenvironment-l1-1-0.dll"
        api_ms_win_core_errorhandling_l1_1_0_fwd = "api-ms-win-core-errorhandling-l1-1-0.dll"
        api_ms_win_core_sysinfo_l1_1_0_fwd = "api-ms-win-core-sysinfo-l1-1-0.dll"
        api_ms_win_core_debug_l1_1_0_fwd = "api-ms-win-core-debug-l1-1-0.dll"
        api_ms_win_core_libraryloader_l1_2_1_fwd = "api-ms-win-core-libraryloader-l1-2-1.dll"
        api_ms_win_core_heap_l2_1_0_fwd = "api-ms-win-core-heap-l2-1-0.dll"
        api_ms_win_core_profile_l1_1_0_fwd = "api-ms-win-core-profile-l1-1-0.dll"
        api_ms_win_core_interlocked_l1_1_0_fwd = "api-ms-win-core-interlocked-l1-1-0.dll"
        api_ms_win_core_realtime_l1_1_0_fwd = "api-ms-win-core-realtime-l1-1-0.dll"
        api_ms_win_core_registry_l1_1_0_fwd = "api-ms-win-core-registry-l1-1-0.dll"
        api_ms_win_core_rtlsupport_l1_1_0_fwd = "api-ms-win-core-rtlsupport-l1-1-0.dll"
        api_ms_win_core_sysinfo_l1_2_0_fwd = "api-ms-win-core-sysinfo-l1-2-0.dll"
        api_ms_win_core_string_l1_1_0_fwd = "api-ms-win-core-string-l1-1-0.dll"
        api_ms_win_core_realtime_l1_1_1_fwd = "api-ms-win-core-realtime-l1-1-1.dll"
        api_ms_win_shcore_scaling_l1_1_1_fwd = "api-ms-win-shcore-scaling-l1-1-1.dll"
        api_ms_win_core_version_l1_1_1_fwd = "api-ms-win-core-version-l1-1-1.dll"
        api_ms_win_core_version_l1_1_0_fwd = "api-ms-win-core-version-l1-1-0.dll"

    elif machine == 0x8664: #64bit. Aynı DLL'i 32 Bit yazılımlarda çağırmak sakıncalı olabildiğinden şu anlık API-MS yönlendirmeleri 32 bit için kapatıldı.
        k32forarchstr = "coak32.dll"  #pe headere yazılan değerin mimariye göre değişim fonksiyonu. DLL ADI GERÇEK OLANDAN DAHA UZUN OLURSA YAZILIM KULLANILAMAZ HALE GELEBILIR!
        u32forarchstr = "coau32.dll"
        uiatcor_forarchstr = "ui86atcor.dll"
        api_ms_win_crt_stdio_l1_1_0_fwd = "ucrtbase.dll"#API-MS-WIN kitaplık yonlendirme baslangici
        api_ms_win_crt_environment_l1_1_0_fwd = "ucrtbase.dll"
        api_ms_win_crt_runtime_l1_1_0_fwd = "ucrtbase.dll"
        api_ms_win_crt_convert_l1_1_0_fwd = "ucrtbase.dll"
        api_ms_win_crt_string_l1_1_0_fwd = "ucrtbase.dll"
        api_ms_win_crt_utility_l1_1_0_fwd = "ucrtbase.dll"
        api_ms_win_crt_math_l1_1_0_fwd = "ucrtbase.dll"
        api_ms_win_crt_conio_l1_1_0_fwd = "ucrtbase.dll"
        api_ms_win_crt_locale_l1_1_0_fwd = "ucrtbase.dll"
        api_ms_win_crt_heap_l1_1_0_fwd = "ucrtbase.dll"
        api_ms_win_crt_time_l1_1_0_fwd = "ucrtbase.dll"
        api_ms_win_crt_filesystem_l1_1_0_fwd = "ucrtbase.dll"
        api_ms_win_core_processthreads_l1_1_0_fwd = "kernel32.dll"
        api_ms_win_core_file_l1_1_0_fwd = "kernel32.dll"
        api_ms_win_core_memory_l1_1_0_fwd = "kernel32.dll"
        api_ms_win_core_handle_l1_1_0_fwd = "kernel32.dll"
        api_ms_win_core_psapi_l1_1_0_fwd = "kernel32.dll"
        api_ms_win_core_synch_l1_1_0_fwd = "kernel32.dll"
        api_ms_win_core_errorhandling_l1_1_1_fwd = "kernel32.dll"
        api_ms_win_core_libraryloader_l1_2_0_fwd = "kernel32.dll"
        api_ms_win_core_util_l1_1_0_fwd = "kernel32.dll"
        api_ms_win_core_processthreads_l1_1_1_fwd = "kernel32.dll"
        api_ms_win_core_localization_l1_2_0_fwd = "kernel32.dll"
        api_ms_win_core_processenvironment_l1_1_0_fwd = "kernel32.dll"
        api_ms_win_core_errorhandling_l1_1_0_fwd = "kernel32.dll"
        api_ms_win_core_sysinfo_l1_1_0_fwd = "kernel32.dll"
        api_ms_win_core_debug_l1_1_0_fwd = "kernel32.dll"
        api_ms_win_core_libraryloader_l1_2_1_fwd = "kernel32.dll"
        api_ms_win_core_heap_l2_1_0_fwd = "kernel32.dll"
        api_ms_win_core_profile_l1_1_0_fwd = "kernel32.dll"
        api_ms_win_core_interlocked_l1_1_0_fwd = "kernel32.dll"
        api_ms_win_core_realtime_l1_1_0_fwd = "kernel32.dll"
        api_ms_win_core_registry_l1_1_0_fwd = "kernel32.dll"
        api_ms_win_core_rtlsupport_l1_1_0_fwd = "kernel32.dll"
        api_ms_win_core_sysinfo_l1_2_0_fwd = "kernel32.dll"
        api_ms_win_core_string_l1_1_0_fwd = "kernel32.dll"
        api_ms_win_core_realtime_l1_1_1_fwd = "coak32.dll"
        api_ms_win_security_base_l1_1_0_fwd = "advapi32.dll"
        api_ms_win_core_version_l1_1_1_fwd = "version.dll"
        api_ms_win_core_version_l1_1_0_fwd = "version.dll"
        api_ms_win_shcore_scaling_l1_1_1_fwd = "shcore.dll"#API-MS-WIN kitaplık yonlendirme sonu
    else:
        k32forarchstr = "kernel32.dll"
        u32forarchstr = "user32.dll"
        uiatcor_forarchstr = "UIAutomationCore.dll"
        api_ms_win_crt_stdio_l1_1_0_fwd = "api-ms-win-crt-stdio-l1-1-0.dll"
        api_ms_win_crt_environment_l1_1_0_fwd = "api-ms-win-crt-environment-l1-1-0.dll"
        api_ms_win_crt_runtime_l1_1_0_fwd = "api-ms-win-crt-runtime-l1-1-0.dll"
        api_ms_win_crt_convert_l1_1_0_fwd = "api-ms-win-crt-convert-l1-1-0.dll"
        api_ms_win_crt_string_l1_1_0_fwd = "api-ms-win-crt-string-l1-1-0.dll"
        api_ms_win_crt_utility_l1_1_0_fwd = "api-ms-win-crt-utility-l1-1-0.dll"
        api_ms_win_crt_math_l1_1_0_fwd = "api-ms-win-crt-math-l1-1-0.dll"
        api_ms_win_crt_conio_l1_1_0_fwd = "api-ms-win-crt-conio-l1-1-0.dll"
        api_ms_win_crt_locale_l1_1_0_fwd = "api-ms-win-crt-locale-l1-1-0.dll"
        api_ms_win_crt_heap_l1_1_0_fwd = "api-ms-win-crt-heap-l1-1-0.dll"
        api_ms_win_crt_time_l1_1_0_fwd = "api-ms-win-crt-time-l1-1-0.dll"
        api_ms_win_crt_filesystem_l1_1_0_fwd = "api-ms-win-crt-filesystem-l1-1-0.dll"
        api_ms_win_core_processthreads_l1_1_0_fwd = "api-ms-win-core-processthreads-l1-1-0.dll"
        api_ms_win_core_file_l1_1_0_fwd = "api-ms-win-core-file-l1-1-0.dll"
        api_ms_win_core_memory_l1_1_0_fwd = "api-ms-win-core-memory-l1-1-0.dll"
        api_ms_win_core_handle_l1_1_0_fwd = "api-ms-win-core-handle-l1-1-0.dll"
        api_ms_win_core_psapi_l1_1_0_fwd = "api-ms-win-core-psapi-l1-1-0.dll"
        api_ms_win_core_synch_l1_1_0_fwd = "api-ms-win-core-synch-l1-1-0.dll"
        api_ms_win_core_errorhandling_l1_1_1_fwd = "api-ms-win-core-errorhandling-l1-1-1.dll"
        api_ms_win_core_libraryloader_l1_2_0_fwd = "api-ms-win-core-libraryloader-l1-2-0.dll"
        api_ms_win_core_util_l1_1_0_fwd = "api-ms-win-core-util-l1-1-0.dll"
        api_ms_win_core_processthreads_l1_1_1_fwd = "api-ms-win-core-processthreads-l1-1-1.dll"
        api_ms_win_core_localization_l1_2_0_fwd = "api-ms-win-core-localization-l1-2-0.dll"
        api_ms_win_core_processenvironment_l1_1_0_fwd = "api-ms-win-core-processenvironment-l1-1-0.dll"
        api_ms_win_core_errorhandling_l1_1_0_fwd = "api-ms-win-core-errorhandling-l1-1-0.dll"
        api_ms_win_core_sysinfo_l1_1_0_fwd = "api-ms-win-core-sysinfo-l1-1-0.dll"
        api_ms_win_core_debug_l1_1_0_fwd = "api-ms-win-core-debug-l1-1-0.dll"
        api_ms_win_core_libraryloader_l1_2_1_fwd = "api-ms-win-core-libraryloader-l1-2-1.dll"
        api_ms_win_core_heap_l2_1_0_fwd = "api-ms-win-core-heap-l2-1-0.dll"
        api_ms_win_core_profile_l1_1_0_fwd = "api-ms-win-core-profile-l1-1-0.dll"
        api_ms_win_core_interlocked_l1_1_0_fwd = "api-ms-win-core-interlocked-l1-1-0.dll"
        api_ms_win_core_realtime_l1_1_0_fwd = "api-ms-win-core-realtime-l1-1-0.dll"
        api_ms_win_core_registry_l1_1_0_fwd = "api-ms-win-core-registry-l1-1-0.dll"
        api_ms_win_core_rtlsupport_l1_1_0_fwd = "api-ms-win-core-rtlsupport-l1-1-0.dll"
        api_ms_win_core_sysinfo_l1_2_0_fwd = "api-ms-win-core-sysinfo-l1-2-0.dll"
        api_ms_win_core_string_l1_1_0_fwd = "api-ms-win-core-string-l1-1-0.dll"
        api_ms_win_core_realtime_l1_1_1_fwd = "api-ms-win-core-realtime-l1-1-1.dll"
        api_ms_win_shcore_scaling_l1_1_1_fwd = "api-ms-win-shcore-scaling-l1-1-1.dll"
        api_ms_win_core_version_l1_1_0_fwd = "api-ms-win-core-version-l1-1-0.dll"
        api_ms_win_core_version_l1_1_1_fwd = "api-ms-win-core-version-l1-1-1.dll"

    dll_rename_map = {
        "kernel32.dll": k32forarchstr,
        "user32.dll": u32forarchstr,
        "UIAutomationCore.dll": uiatcor_forarchstr,
        "api-ms-win-crt-stdio-l1-1-0.dll": api_ms_win_crt_stdio_l1_1_0_fwd,
        "api-ms-win-crt-environment-l1-1-0.dll": api_ms_win_crt_environment_l1_1_0_fwd,
        "api-ms-win-crt-runtime-l1-1-0.dll": api_ms_win_crt_runtime_l1_1_0_fwd,
        "api-ms-win-crt-convert-l1-1-0.dll": api_ms_win_crt_convert_l1_1_0_fwd,
        "api-ms-win-crt-string-l1-1-0.dll": api_ms_win_crt_string_l1_1_0_fwd,
        "api-ms-win-crt-utility-l1-1-0.dll": api_ms_win_crt_utility_l1_1_0_fwd,
        "api-ms-win-crt-math-l1-1-0.dll": api_ms_win_crt_math_l1_1_0_fwd,
        "api-ms-win-crt-conio-l1-1-0.dll": api_ms_win_crt_conio_l1_1_0_fwd,
        "api-ms-win-crt-locale-l1-1-0.dll": api_ms_win_crt_locale_l1_1_0_fwd,
        "api-ms-win-crt-heap-l1-1-0.dll": api_ms_win_crt_heap_l1_1_0_fwd,
        "api-ms-win-crt-time-l1-1-0.dll": api_ms_win_crt_time_l1_1_0_fwd,
        "api-ms-win-crt-filesystem-l1-1-0.dll": api_ms_win_crt_filesystem_l1_1_0_fwd,
        "api-ms-win-core-processthreads-l1-1-0.dll": api_ms_win_core_processthreads_l1_1_0_fwd,
        "api-ms-win-core-file-l1-1-0.dll": api_ms_win_core_file_l1_1_0_fwd,
        "api-ms-win-core-memory-l1-1-0.dll": api_ms_win_core_memory_l1_1_0_fwd,
        "api-ms-win-core-handle-l1-1-0.dll": api_ms_win_core_handle_l1_1_0_fwd,
        "api-ms-win-core-psapi-l1-1-0.dll": api_ms_win_core_psapi_l1_1_0_fwd,
        "api-ms-win-core-synch-l1-1-0.dll": api_ms_win_core_synch_l1_1_0_fwd,
        "api-ms-win-core-errorhandling-l1-1-1.dll": api_ms_win_core_errorhandling_l1_1_1_fwd,
        "api-ms-win-core-libraryloader-l1-2-0.dll": api_ms_win_core_libraryloader_l1_2_0_fwd,
        "api-ms-win-core-util-l1-1-0.dll": api_ms_win_core_util_l1_1_0_fwd,
        "api-ms-win-core-processthreads-l1-1-1.dll": api_ms_win_core_processthreads_l1_1_1_fwd,
        "api-ms-win-core-localization-l1-2-0.dll": api_ms_win_core_localization_l1_2_0_fwd,
        "api-ms-win-core-processenvironment-l1-1-0.dll": api_ms_win_core_processenvironment_l1_1_0_fwd,
        "api-ms-win-core-errorhandling-l1-1-0.dll": api_ms_win_core_errorhandling_l1_1_0_fwd,
        "api-ms-win-core-sysinfo-l1-1-0.dll": api_ms_win_core_sysinfo_l1_1_0_fwd,
        "api-ms-win-core-debug-l1-1-0.dll": api_ms_win_core_debug_l1_1_0_fwd,
        "api-ms-win-core-libraryloader-l1-2-1.dll": api_ms_win_core_libraryloader_l1_2_1_fwd,
        "api-ms-win-core-heap-l2-1-0.dll": api_ms_win_core_heap_l2_1_0_fwd,
        "api-ms-win-core-profile-l1-1-0.dll": api_ms_win_core_profile_l1_1_0_fwd,
        "api-ms-win-core-interlocked-l1-1-0.dll": api_ms_win_core_interlocked_l1_1_0_fwd,
        "api-ms-win-core-realtime-l1-1-0.dll": api_ms_win_core_realtime_l1_1_0_fwd,
        "api-ms-win-core-registry-l1-1-0.dll": api_ms_win_core_registry_l1_1_0_fwd,
        "api-ms-win-core-rtlsupport-l1-1-0.dll": api_ms_win_core_rtlsupport_l1_1_0_fwd,
        "api-ms-win-core-sysinfo-l1-2-0.dll": api_ms_win_core_sysinfo_l1_2_0_fwd,
        "api-ms-win-core-string-l1-1-0.dll": api_ms_win_core_string_l1_1_0_fwd,
        "api-ms-win-core-realtime-l1-1-1.dll": api_ms_win_core_realtime_l1_1_1_fwd,
        "api-ms-win-shcore-scaling-l1-1-1.dll": api_ms_win_shcore_scaling_l1_1_1_fwd,
        "api-ms-win-core-version-l1-1-1.dll": api_ms_win_core_version_l1_1_1_fwd,
        "api-ms-win-core-version-l1-1-0.dll": api_ms_win_core_version_l1_1_0_fwd
    }

    if 'cfgiven' in globals() and cfgiven == 1:
        output_file = cfg_outputfile # eğer cfg verilmişse birbirine eşitle

    prgtmpd = prog_dir + "\\" +  output_file

   # neden aynı döngüden iki adet yaptın diye sormayın. üşendim.

    patch_dll_name(input_file, output_file, dll_rename_map, prog_dir, i2)
        
    if k32forarchstr == "kernel32.dll":
        print("Yazılım mimarisi okunamadı.")
        print("Dizin:",prog_dir)
        print("******************************")

    else:
        print("Dizin:",prog_dir)
        print("******************************")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("CompatibilityAPI coaload")
        print("Kullanım: coaload.exe <dosya/yolu/ornek.exe> | coaload.exe -cfg <dosya/yolu/ornek.txt>")
        print(arc, "Bit", "Derleme")
        sys.exit(1) #dosya verilmemişse
    
    if len(sys.argv) > 2 and sys.argv[1] == "-cfg": #parametre kontrolü
        config_file = sys.argv[2]
        #input_file değişkeni configden okunacak
        print("Config dosyası:", config_file)
        cfgiven = 1
    else:
        print("Config dosyası kullanılmadan devam ediliyor...")
        input_file = sys.argv[1]
        cfgiven = 0 

if (cfgiven == 1):
    with open(sys.argv[2], "r", encoding="utf-8") as cfgfile:
        cfglines = cfgfile.readlines()

    cfgkacsatir = len(cfglines)

    for i in range(cfgkacsatir):
        print_i = i + 1
        print(print_i, ".dosya işleniyor (COACFG)")
        cfginput_for = cfglines[i].strip()  # strip() kullanarak satır sonlarındaki boşlukları temizliyoruz
        print("icerik:", cfginput_for)

        #unique_output_name = f"coacfg_{i}_" + os.path.basename(cfginput_for)
        u_output_name = f"coa_" + os.path.basename(cfginput_for)
        start(cfginput_for, u_output_name, i)

else:
    input_file = sys.argv[1]
    start(input_file)
