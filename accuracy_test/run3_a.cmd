@echo off

REM запускаем процессы проверки одного типа; каждый процесс считает свою 1/3 вариантов

start "3/3" .\accuracy_test_04.exe t 88-60 
timeout 3
start "2/3" .\accuracy_test_04.exe   59-30
timeout 2
start "1/3" .\accuracy_test_04.exe   29-0

