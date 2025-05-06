# YADRO IMPULSE 2025 C++.TATLIN.Unified

Тестовое задание для стажировки, реализующее алгоритм сортировки данных на ленточном устройстве. 
Проект демонстрирует работу с виртуальной лентой (Tape) и алгоритм внешней сортировки слиянием.

## Компиляция
Для сборки основной программы выполните:
```bash
g++ -std=c++17 main.cpp TapeSort.cpp TapeImplementation.cpp -o sort
./sort input_tape.txt output_tape.txt
```
## Запуск unit тестов
Для запуска TapeImplUnitTest.cpp, соберите следующие файлы
```bash
g++ -std=c++17 TapeImplUnitTest.cpp TapeImplementation.cpp doctest_main.cpp -o tape_tests
./tape_tests
``` 
Для запуска TapeSortUnitTest.cpp, соберите следующие файлы
```bash
g++ -std=c++17 TapeSort.cpp TapeSortUnitTest.cpp doctest_main.cpp TapeImplementation.cpp -o sort_tests
./sort_tests
``` 
