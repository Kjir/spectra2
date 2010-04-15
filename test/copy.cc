#include <iostream>
#include <boost/circular_buffer.hpp>
#include <boost/ref.hpp>

class B
{
};

class A {
  public:
    A();
    A(const A &other);
    ~A();
    A & operator=(const A &rhs);
    B *pointer;
};

A::A() {
  pointer = new B;
  std::cerr << "Created pointer: " << std::hex << pointer << std::endl;
}

A::A(const A &other) {
    pointer = new B;
    std::cerr << "Created pointer: " << std::hex << pointer << std::endl;
    std::cerr << "Other is: " << std::hex << other.pointer << std::endl;
}

A::~A() {
  std::cerr << "Deleting " << std::hex << pointer << std::endl;
  delete pointer;
}

A & A::operator=(const A & rhs) {
}

int main() {
    boost::circular_buffer<A> cbuf(2);
    for(int i = 0; i < 8; i++) {
        A a1;
        cbuf.push_back(a1);
        std::cerr << "End of loop" << std::endl;
    }
}
