
-- запускаем генератор таблиц, собранный без дефайна OUT_EN
-- в это виде он выводит только запас по битам в зависимости
-- от параметров:
-- длину от 2^2 до 2^16
-- степень полинома от 1 до 6
-- сдвиг S, что бы веса соседних чоенов полинома отличались в 2^S раз. S = 0..8
-- Итого: 15 * 6 * 11 = 990 комбинаций
-- Потом выберем самые оптимальные в плане запаса по битам (зависит от S), и проверим их.

count = 0
for len_factor = 2, 16, 1 do
    len = math.floor(2 ^ len_factor)
    
    for degree = 1, 6 do
      for shift = 0,10 do
        command = string.format
        (
--          "../table_make/make_table.exe %i %i 0x40000000LL %i > ./tables/sine_approx_%i_%i_%i.c", 
          "../table_make/make_table_no_out.exe %i %i 0x40000000LL %i >> ./tables/bit_reserve.log", 
          len, degree, shift, len, degree, shift
        )
        count = count + 1
        print(count .. "  " .. command)
        os.execute(command)
      end      
    end    
end

 