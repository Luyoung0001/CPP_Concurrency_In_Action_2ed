#include <iostream>

class Singleton {
  public:
    // 提供一个获取单例实例的方法
    static Singleton &getInstance() {
        static Singleton instance; // 局部静态变量
        return instance;
    }

    void doSomething() { std::cout << "Doing something" << std::endl; }

  private:
    // 私有构造和析构防止外部创建和删除实例
    Singleton() {}
    ~Singleton() {}
    // 禁止复制和赋值
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;
};

int main() {
    // 获取单例实例并使用
    Singleton &singleton = Singleton::getInstance();
    singleton.doSomething();

    return 0;
}
