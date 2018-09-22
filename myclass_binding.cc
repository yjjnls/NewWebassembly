#include "example.h"
#include <emscripten/bind.h>

using namespace emscripten;

namespace binding_utils {

MyClass *createMyClass(int num)
{
    return new MyClass(num);
}
void increase(MyClass &obj, int x, int y)
{
    printf("~~~~~~binding_utils MyClass~~~~~~\n");
    obj.incrementX(x + y);
}
int getnumber(const MyClass &obj)
{
    return obj.getX();
}
int getname(const MyClass &obj)
{
    return obj.getX();
}
}  // namespace binding_utils

// Binding code
EMSCRIPTEN_BINDINGS(binding_utils)
{
    class_<MyClass>("MyClass")
        .constructor<>()
        .constructor<int, const std::string &>()
        .constructor(select_overload<MyClass *(int)>(&binding_utils::createMyClass), allow_raw_pointers())

        .function("incrementX", select_overload<void()>(&MyClass::incrementX))
        .function("incrementX", select_overload<void(int)>(&MyClass::incrementX))
        .function("incrementX", select_overload<void(MyClass &, int, int)>(&binding_utils::increase))

        .function("oper", select_overload<MyClass(const MyClass &)>(&MyClass::operator()))

        .property("x", select_overload<int() const>(&MyClass::getX), select_overload<void(int)>(&MyClass::setX))
        .property("number", select_overload<int(const MyClass &)>(&binding_utils::getnumber))

        .class_function("getname", select_overload<int(const MyClass &)>(&binding_utils::getname))
        .class_function("getStringFromInstance", select_overload<std::string(const MyClass &)>(&MyClass::getStringFromInstance));
}
