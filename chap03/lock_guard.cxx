#include <iostream>
#include <mutex>
#include <thread>

std::mutex mtx;  // 全局互斥量

void print_data(const std::string& data) {
    std::lock_guard<std::mutex> guard(mtx);  // 创建时自动锁定mtx
    // 以下代码在互斥锁保护下执行
    std::cout << data << std::endl;
    // guard 在离开作用域时自动解锁mtx
}

int main() {
    std::thread t1(print_data, "Hello from Thread 1");
    std::thread t2(print_data, "Hello from Thread 2");

    t1.join();
    t2.join();

    return 0;
}
