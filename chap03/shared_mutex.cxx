#include <iostream>
#include <shared_mutex>
#include <thread>
#include <vector>

std::shared_mutex rw_mutex;
int data = 0; // 示例共享数据

void reader(int id) {
    std::shared_lock<std::shared_mutex> lock(rw_mutex);
    // 多个读取者可以同时执行下面的代码
    std::cout << "Reader " << id << " sees data = " << data << std::endl;
    // shared_lock 在离开作用域时自动释放
}

void writer(int id, int value) {
    std::unique_lock<std::shared_mutex> lock(rw_mutex);
    // 只有一个写入者可以执行下面的代码
    data = value;
    std::cout << "Writer " << id << " updated data to " << data << std::endl;
    // unique_lock 在离开作用域时自动释放
}

int main() {
    std::thread readers[5];
    std::thread writers[2];

    // 启动读取者线程
    for (int i = 0; i < 5; ++i) {
        readers[i] = std::thread(reader, i);
    }

    // 启动写入者线程
    writers[0] = std::thread(writer, 0, 100);
    writers[1] = std::thread(writer, 1, 200);

    // 等待所有线程完成
    for (int i = 0; i < 5; ++i) {
        readers[i].join();
    }
    for (int i = 0; i < 2; ++i) {
        writers[i].join();
    }

    return 0;
}