#include <emscripten/bind.h>
#include "example.h"

using namespace emscripten;

std::string MyClass::getStringFromInstance(const MyClass &instance)
{
    return instance.y_;
}

// Binding code
EMSCRIPTEN_BINDINGS(my_class_example)
{
    class_<MyClass>("MyClass")
        .constructor<>()
        .constructor<int>()
        .constructor<int, const std::string &>()
        .function("incrementX", select_overload<void()>(&MyClass::incrementX))
        .function("incrementX", select_overload<void(int)>(&MyClass::incrementX))
        .property("x", &MyClass::getX, &MyClass::setX)
        .class_function("getStringFromInstance", &MyClass::getStringFromInstance);
}