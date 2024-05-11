#include <iostream>
#include <type_traits>

int main() {
    std::cout << std::boolalpha; // 使得布尔值输出为 true/false 而不是 1/0

    // 检查是否为整数类型
    std::cout << "is_integral<int>: " << std::is_integral<int>::value << '\n';

    // 检查是否为浮点数类型
    std::cout << "is_floating_point<float>: "
              << std::is_floating_point<float>::value << '\n';

    // 移除引用
    using no_ref = std::remove_reference<int &>::type;
    std::cout << "is_same<int, no_ref>: " << std::is_same<int, no_ref>::value
              << '\n';

    // 添加 const 修饰符
    using add_const_to_int = std::add_const<int>::type;
    std::cout << "is_const<add_const_to_int>: "
              << std::is_const<add_const_to_int>::value << '\n';
}
