@PATH=%PATH%;c:\Program Files\CodeBlocks\MinGW\bin\
REM gcc -std=c99 -lm -O2 main.c gaussian.c -o ./make_table.exe
gcc -Wall -D__USE_MINGW_ANSI_STDIO -O2 main.c gaussian.c -o ./make_table.exe

