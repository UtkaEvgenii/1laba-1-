#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <windows.h>

using namespace std;

void setRussianLocale() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");
}

bool is_prime(long long n) {
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    long long limit = static_cast<long long>(sqrt(n)) + 1;
    for (long long i = 5; i <= limit; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    setRussianLocale();

    if (argc != 4) {
        cout << "Использование: worker.exe <начало> <конец> <ID_работника>" << endl;
        return 1;
    }

    long long start = stoll(argv[1]);
    long long end = stoll(argv[2]);
    int worker_id = stoi(argv[3]);

    string filename = "result_" + to_string(worker_id) + ".txt";
    ofstream outfile(filename, ios::app);

    outfile.imbue(locale("Russian"));

    cout << "Работник " << worker_id << " проверяет диапазон: " << start << " - " << end << endl;

    long long count = 0; //счетчик пар близнецов
    vector<pair<long long, long long>> twins;

    for (long long n = start; n <= end; n++) {
        if (is_prime(n) && is_prime(n + 2)) {
            twins.push_back({ n, n + 2 });
            count++;
        }

        if ((n - start) % 10000 == 0 && n > start) {
            cout << "Работник " << worker_id << " прогресс: "
                << (n - start) << "/" << (end - start) << endl;
        }
    }

    outfile << "=== Работник " << worker_id << " диапазон " << start << "-" << end << " ===" << endl;
    outfile << "Найдено пар близнецов: " << count << endl;

    for (const auto& twin : twins) {
        outfile << twin.first << " и " << twin.second << endl;
    }

    outfile << endl;
    outfile.close();

    cout << "Работник " << worker_id << " завершил. Найдено пар: " << count << endl;

    return 0;
}
