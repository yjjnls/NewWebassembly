
#include <emscripten/bind.h>
#include <new>
#include <node_api.h>
#include "common.h"
#include "../../example.h"
#include <map>

using namespace emscripten::internal;

/*
map<class_name, table(map)>
                  ↑
                map<function_name, function_map(hash_map)>
                                            ↑
                                    <arg_num, function_info>
*/
struct function_info
{
    function_info(GenericFunction function,
                  const std::string &name)
        : fun(function)
        , funName(name)
        , argCount(0)
        , invoker(nullptr)
        , setter(nullptr)
    {
    }

    GenericFunction fun;
    std::string funName;

    unsigned argCount;

    std::string invokerSignature;
    GenericFunction invoker;

    GenericFunction setter;
    // TYPEID argTypes[];
};
typedef std::map<int, function_info *> function_map;
std::map<std::string /*function name*/, function_map *> table;

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
    function_map *s = new function_map;
    (*s)[0] = new function_info(destructor, "destructor");
    table["destructor"] = s;
}
// GenericFunction construct = nullptr;
void _embind_register_class_constructor(
    TYPEID classType,
    unsigned argCount,
    const TYPEID argTypes[],
    const char *invokerSignature,
    GenericFunction invoker,
    GenericFunction constructor)
{
    printf("--------------register constructor: invokerSignature %s-------------\n", invokerSignature);
    printf("argCount: %d\n", argCount);
    for (int i = 0; i < argCount; ++i) {
        printf("%s\n", ((std::type_info *)argTypes[i])->name());
    }
    if (table["constructor"] == nullptr) {
        table["constructor"] = new function_map;
    }
    function_map *map = table["constructor"];
    function_info *info = new function_info(constructor, "constructor");
    // info->argCount=argCount;
    // info->argTypes=argTypes;
    (*map)[argCount - 1] = info;

    // construct = constructor;
    // printf("~~~~ constructor: %p\n",construct);
    // printf("%s\n", (typeid(std::string)).name());
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
    std::string fun_name = std::string(methodName);
    if (table[fun_name] == nullptr) {
        table[fun_name] = new function_map;
    }
    function_map *map = table[fun_name];
    function_info *info = new function_info(context, fun_name);
    (*map)[argCount - 1] = info;
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
    std::string property_name = std::string(fieldName);
    if (table[fun_name] == nullptr) {
        table[fun_name] = new function_map;
    }
    function_map *map = table[fun_name];
    function_info *info_getter = new function_info(getterContext, fun_name + ".getter");
    function_info *info_setter = new function_info(setterContext, fun_name + ".setter");
    (*map)[0] = info_getter;
    (*map)[1] = info_setter;
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
    std::string fun_name = std::string(methodName);
    if (table[fun_name] == nullptr) {
        table[fun_name] = new function_map;
    }
    function_map *map = table[fun_name];
    function_info *info = new function_info(method, fun_name);
    (*map)[argCount - 1] = info;
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





// static napi_value Method(napi_env env, napi_callback_info info)
// {
//     napi_value world;
//     const char *str = "world";
//     size_t str_len = strlen(str);
//     NAPI_CALL(env, napi_create_string_utf8(env, str, str_len, &world));
//     return world;
// }

// static napi_value Create(napi_env env, napi_callback_info info)
// {
//     napi_value instance;
//     MyClass *(*factory)(int &&x, const std::string &&y) = nullptr;
//     // printf("~~~ %s\n", typeid(MyClass).name());
//     // printf("~~~ %s\n", typeid(MyClass *).name());

//     // printf("~~~ %s\n", typeid(&internal::operator_new<MyClass, int>).name());
//     // printf("~~~ %s\n", typeid(MyClass * (*)(int &&x)).name());
//     printf("~~~~ %s\n", typeid(factory).name());

//     // printf("~~~ %p\n", constructor);
//     factory = reinterpret_cast<MyClass *(*)(int &&x, const std::string &&y)>(construct);
//     // printf("~~~ %p\n", factory);
//     size_t argc = 2;
//     napi_value args[2];
//     napi_get_cb_info(env, info, &argc, args, NULL, NULL);

//     int32_t x = 0;
//     napi_get_value_int32(env, args[0], &x);

//     size_t strlen;
//     napi_get_value_string_utf8(env, args[1], NULL, 0, &strlen);
//     std::string y(strlen + 1, 0);
//     size_t res;
//     napi_get_value_string_utf8(env, args[1], (char *)y.c_str(), strlen + 1, &res);
//     // napi_get_value_string_utf8(env, args[1], &x);

//     MyClass *p = factory(std::move(x), std::move(y));
//     printf("========= create MyClass %p \n", p);

//     napi_create_external(env, p, NULL, NULL, &instance);
//     return instance;
// }
// static napi_value getStringFromInstance(napi_env env, napi_callback_info info)
// {
//     size_t argc = 1;
//     napi_value args[1];
//     napi_get_cb_info(env, info, &argc, args, NULL, NULL);

//     void *p = nullptr;
//     assert(napi_get_value_external(env, args[0], &p) == napi_ok);
//     printf("========= create MyClass %p \n", p);
// }
// // static napi_value Initialize(napi_env env, napi_callback_info info) {}
// // static napi_value Call(napi_env env, napi_callback_info info) {}
// // static napi_value Release(napi_env env, napi_callback_info info) {}


// // static napi_value GetValue(napi_env env, napi_callback_info info) {}
// // static napi_value GetError(napi_env env, napi_callback_info info) {}



// static napi_value Init(napi_env env, napi_value exports)
// {
//     napi_property_descriptor desc[] = {
//         NAPI_DECLARE_METHOD("MyClass", Create),
//         NAPI_DECLARE_METHOD("getStringFromInstance", getStringFromInstance),

//         // NAPI_DECLARE_METHOD("initialize", Initialize),
//         // NAPI_DECLARE_METHOD("call", Call),
//         // NAPI_DECLARE_METHOD("release", Release),

//         // {"error", 0, 0, GetError, 0, 0, napi_default, 0},
//         // {"version", 0, 0, GetValue, 0, 0, napi_default, 0},
//         NAPI_DECLARE_METHOD("hello", Method)};

//     NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));

//     // MyClass *p = (MyClass *)new char[sizeof(MyClass)];
//     // p = new (p) MyClass(10, "hello");

//     // p = p->MyClass::MyClass();


//     return exports;
// }

// NAPI_MODULE(node_plugin, Init)



////////////////////////////////////////////////////////////////////////////////////////////////


// class MyClass
// {
//  public:
//     static void Init(napi_env env, napi_value exports);
//     static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

//  private:
//     explicit MyClass(int x, const std::string &y)
//         : x_(x), y_(y), env_(nullptr), wrapper_(nullptr) {}
//     ~MyClass() { napi_delete_reference(env_, wrapper_); };

//     static napi_value New(napi_env env, napi_callback_info info);
//     static napi_value GetValue(napi_env env, napi_callback_info info);
//     static napi_value SetValue(napi_env env, napi_callback_info info);
//     static napi_value PlusOne(napi_env env, napi_callback_info info);
//     static napi_value Multiply(napi_env env, napi_callback_info info);
//     static napi_ref constructor;

//     int x_;
//     std::string y_;
//     napi_env env_;
//     napi_ref wrapper_;
// };


napi_ref MyClass::constructor;
void MyClass::Destructor(napi_env env, void *nativeObject, void *finalize_hint)
{
    MyClass *obj = static_cast<MyClass *>(nativeObject);
    delete obj;
}

void MyClass::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        {"x", nullptr, nullptr, GetX, SetX, 0, napi_default, 0},
        NAPI_DECLARE_METHOD("incrementX", IncrementX),
        // NAPI_DECLARE_METHOD("multiply", Multiply),
    };

    napi_value cons;
    NAPI_CALL_RETURN_VOID(env, napi_define_class(env, "MyClass", -1, New, nullptr, sizeof(properties) / sizeof(properties[0]), properties, &cons));

    NAPI_CALL_RETURN_VOID(env, napi_create_reference(env, cons, 1, &constructor));

    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, exports, "MyClass", cons));
}

