#include <iostream>
using namespace std;
class A {
    int data;

  public:
    A(int d = 0) : data(d) {}
    ~A() { cout << "destructor called for object " << this << endl; }
};
A creat() {
    A a;
    cout << "a_addr " << &a << endl;
    return a;
    // return move(a);
}
int main() {
    A aa = creat();
    cout << "a_addr " << &aa << endl;
    return 0;
}