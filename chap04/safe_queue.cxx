#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <iostream>
#include <thread>
#include <vector>
template <typename T> class threadsafe_queue {
  private:
    mutable std::mutex mut;
    std::queue<T> data_queue;
    std::condition_variable data_cond;

  public:
    threadsafe_queue() {}
    threadsafe_queue(threadsafe_queue const &other) {
        std::lock_guard<std::mutex> lk(other.mut);
        data_queue = other.data_queue;
    }
    void push(T new_value) {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(new_value);
        data_cond.notify_one();
    }
    void wait_and_pop(T &value) {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty(); });
        value = data_queue.front();
        data_queue.pop();
    }
    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty(); });
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }
    bool try_pop(T &value) {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
            return false;
        value = data_queue.front();
        data_queue.pop();
        return true;
    }
    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }
    bool empty() const {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
};



void producer(threadsafe_queue<int> &queue, int start_value) {
    for (int i = 0; i < 5; ++i) {
        queue.push(start_value + i);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100)); // 模拟耗时操作
    }
}

std::mutex print_mutex;
void consumer(threadsafe_queue<int> &queue) {
    for (int i = 0; i < 5; ++i) {
        int value;
        queue.wait_and_pop(value);

        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << "Consumer " << std::this_thread::get_id()
                  << " popped: " << value << std::endl;
    }
}

int main() {
    threadsafe_queue<int> queue;

    std::thread producers[3];
    std::thread consumers[3];

    // 启动生产者线程
    for (int i = 0; i < 3; ++i) {
        producers[i] = std::thread(producer, std::ref(queue),
                                   i * 10); // 每个生产者推送不同范围的数字
    }

    // 启动消费者线程
    for (int i = 0; i < 3; ++i) {
        consumers[i] = std::thread(consumer, std::ref(queue));
    }

    // 等待所有生产者线程完成
    for (int i = 0; i < 3; ++i) {
        producers[i].join();
    }

    // 等待所有消费者线程完成
    for (int i = 0; i < 3; ++i) {
        consumers[i].join();
    }

    return 0;
}
