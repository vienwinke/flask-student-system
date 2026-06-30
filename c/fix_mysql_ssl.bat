@echo off
copy /y "C:\Users\13992\Desktop\c??????\c\c\my.ini.new" "C:\ProgramData\MySQL\MySQL Server 9.7\my.ini"
if %errorlevel% equ 0 (
    echo [OK] my.ini ???
    net stop MySQL97
    net start MySQL97
    echo [OK] MySQL ???
) else (
    echo [FAIL] ????
)
pause
