#include <iostream>
#include <memory>
#include <mutex>

class Singleton {
  private:
    static std::unique_ptr<Singleton> instance;
    static std::once_flag onceFlag;

  public:
    Singleton() = default;
    ~Singleton() = default;
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

    static Singleton *getInstance() {
        std::call_once(onceFlag, [] { instance.reset(new Singleton); });
        return instance.get();
    }

    void doSomething() { std::cout << "Doing something" << std::endl; }
};

std::unique_ptr<Singleton> Singleton::instance;
std::once_flag Singleton::onceFlag;

int main() {
    Singleton::getInstance()->doSomething();
    return 0;
}