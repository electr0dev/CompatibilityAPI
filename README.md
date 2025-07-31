# CompatibilityAPI ✨
Modern uygulamaların Windows 8.1 üzerinde çalışmasını sağlamak adına oluşturulmuş kütüphane.

A library designed to make modern apps work on Windows 8.1.

## Kullanım
İlk kurulum için `coasetup.exe` çalıştırılıp kurulum bitirildiğinde;                                                                                                  

```bash 
user.bat ile yada coaload.exe <uygulama\yolu\ornek.exe>
```                                                                                                                                     
şeklinde kullanılabilir.

Setup çalıştırıldıktan sonra user.bat'ın masaüstü kısayolu bulunmaktadır.

## Usage
To set it up for the first time, run coasetup.exe. Once that’s done, you can use it like this:

```bash 
with user.bat or
coaload.exe <path\tofile\ornek.exe>
```

After running the coasetup.exe, a shortcut to user.bat will be available on the desktop.

## Uygulama uyumlulugu ✔
Chromium uygulamaları için destek üzerine çalışılıyor. Testbuildleri Chromium üzerinde test edip ortaya çıkan sorunlar(varsa) issue açarsanız geliştirmeye büyük oranda katkı sağlamış olursunuz.
Önceki versiyonda olduğu gibi bir liste oluşturmadım, oluştururum büyük ihtimalle. Çalışan programları da issue bölümünden bildirebilirsiniz.
Diğer yazılımlarda büyük sorunlar görmedim, çalışmayan programları varsa C ile başlayan hata kodu ile, yoksa Windows hata kodunu(giriş noktası bulunamadı gibi), bunun gibi olan ekran görüntüsünü issue 
kısmında paylaşırsanız yukarıda da dediğim gibi geliştirmeye büyük katkılar sağlamış olursunuz.

## App compatibility ✔

Support for Chromium-based applications is currently in development. If you test the builds with Chromium and report any issues you find, it would be a huge help for improving app support.

I haven’t created "working apps" list yet, but I’ll probably do it soon. You can also report working programs in the issues section.

I haven’t seen major problems with other software. If you find an app that doesn’t work, please share the error details in the issues section—if there’s a error code that starting with "C", include that; if not, include the Windows error message (like “entry point not found”) or a screenshot of the error. As I mentioned above, this kind of feedback really helps development a lot.


![Ekran görüntüsü 2025-06-27 223546](https://github.com/user-attachments/assets/0f121b92-a8d0-4331-b657-085bbd4229d3)


## C ile başlayan mesaj kutuları ne anlama geliyor ?
Bilerek getirildi, kod içinde testbox(L"ornek") şeklinde kullanılıyor. Temelde hangi apinin sıkıntı çıkardığını öğrenmek için getirdim. Eğer bir API'yi yanlış yazdıysam bu şekilde program çökmeden önceki son mesaj kutusunun kodu ile 
hatayı daha rahat anlayabiliyorum. Eğer bu şekilde bir mesaj kutusu program başlangıcında gelirse yada programın içinde iken karşınıza çıkarsa not alın. Sistem donması, çökme gibi sorunlar yaşandığında teşhis etmek kolaylaşıyor.
Sadece Test sürümlerinde olan bir özellik olarak planlandı. Belki ana sürüme de getirebilirim.

## What is the meaning of error codes that starts with "C"

In the code, it’s used like testbox(L"example"). I added it mainly to figure out which API is causing problems. If I’ve written an API call incorrectly, the code of the last message box before the crash helps me understand the issue more easily.

If you see one of these message boxes at program startup or while using the program, please make a note of it. It makes diagnosing freezes or crashes much easier.

This feature is planned to be available only in test builds.

## Son olarak
@JacquelinPOTIER'e Dll Proxy Template Generator için teşekkür ederim. Forwardları oluşturmak için kullanmıştım.

MinHook projesine API Hook kütüphanesi için teşekkür ederim.

## Finally
Thanks to @JacquelinPOTIER for the Dll Proxy Template Generator. I used it to create the forwards.

Thanks to MinHook project for API Hook library.



