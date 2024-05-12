#include <iostream>
#include <memory>
#include <ostream>
using namespace std;

// 1. 针对右值引用实施std::move，针对万能引用实施std::forward
class Data {};

class Widget {
    std::string name = "SantaClausSantaClausSantaClausSantaClausSantaClausSantaClausSantaClausSantaClaus";
    std::shared_ptr<Data> ptr;

  public:
    Widget() {cout << "Widget() used for object@" << this
             << " addr_name: " << &(this->name) <<" string buffer addr: "<< static_cast<const void*>(this->name.data())<< endl;};

    //复制构造函数
    Widget(const Widget &w) : name(w.name), ptr(w.ptr) {
        cout << "Widget(const Widget& w) used for object@" << this
             << " addr_name: " << &(this->name) <<" string buffer addr: "<< static_cast<const void*>(this->name.data())<< endl;
    }
    //针对右值引用使用std::move
    Widget(Widget &&rhs) noexcept
        : name(std::move(rhs.name)), ptr(std::move(rhs.ptr)) {
        cout << "Widget(Widget&& rhs) used for object@" << this
             << " addr_name: " << &(this->name) <<" string buffer addr: "<<static_cast<const void*>(this->name.data())<< endl;
    }

    //针对万能引用使用std::forward。
    //注意，这里使用万能引用来替代两个重载版本：void setName(const
    // string&)和void setName(string&&)
    //好处就是当使用字符串字面量时，万能引用版本的效率更高。如w.setName("SantaClaus")，此时字符串会被
    //推导为const
    // char(&)[11]类型，然后直接转给setName函数（可以避免先通过字量面构造临时string对象）。
    //并将该类型直接转给name的构造函数，节省了一个构造和释放临时对象的开销，效率更高。
    template <typename T> void setName(T &&newName) {
        if (newName != name) { //第1次使用newName
            name = std::forward<T>(
                newName); //针对万能引用的最后一次使用实施forward
        }
    }
    ostream &print_addr_of_name(ostream &os) {
        cout << "addr of name: " << static_cast<const void*>(this->name.data()) << endl;
        return os;
    }
};

// 2. 按值返回函数
// 2.1 按值返回的是一个绑定到右值引用的对象
class Complex {
    double x;
    double y;

  public:
    Complex(double x = 0, double y = 0) : x(x), y(y) {}
    Complex &operator+=(const Complex &rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
};

Complex operator+(Complex &&lhs, const Complex &rhs) //重载全局operator+
{
    lhs += rhs;
    return std::move(lhs); //由于lhs绑定到一个右值引用，这里可以移动到返回值上。
}

// 2.2 按值返回一个绑定到万能引用的对象
template <typename T> auto test(T &&t) {
    return std::forward<T>(
        t); //由于t是一个万能引用对象。按值返回时实施std::forward
            //如果原对象一是个右值，则被移动到返回值上。如果原对象
            //是个左值，则会被拷贝到返回值上。
}

// 3. RVO优化
// 3.1 返回局部对象
Widget makeWidget() {
    Widget w;
    return w; //返回局部对象，满足RVO优化两个条件。为避免复制，会直接在返回值内存上创建w对象。
              //但如果改成return
              // std::move(w)时，由于返回值类型不同（Widget右值引用，另一个是Widget）
              //会剥夺RVO优化的机会，就会先创建w局部对象，再移动给返回值，无形中增加一个移动操作。
              //对于这种满足RVO条件的，当某些情况下无法避免复制的（如多路返回），编译器仍会默认地对
              //将w转为右值，即return std::move(w)，而无须用户显式std::move!!!
}

// 3.2 按值形参作为返回值
Widget makeWidget(Widget w) //注意，形参w是按值传参的。
{
    return w; //这里虽然不满足RVO条件（w是形参，不是函数内的局部对象），但仍然会被编译器优化。
              //这里会默认地转换为右值，即return std::move(w)
}

int main() {
    cout << "1. 针对右值引用实施std::move,针对万能引用实施std::forward" << endl;
    Widget w;
    w.setName("SantaClausSantaClausSantaClausSantaClaus");
    cout << "w_addr:" << &w << endl;
    w.print_addr_of_name(cout);

    cout << "2. 按值返回时" << endl;
    auto t1 = test(w);
    auto t2 = test(std::move(w));
    cout << "t1_addr:" << &t1 << endl;
    t1.print_addr_of_name(cout);

    cout << "t2_addr:" << &t2 << endl;
    t2.print_addr_of_name(cout);

    cout << "3. RVO优化" << endl;

    Widget w1 = makeWidget(); //按值返回 局部对象（RVO）
    cout << "w1_addr:" << &w1 << endl;
    w1.print_addr_of_name(cout);

    cout << "w2:\n";
    Widget w2 = makeWidget(w1); //按值返回 按值形参对象

    cout << "w2_addr:" << &w2 << endl;
    w2.print_addr_of_name(cout);

    return 0;
}