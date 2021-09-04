@PATH=%PATH%;c:\Program Files\CodeBlocks\MinGW\bin\
gcc -lm -std=c99 -O3 -DTABLE_TYPE=int32_t           ./main.c ./get_sine_int32.c  ./get_sine_float.c ../../tables/sine_approx_64_3_3.c -o ./sine_approx_example.exe
gcc -lm -std=c99 -O3 -DTABLE_TYPE=int32_t -DQ1_ONLY ./main.c ./get_sine_int32.c  ./get_sine_float.c ../../tables/sine_approx_64_3_3.c -o ./sine_approx_example_Q1.exe