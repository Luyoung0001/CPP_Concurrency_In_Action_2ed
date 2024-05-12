#include <iostream>
#include <string>
#include <utility>
#include <vector>

// 返回局部右值引用对象，使用 std::move
std::vector<int> createVector() {
    std::vector<int> localVec = {1, 2, 3, 4, 5};
    return std::move(localVec); // 移动 localVec 到返回值
}

// 接收万能引用，返回相同类型
template <typename T>
T relay(T &&obj) {
    // std::forward 确保 obj 的值类别保持不变
    return std::forward<T>(obj);
}

int main() {
    auto vec = createVector(); // vec 通过移动构造器获取 localVec 的资源
    for (auto v : vec) {
        std::cout << v << " ";
    }
    std::cout << "\n";

    std::string str = "Hello, World!";
    auto result = relay(std::move(str)); // str 被视为右值，使用移动语义

    std::cout << "Result: " << result << " addr:" << &str << "\n";
    std::cout << "Original string: " << str << " addr:" << &str << " (moved)\n";

    std::string anotherStr = "Another test";
    auto anotherResult = relay(anotherStr); // anotherStr 仍为左值，使用复制语义

    std::cout << "AnotherStr " << anotherResult <<" addr:" << &anotherStr << "\n";
    std::cout << "Another result: " << anotherResult <<" addr:" << &anotherResult << "\n";

}
