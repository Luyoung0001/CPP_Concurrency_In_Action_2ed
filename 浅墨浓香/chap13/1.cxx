#include <iostream>
void func(int &&b) {
    std::cout << "b: " << b << " addr: " << &b << std::endl;
    b++;
    std::cout << b << std::endl;
}
int main() {
    int c = 10;
    func(std::move(c));
    std::cout << "c: " << c << " addr: " << &c << std::endl;
    return 0;
}