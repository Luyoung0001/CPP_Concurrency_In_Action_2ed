#include <iostream>
#include <mutex>
#include <thread>

std::mutex mtx1, mtx2;

void process_shared_data() {
    // 使用std::scoped_lock同时锁定两个互斥量
    std::scoped_lock lock(mtx1, mtx2);

    // 执行一些操作
    std::cout << "Processing shared data safely." << std::endl;
}

int main() {
    std::thread t1(process_shared_data);
    std::thread t2(process_shared_data);

    t1.join();
    t2.join();

    return 0;
}
