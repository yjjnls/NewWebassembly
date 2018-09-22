#include "example.h"


std::string MyClass::getStringFromInstance(const MyClass &instance)
{
    return instance.y_;
}

