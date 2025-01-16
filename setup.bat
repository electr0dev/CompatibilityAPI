@echo off
openfiles >nul 2>&1
if %errorlevel% NEQ 0 (
echo Yonetici olarak calistirip tekrar deneyin.
pause
exit /b
)
set tasimayolu=C:\Windows\SysWOW64\
move "CA_VerAPIv1.dll" "%tasimayolu%"
move "msvcp100d.dll" "%tasimayolu%"
move "msvcr100d.dll" "%tasimayolu%"
echo Islem basarili.
pause