
-- запускаем генератор таблиц
-- меняем параметры
-- длину от 2^2 до 2^16
-- степень полинома от 1 до 6
-- сдвиг S берём оптимальный, заранее определённый
-- Итого: 15 * 6 = 90 таблиц

-- Заодно создаём заготовку массива структур с описаниями сгенерённыйх таблиц 
-- для последующей проверки точности вычисления функции разными методами


count = 0

file_C   = io.open("../tables/all_tables.c", "w")
file_H   = io.open("../tables/all_tables.h", "w")
file_CMD = io.open("./make_all_tables.cmd",  "w")
file_SH  = io.open("./make_all_tables.sh",   "w")

io.output(file_C)
io.write("#include \"approx_table_def.h\"\n")
io.write("#include \"all_tables.h\"\n\n")
io.write("const struct t_ApproxTableDef approx_tables[90] =\n{\n")

io.output(file_H)
io.write("#ifndef APPROX_TABLE_DEF_H\n")
io.write("#define APPROX_TABLE_DEF_H\n\n")
io.write("#include <stdint.h>\n\n")
io.write("#ifndef TABLE_TYPE\n\t#define TABLE_TYPE int32_t\n#endif\n\n")
io.write("#define LONG_DOUBLE long double\n\n\n") 

for len_factor = 2,16,1 do
    len = math.floor(2 ^ len_factor)
    
    -- оптимальный shift лежит в пределах от 0 до 5, в зависимости от размера таблицы
    S = len_factor - 3
    if S < 0 then
      S = 0
    elseif S > 5 then
      S = 5
    end
    
    
    for degree = 1, 6 do
      table_name = string.format("sine_approx_table_%i_deg_%i_acs_%i", len, degree, S)
      
      str = string.format("\t{%s,      %i,  %i,  %i},    // %i\n", table_name, len_factor, degree, S, count)
      io.output(file_C)
      io.write(str)
      
      str = string.format("extern TABLE_TYPE %s[%i]; // %i\n", table_name, len * (degree + 1), count)
      io.output(file_H)
      io.write(str)

      count = count + 1
      
      command = string.format
      (
        "./make_table.exe %i %i 0x40000000LL %i > ../tables/sine_approx_%i_%i_%i.c", 
--          "../make_table/make_table_no_out.exe %i %i 0x40000000LL %i >> ../tables/bit_reserve.log", 
        len, degree, S, len, degree, S
      )
      print(count .. "  " .. command)
      
      io.output(file_SH)
      io.write(command)
      io.write("\n")
      
      command = string.format
      (
        ".\\make_table.exe %i %i 0x40000000LL %i > ..\\tables\\sine_approx_%i_%i_%i.c", 
        len, degree, S, len, degree, S
      )
      --os.execute(command)

      io.output(file_CMD)
      io.write(command)
      io.write("\n")
			
    end   
  io.output(file_C)
  io.write("\n")
  io.output(file_H)
  io.write("\n")
    
end
io.output(file_C)
io.write("};\n")
io.close(file_C)

io.output(file_H)
io.write("\n#endif // APPROX_TABLE_DEF_H\n")
io.close(file_H)

io.close(file_CMD)
io.close(file_SH)

 file_H = io.open("../tables/approx_table_def.h", "w")
io.output(file_H)
str = string.format
(
  "#ifndef APPROX_TYPLE_DEF_H\n" ..
  "#define APPROX_TYPLE_DEF_H\n\n" ..
  "#include <stdint.h>\n\n" ..
  "#ifndef TABLE_TYPE\n\t#define TABLE_TYPE int32_t\n#endif\n\n" ..
  "#define LONG_DOUBLE long double\n\n\n" ..      
  "struct t_ApproxTableDef\n" ..
  "{\n" ..
  "\tTABLE_TYPE *p;  // сама таблица\n" ..
  "\tint pN;         // 2^pN строк в таблице\n" ..
  "\tint pw;         // степень полинома (pw+1 коэффициентов в строке)\n" ..
  "\tint S;          // AC shift\n" ..
  "};\n" ..
  "\n" ..
  "extern const struct t_ApproxTableDef approx_tables[%i];\n" ..
  "\n" ..
  "\n#endif // APPROX_TYPLE_DEF_H\n"
  , count
)
io.write(str)      
io.close(file_H)
