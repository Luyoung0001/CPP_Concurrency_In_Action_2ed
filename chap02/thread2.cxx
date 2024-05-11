#include <algorithm>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

constexpr auto tenMillion = 10000000;

bool conditionsAreSatisfied() { return false; } // return true or false

//问题函数：doWork_oops（没有确保std::thread所有皆为不可联结）
//参数：filter过滤器，选0至maxVal之间的值选择出来并放入vector中
bool doWork_oops(std::function<bool(int)> filter, int maxVal = tenMillion) {
    std::vector<int> goodVals; //保存经过滤器筛选出来的数值（0-maxVal）

    std::thread t([&filter, maxVal,
                   &goodVals] { //注意goodVals是局部变量，按引用传入子线程。
        for (auto i = 0; i <= maxVal; ++i)
            if (filter(i))
                goodVals.push_back(i);
    });

    if (conditionsAreSatisfied()) { //如果一切就绪，就开始计算任务
        t.join();                   //等待子线程结束
        // performComputation(goodVals);  //主线程执行计算任务
        return true;
    }

    // conditionsAreSatisfied()时false，表示条件不满足。（注意，仍没调用join()或detach())
    return false; //调用线程（一般是主线程）执行到这里，t对象被析构，std::thread的析构函数被调用，
                  //此时由于子线程仍处于可联结状态，将执行std::ternimate终止程序！
                  //为什么std::thread析构函数不隐式执行join或detach，而是终止程序的运行？
                  //如果隐式调用join()会让主线程等待子线程（耗时任务）结束，这会浪费性能。
                  
                  //而如果隐式调用detach会使主线程和子线程分离，子线程由于引用goodVals局部变量，
                  //会出现悬空引用的问题，但这问题又不容易被发现。因此，通过std::ternimate来终止
                  //程序，以便让程序员自己决定和消除这些问题。比如继续调用join()，还是detach(但需
                  //要同时解决悬空引用问题)？
}

//利用RAII技术，确保std::thread的正常析构
class thread_guard // scoped_thread
{
  public:
    enum class DtorAction { join, detach }; //析构行为

    //构造函数只接受右值类型，因为std::thread只能被移动。虽然t为右值引用类型，但由于形参本身
    //左值，因此调用std::move将形参转为右值。
    thread_guard(std::thread &&t, DtorAction a = DtorAction::join)
        : action(a), thr(std::move(t)) {}

    ~thread_guard() {
        if (thr.joinable()) //必须校验，join和detach只能被调用一次
        {
            if (action == DtorAction::join) {
                thr.join();
            } else {
                thr.detach();
            }
        }
    }

    std::thread &get() { return thr; }

    //由于声明了析构函数，编译器将不再提供移动操作函数，因此需手动生成
    thread_guard(thread_guard &&) noexcept = default;
    thread_guard &operator=(thread_guard &&) = default;

    //本类不支持复制
    thread_guard(const thread_guard &) = delete;
    thread_guard &operator=(const thread_guard &) = delete;

  private:
    //注意action和thr的声明顺序，由于thr被创建以后会执行起来，必须
    //保证action己被初始化。因此先声明action，再声明thr。
    DtorAction action;
    std::thread thr;
};

bool doWork_ok(std::function<bool(int)> filter, int maxVal = tenMillion) {
    std::vector<int> goodVals;

    std::thread t([&filter, maxVal,
                   &goodVals] { //注意goodVals是局部变量，按引用传入子线程。
        for (auto i = 0; i <= maxVal; ++i)
            if (filter(i)) {
                cout << i << endl;
                goodVals.push_back(i);
            }
    });

    thread_guard guard(
        std::move(t)); //默认析构策略是thread_guard::DtorAction::join

    if (conditionsAreSatisfied()) { //如果一切就绪，就开始计算任务
        guard.get().join();         //等待子线程结束
        // performComputation(goodVals);  //主线程执行计算任务
        return true;
    }

    // conditionsAreSatisfied()时false，表示条件不满足。guard对象析构，但会隐式调std::thread对象
    //的join()。
    return false;
}

//使用RAII等待线程完成：joining_thread类的实现
class joining_thread {
    std::thread thr;

  public:
    joining_thread() noexcept = default;

    //析构函数
    ~joining_thread() {
        if (joinable()) //对象析构造，会隐式调用join()
        {
            join();
        }
    }

    template <typename Callable, typename... Args>
    explicit joining_thread(Callable &&func, Args &&...args)
        : thr(std::forward<Callable>(func), std::forward<Args>(args)...) {}

    //类型转换构造函数
    explicit joining_thread(std::thread t) noexcept : thr(std::move(t)) {}

    //移动操作
    joining_thread(joining_thread &&other) noexcept
        : thr(std::move(other.thr)) {}

    joining_thread &operator=(joining_thread &&other) noexcept {
        if (joinable())
            join(); //等待原线程执行完

        thr = std::move(other.thr); //将新线程移动到thr中

        return *this;
    }

    joining_thread &operator=(std::thread other) noexcept {
        if (joinable())
            join();

        thr = std::move(other);

        return *this;
    }

    bool joinable() const noexcept { return thr.joinable(); }

    void join() { thr.join(); }
    void detach() { thr.detach(); }

    void swap(joining_thread &other) noexcept { thr.swap(other.thr); }
    std::thread::id get_id() const noexcept { return thr.get_id(); }

    std::thread &asThread() noexcept //转化为std::thread对象
    {
        return thr;
    }
    const std::thread &asThread() const noexcept { return thr; }
};

void doWork(int i) { cout << i << endl; }

int main() {
    // 1.问题函数：doWork_oops：没有确保std::thread的所有路径都为joinable
    // doWork_oops([](auto val) { return val >= 100; }, 1000);

    // 2. doWork_ok函数
    doWork_ok([](auto val) { return val >= 100; }, 1000);

    // 3. 测试joining_thread类
    std::vector<joining_thread> threads; // joining_thread析构时隐式调用join
    for (unsigned int i = 0; i < 20; ++i) {
        threads.push_back(joining_thread(doWork, i));
    }

    std::for_each(threads.begin(), threads.end(),
                  std::mem_fn(&joining_thread::join));

    return 0;
}