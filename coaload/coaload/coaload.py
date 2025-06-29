import pefile
import sys
import os
import struct
import shutil
import subprocess
import time

arc = struct.calcsize("P") * 8

def patch_dll_name(input_path, output_path, dll_map):
    pe = pefile.PE(input_path)
    print("/P1 |", arc, "Bit\n---------------------------")
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

    pe.write(output_path)
    pe.close()

    print(f"Yeni dosya yazıldı: {output_path}")
    prgtmpd = prog_dir + "\\" + output_path
    coak32tmpd = prog_dir + "\\" + "coak32.dll"
    coawow64k32tmpd = prog_dir + "\\" + "cx86k32.dll"
    coau32tmpd = prog_dir + "\\" + "coau32.dll"
    coawow64u32tmpd = prog_dir + "\\" + "cx86u32.dll"

    #Daha eklenecek bir sürü dll var.
    shutil.move(output_path, prgtmpd)#Başlangıçta argv ile aldığımız yazılım ismi
    shutil.copy("C:\\CompatibilityAPI\\coak32.dll", coak32tmpd)
    shutil.copy("C:\\CompatibilityAPI\\cx86k32.dll", coawow64k32tmpd)
    shutil.copy("C:\\CompatibilityAPI\\coau32.dll", coau32tmpd)
    shutil.copy("C:\\CompatibilityAPI\\cx86u32.dll", coawow64u32tmpd)
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("CompatibilityAPI V2 coaload")
        print("Kullanım: coaload.exe <dosya/yolu/ornek.exe>")
        print("Mimari:", arc, "bit")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = "coatmp_" + os.path.basename(input_file)
    
    if not os.path.isabs(input_file):  # Eğer tam bir yol değilse, yani sadece dosya adıysa
    # Varsayılan dizin
      prog_dir = r"C:\\compatibilityapi\\tmprun"
    else:
    # Eğer tam bir yol verilmişse, programın bulunduğu dizini al
      prog_dir = os.path.dirname(input_file)


    pe = pefile.PE(input_file)
    machine = pe.FILE_HEADER.Machine

    if machine == 0x14c: #32bit
        k32forarchstr = "cx86k32.dll"
        u32forarchstr = "cx86u32.dll"
    elif machine == 0x8664: #64bit
        k32forarchstr = "coak32.dll"  #pe headere yazılan değerin mimariye göre değişim fonksiyonu. DLL ADI GERÇEK OLANDAN DAHA UZUN OLURSA YAZILIM KULLANILAMAZ HALE GELEBILIR!
        u32forarchstr = "coau32.dll"
    else:
        k32forarchstr = "kernel32.dll"
        u32forarchstr = "user32.dll"

    

    dll_rename_map = {
        "kernel32.dll": k32forarchstr,
        "user32.dll": u32forarchstr,
    }

    prgtmpd = prog_dir + "\\" + output_file
    patch_dll_name(input_file, output_file, dll_rename_map)
    if k32forarchstr == "kernel32.dll":
        print("Yazılım mimarisi okunamadı.")
        print("Dizin:",prog_dir)
        sys.exit(1)
    else:
        print("Dizin:",prog_dir)
        subprocess.run([prgtmpd])
        sys.exit(0)
