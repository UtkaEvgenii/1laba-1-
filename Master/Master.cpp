#include <iostream> 
#include <fstream> //работа с файлами
#include <string> 
#include <vector> 
#include <cstdlib> //system
#include <windows.h> //Windows API
#include <process.h> //для работы с процессами spawnl

using namespace std;

//const long long START = 10000000000LL;  // 10^10
//const long long END = 100000000000LL;   // 10^11

const long long START = 1LL;
const long long END = 100000000LL;
const int NUM_WORKERS = 8;              // Используем 8 worker'ов
const long long TOTAL_NUMBERS = END - START; //
const long long CHUNK_SIZE = TOTAL_NUMBERS / NUM_WORKERS;  // Равные части


void setRussianLocale() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");
}

int main() {
    setRussianLocale();

    cout << "Главная программа запущена" << endl;
    cout << "Поиск простых чисел-близнецов в диапазоне: " << START << " - " << END << endl;
    cout << "Количество worker'ов: " << NUM_WORKERS << endl;
    cout << "Размер блока на worker: " << CHUNK_SIZE << " чисел" << endl;
    cout << "Общее количество чисел: " << TOTAL_NUMBERS << endl;

    // Удаляем старые результаты, 2>nul - пустота
    system("del result_*.txt 2>nul");

    cout << "\nЗапуск 8 рабочих процессов..." << endl;

    // Запускаем ровно 8 рабочих процессов ПАРАЛЛЕЛЬНО (uintptr_t - хранение дескрипторов процессов
    vector<uintptr_t> processes;

    for (int worker_id = 0; worker_id < NUM_WORKERS; worker_id++) {
        // Распределяем диапазон равномерно между 8 worker'ами
        long long worker_start = START + (worker_id * CHUNK_SIZE);
        long long worker_end = START + ((worker_id + 1) * CHUNK_SIZE) - 1;

        // Последний worker добирает до конца диапазона
        if (worker_id == NUM_WORKERS - 1) {
            worker_end = END;
        }

        //преобразование числа в строки для передачи в командной строке
        string start_str = to_string(worker_start);
        string end_str = to_string(worker_end);
        string worker_str = to_string(worker_id);

        // Запускаем процесс параллельно
        //Флаг _P_NOWAIT - не ждать завершение

        uintptr_t process = _spawnl(_P_NOWAIT, "worker.exe", "worker.exe",
            start_str.c_str(), end_str.c_str(), worker_str.c_str(), NULL);

        if (process != -1) {
            processes.push_back(process);
            cout << "Запущен worker " << worker_id << ": " << worker_start << " - " << worker_end << endl;
            cout << "  Размер блока: " << (worker_end - worker_start + 1) << " чисел" << endl;
        }
        else {
            cout << "Ошибка запуска worker " << worker_id << endl;
        }

        Sleep(50); // Задержка для стабильности
    }

    cout << "\nУспешно запущено " << processes.size() << " процессов. Ожидание завершения..." << endl;

    // Ждем завершения всех процессов
    for (uintptr_t process : processes) {
        _cwait(NULL, process, WAIT_CHILD);
    }

    cout << "\nВсе процессы завершены! Сбор результатов..." << endl;

    // Собираем результаты
    cout << "\n=== РЕЗУЛЬТАТЫ ===" << endl;
    system("type result_*.txt 2>nul");

    // Объединяем все результаты
    system("copy result_*.txt final_results.txt 2>nul");

    // Показываем статистику по файлам
    cout << "\n=== СТАТИСТИКА ===" << endl;
    cout << "Создано файлов: result_0.txt до result_7.txt" << endl;
    cout << "Общие результаты сохранены в final_results.txt" << endl;

    // Показываем размеры файлов
    system("dir result_*.txt | find \"result_\"");

    return 0;
}