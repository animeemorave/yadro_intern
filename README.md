# YADRO IMPULSE 2025 C++

Тестовое задание для стажировки в рамках YADRO IMPULSE 2025 


## Компиляция программы

Для сборки программы используйте одну из следующих команд (в зависимости от компилятора):

**GCC**:
```bash
g++ -g -O3 -Wall -Wextra -std=c++17 task.cpp ledger.cpp -o task
```
**Clang**:
```bash
clang++ -g -O3 -Wall -Wextra -std=c++17 task.cpp ledger.cpp -o task
```
## Запуск тестов
Тестовые данные расположены в папке ./task_data. Для запуска тестов выполните:
```bash
bash ./run_test_data.sh
```
В случае отсутствия прав выполнить: 
```bash
chmod +x ./run_test_data.sh 
```