napi_value MyClass::New(napi_env env, napi_callback_info info)
{
    napi_value new_target;
    NAPI_CALL(env, napi_get_new_target(env, info, &new_target));
    bool is_constructor = (new_target != nullptr);

    size_t argc = 2;
    napi_value args[2];
    napi_value _this;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));

    if (is_constructor) {
        // Invoked as constructor: `new MyClass(...)`
        int32_t x = 0;
        napi_get_value_int32(env, args[0], &x);

        size_t strlen;
        napi_get_value_string_utf8(env, args[1], NULL, 0, &strlen);
        std::string y(strlen + 1, 0);
        size_t res;
        napi_get_value_string_utf8(env, args[1], (char *)y.c_str(), strlen + 1, &res);

        MyClass *obj = new MyClass(x, y);

        obj->env_ = env;
        NAPI_CALL(env, napi_wrap(env, _this, obj, MyClass::Destructor,
                                 nullptr,  // finalize_hint
                                 &obj->wrapper_));

        return _this;
    }

    // Invoked as plain function `MyClass(...)`, turn into construct call.
    argc = 1;
    napi_value argv[1] = {args[0]};

    napi_value cons;
    NAPI_CALL(env, napi_get_reference_value(env, constructor, &cons));

    napi_value instance;
    NAPI_CALL(env, napi_new_instance(env, cons, argc, argv, &instance));

    return instance;
}

napi_value MyClass::GetX(napi_env env, napi_callback_info info)
{
    napi_value _this;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &_this, nullptr));

    MyClass *obj;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void **>(&obj)));

    napi_value num;
    NAPI_CALL(env, napi_create_int32(env, obj->x_, &num));

    return num;
}

napi_value MyClass::SetX(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value _this;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));

    MyClass *obj;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void **>(&obj)));

    NAPI_CALL(env, napi_get_value_int32(env, args[0], &obj->x_));

    return nullptr;
}

napi_value MyClass::IncrementX(napi_env env, napi_callback_info info)
{
    napi_value _this;
    NAPI_CALL(env,
              napi_get_cb_info(env, info, nullptr, nullptr, &_this, nullptr));

    MyClass *obj;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void **>(&obj)));

    obj->x_ += 1;

    napi_value num;
    NAPI_CALL(env, napi_create_int32(env, obj->x_, &num));

    return num;
}

napi_value Init(napi_env env, napi_value exports)
{
    MyClass::Init(env, exports);
    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)