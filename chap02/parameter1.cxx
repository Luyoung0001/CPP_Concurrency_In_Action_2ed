#include <iostream>
#include <thread>

class BigObject {
    std::string s = "hello";

  public:
    const std::string &getData() const { return s; }
    void upDateData(const std::string &str) { s = str; }
    void showInfo() const {
        std::cout << "addr: " << this << " value: " << s << std::endl;
    }
    ~BigObject(){};
    BigObject(){};
};
void update_data_for_BigOb(std::string newString, BigObject &data);
void printInfo(BigObject &ob);

void oops_again(std::string w) {
    BigObject data;
    printInfo(data);
    std::thread t(update_data_for_BigOb, w, std::ref(data));
    t.join();
}

int main() {
    oops_again("hello_new!"); // 函数调用
    return 0;
}

void update_data_for_BigOb(std::string newString, BigObject &data) {
    // 修改
    data.upDateData(newString);
    printInfo(data);
}
void printInfo(BigObject &ob) { ob.showInfo(); }
