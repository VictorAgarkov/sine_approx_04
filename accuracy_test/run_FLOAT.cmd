@echo off
REM запускаем 3 проверки 3 разных типов; каждая проверка считает ВСЕ варианты
start "32" .\accuracy_test_04_FLOAT32.exe t a
start "64" .\accuracy_test_04_FLOAT64.exe t a
start "80" .\accuracy_test_04_FLOAT80.exe t a

