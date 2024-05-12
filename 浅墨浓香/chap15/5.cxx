#include <iostream>
#include <vector>

using namespace std;

// 1. 大括号初始化列表
void f(const std::vector<int> &v) {
    cout << "void f(const std::vector<int> & v)" << endl;
}

// 2. 0或NULL用作空指针时
void f(int x) { cout << "void f(int x)" << endl; }

// 3. 仅声明static const的整型成员变量而无定义
class Widget {
  public:
    static const std::size_t MinVals;//仅声明，无定义（因为静态变量需在类外定义！）
};

const std::size_t Widget::MinVals = 10;

// 4. 使用重载函数名或模板函数名
int f(int (*pf)(int)) {
    cout << "int f(int(*pf)(int))" << endl;
    return 0;
}

int processVal(int value) { return 0; }
int processVal(int value, int priority) { return 0; }

// 5.位域
struct IPv4Header {
    std::uint32_t version : 4, IHL : 4, DSCP : 6, ECN : 2, totalLength : 16;
    //...
};

template <typename T>
T workOnVal(T param) //函数模板，代表许许多多的函数。
{
    return param;
}

//用于测试的转发函数
template <typename... Ts>
void fwd(Ts &&...param) //转发函数
{
    f(std::forward<Ts>(param)...); //目标函数
}

int main() {
    cout << "-------------------1. 大括号初始化列表---------------------"
         << endl;
    // 1.1 用同一实参分别调用f和fwd函数
    f({1, 2, 3}); //{1, 2, 3}会被隐式转换为std::vector<int>
    // fwd({ 1, 2, 3 });
    // //编译失败。由于fwd是个函数模板，而模板推导时{}不能自动被推导为std:;initializer_list<T>
    // 1.2 解决方案
    auto il = {1, 2, 3};
    fwd(il);

    cout << "-------------------2. 0或NULL用作空指针-------------------"
         << endl;
    // 2.1 用同一实参分别调用f和fwd函数
    // f(NULL);   //调用void f(int)函数，
    fwd(NULL); // NULL被推导为int，仍调用void f(int)函数
    // 2.2 解决方案：使用nullptr
    f(nullptr); //匹配int f(int(*pf)(int))
    fwd(nullptr);

    cout << "-------3. 仅声明static const的整型成员变量而无定义--------"
         << endl;
    // 3.1 用同一实参分别调用f和fwd函数
    f(Widget::MinVals); //调用void f(int)函数。实参从符号表中取得，编译成功！
    fwd(Widget::
            MinVals); // fwd的形参是引用，而引用的本质是指针，但fwd使用到该实参时需要解引用
                      //这里会因没有为MinVals分配内存而出现编译失败（取决于编译器和链接器）
    // 3.2 解决方案：在类外定义该变量

    cout << "-------------4. 使用重载函数名或模板函数名---------------" << endl;
    // 4.1 用同一实参分别调用f和fwd函数
    f(processVal); // ok，由于f形参为int(*pf)(int)，带有类型信息，会匹配int
                   // processVal(int value)
    // fwd(processVal);
    // //error,fwd的形参不带任何类型信息，不知该匹配哪个processVals重载函数。
    // fwd(workOnVal);
    // //error,workOnVal是个函数模板，代表许许多多的函数。这里不知绑定到哪个函数
    // 4.2 解决方案：手动指定类型信息
    using ProcessFuncType = int (*)(int);
    ProcessFuncType processValPtr = processVal;
    fwd(processValPtr);
    fwd(static_cast<ProcessFuncType>(workOnVal)); //调用int f(int(*pf)(int))

    cout << "----------------------5. 转发位域时---------------------" << endl;
    // 5.1 用同一实参分别调用f和fwd函数
    IPv4Header ip = {};
    f(ip.totalLength); //调用void f(int)
    // fwd(ip.totalLength);
    // //error，fwd形参是引用，由于位域是比特位组成。无法创建比特位的引用！
    //解决方案：创建位域的副本，并传给fwd
    auto length = static_cast<std::uint16_t>(ip.totalLength);
    fwd(length);

    return 0;
}
