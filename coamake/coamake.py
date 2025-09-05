import os

print("CompatibilityAPI coamake\n")

for filename in os.listdir("."):
    if filename.startswith("coa_"):
        newname = filename.replace("coa_", "", 1)  # sadece baştaki 'coa_' silinir

        # aynı isimde dosya varsa sil
        if os.path.exists(newname):
            print(f"Orijinal dosya siliniyor: {newname}")
            os.remove(newname)

        # dosyayı yeniden adlandır
        print(f"Yeni dosya adlandiriliyor: {filename} -> {newname}")
        os.rename(filename, newname)