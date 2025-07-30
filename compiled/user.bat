@echo off
echo CompatibilityAPI V2 rc0.2
echo user.bat
set /p dir=Calistirmak istediginiz programin yolunu belirtin:
:Tabii ki bazen yol olarak değil C:\CompatibilityAPI\ dizininden "ornek.exe" gibi yazılıp çalıştırılabiliyor, hata alınmaması için,
:dizinden yol belirtmeden başlatılıyorsa coaload varsayılan dizini C:\CompatibilityAPI\tmprun olarak belirler. Eğer 
:"C:\ornekdizin\ornek.exe" şeklinde belirtilirse yeni dosya verilen dizine taşınacaktır.  
coaload.exe %dir%
echo Bir tusa basin...
pause >nul