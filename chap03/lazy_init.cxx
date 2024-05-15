#include <iostream>
#include <mutex>
#include <thread>

std::once_flag flag;
int config;

void init_config() {
    config = 42; // 假设这是从文件或数据库加载的配置
    std::cout << "Configuration initialized to " << config << std::endl;
}

void process(int id) {
    std::call_once(flag, init_config);
    std::cout << "Thread " << id << " sees configuration as: " << config
              << std::endl;
}

int main() {
    std::thread threads[5];
    for (int i = 0; i < 5; ++i) {
        threads[i] = std::thread(process, i);
    }
    for (auto &t : threads) {
        t.join();
    }
    return 0;
}