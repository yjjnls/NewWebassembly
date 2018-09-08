
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
                  const std::string &name,
                  unsigned argcount = 0,
                  TYPEID argtypes[] = nullptr)
        : fun(function)
        , funName(name)
        , argCount(argcount)
        , argTypes(nullptr)
    // , invoker(nullptr)
    // , setter(nullptr)
    {
        argTypes = new TYPEID[argCount];
        // memset(argTypes,0,sizeof(TYPEID)*argCount);
        for (int i = 0; i < argCount; ++i) {
            argTypes[i] = argtypes[i];
        }
    }

    ~function_info()
    {
        if (argTypes != nullptr) {
            delete[] argTypes;
            argTypes = nullptr;
        }
    }


    GenericFunction fun;
    std::string funName;

    unsigned argCount;

    // std::string invokerSignature;
    // GenericFunction invoker;

    // GenericFunction setter;
    TYPEID *argTypes;
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
    // printf("--------------register constructor: invokerSignature %s-------------\n", invokerSignature);
    // printf("argCount: %d\n", argCount);
    // for (int i = 0; i < argCount; ++i) {
    //     printf("%s\n", ((std::type_info *)argTypes[i])->name());
    // }
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

    // printf("function name: %s \n", fun_name.c_str());
    // printf("argCount: %d\n", argCount);
    // for (int i = 0; i < argCount; ++i) {
    //     printf("%s\n", ((std::type_info *)argTypes[i])->name());
    // }

    if (table[fun_name] == nullptr) {
        table[fun_name] = new function_map;
    }
    function_map *map = table[fun_name];
    function_info *info = new function_info(context, fun_name);
    (*map)[argCount - 2] = info;  // arg[0]:return_type arg[1]:class_type arg[2...]:function_args
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
    if (table[property_name] == nullptr) {
        table[property_name] = new function_map;
    }
    function_map *map = table[property_name];
    function_info *info_getter = new function_info(getterContext, property_name + ".getter");
    function_info *info_setter = new function_info(setterContext, property_name + ".setter");
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
// template <typename ReturnType, typename... Args>
// void exec_fun()
// {
//     typedef ReturnType (*fun)(Args...) = nullptr;
//     fun=fun(origin_f_ptr/* void * */);
//     fun(Args...);
// }
napi_value MyClass::New(napi_env env, napi_callback_info info)
{
    napi_value new_target;
    NAPI_CALL(env, napi_get_new_target(env, info, &new_target));
    bool is_constructor = (new_target != nullptr);

    size_t argc = 0;
    napi_value _this;
    napi_get_cb_info(env, info, &argc, nullptr, &_this, nullptr);
    napi_value args[argc];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));

    printf("constructor arguments: %d\n", argc);
    napi_valuetype valuetype;
    for (int i = 0; i < argc; ++i) {
        NAPI_CALL(env, napi_typeof(env, args[i], &valuetype));
        printf("argument %d type: %d\n", i, valuetype);
    }

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
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &_this, nullptr));

    MyClass *obj;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void **>(&obj)));

    ////////////////////////////////////////////////////////////////////////
    size_t argc = 0;
    napi_get_cb_info(env, info, &argc, nullptr, &_this, nullptr);
    // printf("increment arg: %d\n", argc);
    function_info *f_info = (*table["incrementX"])[argc];
    assert(f_info != NULL);
    void *fun = f_info->fun;
    typedef void (MyClass::*pf)();
    pf p = *reinterpret_cast<pf *>(fun);
    (obj->*p)();
    ////////////////////////////////////////////////////////////////////////

    napi_value num;
    // NAPI_CALL(env, napi_create_int32(env, obj->x_, &num));

    return num;
}

napi_value Init(napi_env env, napi_value exports)
{
    MyClass::Init(env, exports);
    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)