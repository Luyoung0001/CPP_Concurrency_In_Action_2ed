#include <iostream>
void func(const int &b) {
    std::cout << "b: " << b << " addr: " << &b << std::endl;
    // b++; // 会报错
    std::cout << b << std::endl;
}
int main() {
    int c = 10;
    func(std::move(c));
    std::cout << "c: " << c << " addr: " << &c << std::endl;
    return 0;
}