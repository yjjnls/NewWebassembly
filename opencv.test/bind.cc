
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
    TYPEID classType,         // rtti info of this class(Mat)
    TYPEID pointerType,       // rtti info of Mat
    TYPEID constPointerType,  // rtti info of Mat
    TYPEID baseClassType,     // rtti info of base class(nullptr)
    const char *getActualTypeSignature,
    GenericFunction getActualType,  // pointer of function to get the rtti info of this class(Mat)
    const char *upcastSignature,
    GenericFunction upcast,  // pointer of function to upcast this class to base class(nullptr)
    const char *downcastSignature,
    GenericFunction downcast,  // pointer of function to downcast base class to this class(nullptr)
    const char *className,     // class name("Mat")
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

template <typename ClassType>
ClassType *constructor_factory(napi_env env, size_t argc, napi_value *args)
{
    ClassType *p = nullptr;
    switch (argc) {
        case 0: {
            p = new ClassType();
        } break;
        case 1: {
            void *arg0 = nullptr;
            napi_get_value_external(env, args[0], arg0);

            p = new ClassType(*(ClassType *)arg0);
        } break;
        case 2: {
            void *arg0 = nullptr;
            napi_get_value_external(env, args[0], arg0);
            int32_t arg1 = 0;
            napi_get_value_int32(env, args[1], &arg1);

            p = new ClassType(*(Size *)arg0, arg1);
        } break;
        case 3: {
        } break;
        case 4: {
        } break;
    }
    return p;
}

template <typename ClassType>
void fun_create_factory(ClassType *obj, napi_env env, size_t argc, napi_value *args)
{
    switch (argc) {
        case 2: {
            void *arg0 = nullptr;
            napi_get_value_external(env, args[0], arg0);
            int32_t arg1 = 0;
            napi_get_value_int32(env, args[1], &arg1);

            obj->cv::Mat::create(*(Size *)arg0, arg1);
        } break;
        case 3: {
            int32_t arg0 = 0;
            napi_get_value_int32(env, args[0], &arg0);
            int32_t arg1 = 0;
            napi_get_value_int32(env, args[1], &arg1);
            int32_t arg2 = 0;
            napi_get_value_int32(env, args[2], &arg2);

            obj->cv::Mat::create(arg0, arg1, arg2);
        } break;
    }
}

template <typename ClassType>
ClassType roi_create_factory(ClassType *obj, napi_env env, size_t argc, napi_value *args)
{
    switch (argc) {
        case 1: {
            void *arg0 = nullptr;
            napi_get_value_external(env, args[0], arg0);

            return (*obj)(*(Rect *)arg0);
        } break;
    }
}

template <typename ClassType>
ClassType roi_create_factory(ClassType *obj, napi_env env, size_t argc, napi_value *args)
{
    switch (argc) {
        case 1: {
            void *arg0 = nullptr;
            napi_get_value_external(env, args[0], arg0);

            return (*obj)(*(Rect *)arg0);
        } break;
    }
}
///////////////////////////////////////////////////////////////////////////////////


napi_ref Mat::constructor;
void Mat::Destructor(napi_env env, void *nativeObject, void *finalize_hint)
{
    Mat *obj = static_cast<Mat *>(nativeObject);
    delete obj;
}

void Mat::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        //property
        {"rows", nullptr, nullptr, get_rows, set_rows, 0, napi_default, 0},
        {"data32S", nullptr, nullptr, get_data32S, nullptr, 0, napi_default, 0},
        //function(return type, arguments, function name)
        NAPI_DECLARE_METHOD("ones", ones),
        NAPI_DECLARE_METHOD("create", create),
        NAPI_DECLARE_METHOD("roi", roi)

    };

    napi_value cons;
    NAPI_CALL_RETURN_VOID(env,
                          napi_define_class(env,
                                            "Mat",
                                            -1,
                                            New,
                                            nullptr,
                                            sizeof(properties) / sizeof(properties[0]),
                                            properties,
                                            &cons));

    NAPI_CALL_RETURN_VOID(env, napi_create_reference(env, cons, 1, &constructor));

    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, exports, "Mat", cons));
}

napi_value Mat::New(napi_env env, napi_callback_info info)
{
    napi_value new_target;
    NAPI_CALL(env, napi_get_new_target(env, info, &new_target));
    bool is_constructor = (new_target != nullptr);

    size_t argc = 0;
    napi_value _this;
    napi_get_cb_info(env, info, &argc, nullptr, &_this, nullptr);
    napi_value args[argc];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));

    // printf("constructor arguments: %d\n", argc);
    // napi_valuetype valuetype;
    // for (int i = 0; i < argc; ++i) {
    //     NAPI_CALL(env, napi_typeof(env, args[i], &valuetype));
    //     printf("argument %d type: %d\n", i, valuetype);
    // }

    if (is_constructor) {
        // Invoked as constructor: `new Mat(...)`
        Mat *obj = constructor_factory<Mat>(env, argc, args);

        obj->env_ = env;
        NAPI_CALL(env, napi_wrap(env, _this, obj, Mat::Destructor,
                                 nullptr,  // finalize_hint
                                 &obj->wrapper_));

        return _this;
    }

    // Invoked as plain function `Mat(...)`, turn into construct call.
    argc = 1;
    napi_value argv[1] = {args[0]};

    napi_value cons;
    NAPI_CALL(env, napi_get_reference_value(env, constructor, &cons));

    napi_value instance;
    NAPI_CALL(env, napi_new_instance(env, cons, argc, argv, &instance));

    return instance;
}

napi_value Mat::create(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value _this;
    napi_get_cb_info(env, info, &argc, nullptr, &_this, nullptr);
    napi_value args[argc];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));

    Mat *obj;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void **>(&obj)));

    ////////////////////////////////////////////////////////////////////////
    Mat *res = fun_create_factory(obj, env, argc, args);
    ////////////////////////////////////////////////////////////////////////

    return nullptr;
}
napi_value Mat::roi(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value _this;
    napi_get_cb_info(env, info, &argc, nullptr, &_this, nullptr);
    napi_value args[argc];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));

    Mat *obj;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void **>(&obj)));

    ////////////////////////////////////////////////////////////////////////
    Mat res = fun_roi_factory(obj, env, argc, args);
    ////////////////////////////////////////////////////////////////////////

    napi_value result;
    napi_create_external(env, &res, nullptr, nullptr, &result);

    return result;
}
napi_value Mat::get_rows(napi_env env, napi_callback_info info)
{
    napi_value _this;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &_this, nullptr));

    Mat *obj;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void **>(&obj)));

    napi_value num;
    NAPI_CALL(env, napi_create_int32(env, obj->rows, &num));

    return num;
}

napi_value Mat::set_rows(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value _this;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));

    Mat *obj;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void **>(&obj)));

    NAPI_CALL(env, napi_get_value_int32(env, args[0], &obj->rows));

    return nullptr;
}

napi_value Mat::get_data32S(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value _this;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));

    Mat *obj;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void **>(&obj)));

    ////////////////////////////////////////////////////////////////////////
    emscripten::val res = binding_utils::matData<int>(*obj);
    ////////////////////////////////////////////////////////////////////////

    napi_value result;
    napi_create_external(env, &res, nullptr, nullptr, &result);

    return result;
}

napi_value Init(napi_env env, napi_value exports)
{
    Mat::Init(env, exports);
    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)