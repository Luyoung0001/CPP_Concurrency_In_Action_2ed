#include <iostream>
#include <mutex>
#include <thread>

std::recursive_mutex rec_mtx;
int Num[5] = {0, 1, 2, 3, 4};

void recursive_function_update(int n) {
    if (n <= 0)
        return;

    rec_mtx.lock();
    std::cout << "updating: " << n << std::endl;
    Num[n] = Num[n] * Num[n];
    recursive_function_update(n - 1);
    std::cout << "Unlock level " << n << std::endl;
    rec_mtx.unlock();
}

void recursive_function_read(int n) {
    if (n <= 0)
        return;

    rec_mtx.lock();
    std::cout << "reading: " << n << std::endl;
    std::cout << Num[n] << std::endl;
    recursive_function_read(n - 1);
    std::cout << "Unlock level " << n << std::endl;
    rec_mtx.unlock();
}

int main() {
    std::thread u(recursive_function_update, 3);
    std::thread u1(recursive_function_update, 3);
    std::thread r(recursive_function_read, 3);
    std::thread r1(recursive_function_read, 3);

    u.join();
    u1.join();
    r.join();
    r1.join();
    return 0;
}
