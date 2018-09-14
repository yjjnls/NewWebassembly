#include <emscripten/bind.h>
#include "example.h"

using namespace emscripten;

std::string MyClass::getStringFromInstance(const MyClass &instance)
{
    return instance.y_;
}


MyClass *createMyClass(int num)
{
    return new MyClass(num);
}

// Binding code
EMSCRIPTEN_BINDINGS(my_class_example)
{
    class_<MyClass>("MyClass")
        .constructor<>()
        .constructor<int, const std::string &>()
        .constructor(&createMyClass, allow_raw_pointers())

        .function("incrementX", select_overload<void()>(&MyClass::incrementX))
        .function("incrementX", select_overload<void(int)>(&MyClass::incrementX))
        .function("oper", select_overload<MyClass(const MyClass &)>(&MyClass::operator()))

        .property("x", &MyClass::getX, &MyClass::setX)

        .class_function("getStringFromInstance", &MyClass::getStringFromInstance);
}