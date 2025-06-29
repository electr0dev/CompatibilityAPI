# CompatibilityAPI ✨
Modern uygulamaların Windows 8.1 üzerinde çalışmasını sağlamak adına oluşturulmuş kütüphane. 

## Kullanım
İlk kurulum için `setup.bat` çalıştırılıp kurulum bitirildiğinde;                                                                                                  

```bash 
user.bat ile yada coaload.exe <uygulama\yolu\ornek.exe>
```                                                                                                                                     
şeklinde kullanılabilir.
Ayrıca setup çalıştırıldıktan sonra user.bat'ın masaüstü kısayolu bulunmaktadır.
## Uygulama uyumlulugu ✔
Chromium uygulamaları için destek üzerine çalışılıyor. Testbuildleri Chromium üzerinde test edip ortaya çıkan sorunlar(varsa) issue açarsanız geliştirmeye büyük oranda katkı sağlamış olursunuz.
Önceki versiyonda olduğu gibi bir liste oluşturmadım, oluştururum büyük ihtimalle. Çalışan programları da issue bölümünden bildirebilirsiniz.
Diğer programlarda büyük sorunlar görmedim, çalışmayan programları varsa C ile başlayan hata kodu ile, yoksa Windows hata kodunu(giriş noktası bulunamadı gibi), hiç yoksa
https://github.com/user-attachments/assets/6ea73948-e7df-4ae9-901f-6fb8cf79489c
bunun gibi olan ekran görüntüsünü issue kısmında paylaşırsanız yukarıda da dediğim gibi geliştirmeye büyük katkılar sağlamış olursunuz.
## C ile başlayan mesaj kutuları ne anlama geliyor ?
Bilerek getirildi, kod içinde testbox(L"ornek") şeklinde kullanılıyor. Hangi apinin sıkıntı çıkardığını öğrenmek için. Eğer bir API'yi yanlış yazdıysam bu şekilde program çökmeden önceki son mesaj kutusunun kodu ile 
hatayı daha rahat anlayabiliyorum. Eğer bu şekilde bir mesaj kutusu program başlangıcında gelirse yada programın içinde iken karşınıza çıkarsa not alın. Sistem donması, çökme gibi sorunlar yaşandığında teşhis etmek kolaylaşıyor.
Sadece Test sürümlerinde olan bir özellik olarak planlandı. Belki ana sürüme de getirebilirim.

