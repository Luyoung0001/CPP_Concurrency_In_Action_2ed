#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx1, mtx2;

void process_data() {
    // 使用std::lock来同时锁定两个互斥量
    std::lock(mtx1, mtx2);

    // 确保两个互斥量都已锁定，使用std::lock_guard进行管理，不指定std::adopt_lock参数
    std::lock_guard<std::mutex> lk1(mtx1, std::adopt_lock);
    std::lock_guard<std::mutex> lk2(mtx2, std::adopt_lock);

    // 执行一些操作
    std::cout << "Processing shared data." << std::endl;
}

int main() {
    std::thread t1(process_data);
    std::thread t2(process_data);

    t1.join();
    t2.join();

    return 0;
}
