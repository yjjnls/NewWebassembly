#include <emscripten/bind.h>
#include "example.h"

using namespace emscripten;

std::string MyClass::getStringFromInstance(const MyClass &instance)
{
    return instance.y;
}

// Binding code
EMSCRIPTEN_BINDINGS(my_class_example)
{
    class_<MyClass>("MyClass")
        .constructor<int>()
        .function("incrementX", &MyClass::incrementX)
        .property("x", &MyClass::getX, &MyClass::setX)
        .class_function("getStringFromInstance", &MyClass::getStringFromInstance);
}