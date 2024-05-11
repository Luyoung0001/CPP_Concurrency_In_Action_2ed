#include <chrono>  //for std::chrono::seconds
#include <ctime>   //for std::time_t
#include <iomanip> //for std::put_time
#include <iostream>
#include <thread>

using namespace std;
using namespace std::chrono;

void thread_func(int x) {
    cout << "thread_func start..." << endl;
    cout << "x = " << x << endl;
    cout << "child thread id: " << std::this_thread::get_id() << endl;

    std::this_thread::yield(); //当前线程放弃执行

    cout << "thread_func end." << endl;
}

void test_sleepUntil() {
    std::cout << "thread id " << std::this_thread::get_id()
              << "'s sleepUntil end." << endl;
}

int main() {
    // 1. 获取当前线程信息
    cout << "hardware_concurrency: " << std::thread::hardware_concurrency()
         << endl; // 8，当前cpu核数
    cout << "main thread id: " << std::this_thread::get_id()
         << endl; //当前线程（主线程）id

    std::thread t(thread_func, 5);
    cout << "child thread id: " << t.get_id() << endl; //子线程id
    cout << "child thread handle: " << t.native_handle() << endl;

    // 2.joinable检查
    cout << endl;
    std::this_thread::sleep_for(
        std::chrono::seconds(3)); //主线程睡眠3秒，等待子线程结束

    if (t.joinable())
        cout << "t is joinable"
             << endl; //该行打印，说明子线程己结束时，仍处于joinable状态！！！
    else
        cout << "t is unjoinable" << endl;

    t.join();

    // sleep_until
    cout << endl;
    std::thread t2(test_sleepUntil);
    t2.join();

    //传入lambda
    cout << endl;
    std::thread t3([]() {
        cout << "t3(thread id: " << std::this_thread::get_id()
             << ") is running..." << endl;
    });
    t3.join();

    return 0;
}
