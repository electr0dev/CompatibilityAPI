@echo off
REM Hedef klasörü ayarla
set "TARGET=C:\CompatibilityAPI"

REM Kaynak klasör: Bu BAT dosyasının olduğu klasör
set "SOURCE=%~dp0"

REM Hedef klasör yoksa oluştur
if not exist "%TARGET%" (
    mkdir "%TARGET%"
)

REM Kendi adını al
set "NAME=%~nx0"

echo Tasima basliyor...

REM Dosyaları taşı
for %%f in ("%SOURCE%\*") do (
    if /I not "%%~nxf"=="%NAME%" (
        copy "%%f" "%TARGET%"
    )
)

mkdir "C:\CompatibilityAPI\tmprun"

echo İslem tamam!
echo Kisayollar olusturuluyor...
shortcut.bat
pause
