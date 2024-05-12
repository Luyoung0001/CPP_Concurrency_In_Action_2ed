#include <algorithm>
#include <iostream>

class Widget {
  public:
    static const int MinVals = 20; // 静态常量整型成员的声明和初始化
};
// const int Widget:: MinVals = 20;

void f(int val) { std::cout << "Value: " << val << std::endl; }

template <typename T>
void fwd(T &&val) {
    std::cout << "Forwarded value: " << val << std::endl;
}

int main() {
    f(Widget::MinVals);
    // fwd(Widget::MinVals); // 连接错误
    return 0;
}
