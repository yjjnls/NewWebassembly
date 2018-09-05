
#include <emscripten/bind.h>
#include <new>
#include <node_api.h>
#include "common.h"
#include "../../example.h"

using namespace emscripten::internal;

extern "C" {
void _embind_register_class(
    TYPEID classType,         // rtti info of this class(MyClass)
    TYPEID pointerType,       // rtti info of MyClass
    TYPEID constPointerType,  // rtti info of MyClass
    TYPEID baseClassType,     // rtti info of base class(nullptr)
    const char *getActualTypeSignature,
    GenericFunction getActualType,  // pointer of function to get the rtti info of this class(MyClass)
    const char *upcastSignature,
    GenericFunction upcast,  // pointer of function to upcast this class to base class(nullptr)
    const char *downcastSignature,
    GenericFunction downcast,  // pointer of function to downcast base class to this class(nullptr)
    const char *className,     // class name("MyClass")
    const char *destructorSignature,
    GenericFunction destructor)  // pointer of delete function
{

    // printf("---------------------------\n");
    // printf("%s\n%s\n%s\n%p\n%s\n%p\n%s\n%p\n%s\n%p\n%s\n%s\n%p\n", ((std::type_info *)classType)->name(), ((std::type_info *)pointerType)->name(), ((std::type_info *)constPointerType)->name(), baseClassType, getActualTypeSignature, getActualType, upcastSignature, upcast, downcastSignature, downcast, className, destructorSignature, destructor);
    // printf("---------------------------\n");
}
GenericFunction construct = nullptr;
void _embind_register_class_constructor(
    TYPEID classType,
    unsigned argCount,
    const TYPEID argTypes[],
    const char *invokerSignature,
    GenericFunction invoker,
    GenericFunction constructor)
{
    // printf("--------------register constructor-------------\n");
    // printf("argCount: %d\n", argCount);
    construct = constructor;
    // printf("~~~~ constructor: %p\n",construct);
}

void _embind_register_class_function(
    TYPEID classType,
    const char *methodName,
    unsigned argCount,
    const TYPEID argTypes[],
    const char *invokerSignature,
    GenericFunction invoker,
    void *context,
    unsigned isPureVirtual)
{
}

void _embind_register_class_property(
    TYPEID classType,
    const char *fieldName,
    TYPEID getterReturnType,
    const char *getterSignature,
    GenericFunction getter,
    void *getterContext,
    TYPEID setterArgumentType,
    const char *setterSignature,
    GenericFunction setter,
    void *setterContext)
{
}

void _embind_register_class_class_function(
    TYPEID classType,
    const char *methodName,
    unsigned argCount,
    const TYPEID argTypes[],
    const char *invokerSignature,
    GenericFunction invoker,
    GenericFunction method)
{
}

void _embind_register_class_class_property(
    TYPEID classType,
    const char *fieldName,
    TYPEID fieldType,
    const void *fieldContext,
    const char *getterSignature,
    GenericFunction getter,
    const char *setterSignature,
    GenericFunction setter) {}
}
///////////////////////////////////////////////////////////////////////////////////





static napi_value Method(napi_env env, napi_callback_info info)
{
    napi_value world;
    const char *str = "world";
    size_t str_len = strlen(str);
    NAPI_CALL(env, napi_create_string_utf8(env, str, str_len, &world));
    return world;
}

static napi_value Create(napi_env env, napi_callback_info info)
{
    napi_value instance;
    MyClass *(*factory)(int &&x) = nullptr;
    // printf("~~~ %s\n", typeid(MyClass).name());
    // printf("~~~ %s\n", typeid(MyClass *).name());

    // printf("~~~ %s\n", typeid(&internal::operator_new<MyClass, int>).name());
    // printf("~~~ %s\n", typeid(MyClass * (*)(int &&x)).name());
    // printf("~~~ %s\n", typeid(MyClass * (*)(int x)).name());

    // printf("~~~ %p\n", constructor);
    factory = reinterpret_cast<MyClass *(*)(int &&x)>(construct);
    // printf("~~~ %p\n", factory);
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, NULL, NULL);

    int32_t x = 0;
    napi_get_value_int32(env, args[0], &x);
    MyClass *p = factory(std::move(x));
    printf("========= create MyClass %p \n", p);

    napi_create_external(env, p, NULL, NULL, &instance);
    return instance;
}
static napi_value getStringFromInstance(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, NULL, NULL);

    void *p = nullptr;
    assert(napi_get_value_external(env, args[0], &p) == napi_ok);
    printf("========= create MyClass %p \n", p);
}
// static napi_value Initialize(napi_env env, napi_callback_info info) {}
// static napi_value Call(napi_env env, napi_callback_info info) {}
// static napi_value Release(napi_env env, napi_callback_info info) {}


// static napi_value GetValue(napi_env env, napi_callback_info info) {}
// static napi_value GetError(napi_env env, napi_callback_info info) {}

static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        NAPI_DECLARE_METHOD("MyClass", Create),
        NAPI_DECLARE_METHOD("getStringFromInstance", getStringFromInstance),

        // NAPI_DECLARE_METHOD("initialize", Initialize),
        // NAPI_DECLARE_METHOD("call", Call),
        // NAPI_DECLARE_METHOD("release", Release),

        // {"error", 0, 0, GetError, 0, 0, napi_default, 0},
        // {"version", 0, 0, GetValue, 0, 0, napi_default, 0},
        NAPI_DECLARE_METHOD("hello", Method)};

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc)/sizeof(desc[0]), desc));

    // MyClass *p = (MyClass *)new char[sizeof(MyClass)];
    // p = new (p) MyClass(10, "hello");

    // p = p->MyClass::MyClass();


    return exports;
}

NAPI_MODULE(node_plugin, Init)