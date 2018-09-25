
#include <emscripten/bind.h>
#include <node_api.h>
#include "common.h"
#include "../../example.h"

using namespace emscripten::internal;

extern "C" {
void _embind_register_class(
    TYPEID classType,
    TYPEID pointerType,
    TYPEID constPointerType,
    TYPEID baseClassType,
    const char *getActualTypeSignature,
    GenericFunction getActualType,
    const char *upcastSignature,
    GenericFunction upcast,
    const char *downcastSignature,
    GenericFunction downcast,
    const char *className,
    const char *destructorSignature,
    GenericFunction destructor) {}

void _embind_register_class_constructor(
    TYPEID classType,
    unsigned argCount,
    const TYPEID argTypes[],
    const char *invokerSignature,
    GenericFunction invoker,
    GenericFunction constructor) {}

void _embind_register_class_function(
    TYPEID classType,
    const char *methodName,
    unsigned argCount,
    const TYPEID argTypes[],
    const char *invokerSignature,
    GenericFunction invoker,
    void *context,
    unsigned isPureVirtual) {}

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
    void *setterContext) {}

void _embind_register_class_class_function(
    TYPEID classType,
    const char *methodName,
    unsigned argCount,
    const TYPEID argTypes[],
    const char *invokerSignature,
    GenericFunction invoker,
    GenericFunction method) {}

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

class Addon
{
 public:
    Addon()
        : target_(nullptr)
        , env_(nullptr)
        , wrapper_(nullptr) {}
    ~Addon()
    {
        napi_delete_reference(env_, wrapper_);
        if (target_ != nullptr) {
            delete target_;
            target_ = nullptr;
        }
    }
    static void Init(napi_env env, napi_value exports);
    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);
    static napi_status NewInstance(napi_env env, napi_callback_info info, napi_value *instance);
    static napi_value CreateObject(napi_env env, napi_callback_info info);
    MyClass *target() { return target_; }

 private:
    static napi_value Release(napi_env env, napi_callback_info info);
    static napi_value New(napi_env env, napi_callback_info info);


    ////////////////////////////////////////////////////////////////////////
    static napi_value incrementX(napi_env env, napi_callback_info info);
    static napi_value oper(napi_env env, napi_callback_info info);
    static napi_value getX(napi_env env, napi_callback_info info);
    static napi_value setX(napi_env env, napi_callback_info info);
    ////////////////////////////////////////////////////////////////////////

    static napi_ref constructor_;
    MyClass *target_;
    napi_env env_;
    napi_ref wrapper_;
};

namespace binding_utils {
extern void increase(MyClass &, int, int);
}  // namespace binding_utils
using namespace binding_utils;


MyClass *constructor_factory(napi_env env, size_t argc, napi_value *args)
{
    MyClass *p = nullptr;
    switch (argc) {
        case 0: {
            p = new MyClass();
        } break;
        case 1: {
            int32_t arg0 = 0;
            napi_get_value_int32(env, args[0], &arg0);

            p = new MyClass(arg0);
        } break;
        case 2: {
            int32_t arg0 = 0;
            napi_get_value_int32(env, args[0], &arg0);

            size_t strlen;
            napi_get_value_string_utf8(env, args[1], NULL, 0, &strlen);
            std::string arg1(strlen + 1, 0);
            size_t res;
            napi_get_value_string_utf8(env, args[1], (char *)arg1.c_str(), strlen + 1, &res);

            p = new MyClass(arg0, arg1);
        } break;
        case 3: {
        } break;
        case 4: {
        } break;
    }
    return p;
}

void fun_incrementX_factory(MyClass *obj, napi_env env, size_t argc, napi_value *args)
{
    switch (argc) {
        case 0: {
            return obj->incrementX();
        } break;
        case 1: {
            int32_t arg0 = 0;
            napi_get_value_int32(env, args[0], &arg0);

            return obj->incrementX(arg0);
        } break;
        case 2: {
            int32_t arg0 = 0;
            napi_get_value_int32(env, args[0], &arg0);
            int32_t arg1 = 0;
            napi_get_value_int32(env, args[1], &arg1);

            return binding_utils::increase(*obj, arg0, arg1);
        } break;
    }
}

MyClass fun_oper_factory(MyClass *obj, napi_env env, size_t argc, napi_value *args)
{
    switch (argc) {
        case 1: {
            Addon *p;
            napi_unwrap(env, args[0], reinterpret_cast<void **>(&p));
            MyClass &arg0 = *(p->target());
            return (*obj)(arg0);
        } break;
    }
}

///////////////////////////////////////////////////////////////////////////////////


napi_ref Addon::constructor_;
void Addon::Destructor(napi_env env, void *nativeObject, void *finalize_hint)
{
    printf("~~~~~~~  Destructor  ~~~~~~~~");
    Addon *obj = static_cast<Addon *>(nativeObject);
    delete obj;
}
napi_value Addon::Release(napi_env env, napi_callback_info info)
{
    return nullptr;
}

napi_value Addon::CreateObject(napi_env env, napi_callback_info info)
{
    napi_value instance;
    Addon::NewInstance(env, info, &instance);

    return instance;
}

void Addon::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        NAPI_DECLARE_METHOD("delete", Release),
        //property
        {"x", nullptr, nullptr, getX, setX, 0, napi_default, 0},
        //function(return type, arguments, function name)
        NAPI_DECLARE_METHOD("incrementX", incrementX),
        NAPI_DECLARE_METHOD("oper", oper)

    };

    napi_value cons;
    napi_define_class(env,
                      "MyClass",
                      -1,
                      New,
                      nullptr,
                      sizeof(properties) / sizeof(properties[0]),
                      properties,
                      &cons);

    napi_create_reference(env, cons, 1, &constructor_);

    napi_set_named_property(env, exports, "MyClass", cons);
}



napi_value Addon::New(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value _this;
    napi_get_cb_info(env, info, &argc, nullptr, &_this, nullptr);
    napi_value args[argc];
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);

    Addon *obj = new Addon();
    MyClass *target = constructor_factory(env, argc, args);

    obj->env_ = env;
    obj->target_ = target;
    env, napi_wrap(env, _this, obj, Addon::Destructor,
                   nullptr,  // finalize_hint
                   &obj->wrapper_);

    return _this;
}
napi_status Addon::NewInstance(napi_env env, napi_callback_info info, napi_value *instance)
{
    napi_status status;

    size_t argc = 0;
    napi_value _this;
    napi_get_cb_info(env, info, &argc, nullptr, &_this, nullptr);
    napi_value args[argc];
    assert(napi_get_cb_info(env, info, &argc, args, &_this, nullptr) == napi_ok);

    napi_value cons;
    status = napi_get_reference_value(env, constructor_, &cons);
    if (status != napi_ok)
        return status;

    status = napi_new_instance(env, cons, argc, args, instance);
    if (status != napi_ok)
        return status;

    return napi_ok;
}
napi_value Addon::incrementX(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value _this;
    napi_get_cb_info(env, info, &argc, nullptr, &_this, nullptr);
    napi_value args[argc];
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);

    Addon *obj;
    napi_unwrap(env, _this, reinterpret_cast<void **>(&obj));

    ////////////////////////////////////////////////////////////////////////
    MyClass *target = obj->target_;
    fun_incrementX_factory(target, env, argc, args);
    ////////////////////////////////////////////////////////////////////////

    return nullptr;
}
napi_value Addon::oper(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value _this;
    napi_get_cb_info(env, info, &argc, nullptr, &_this, nullptr);
    napi_value args[argc];
    napi_get_cb_info(env, info, &argc, args, &_this, nullptr);

    Addon *obj;
    env, napi_unwrap(env, _this, reinterpret_cast<void **>(&obj));

    ////////////////////////////////////////////////////////////////////////
    MyClass *target = obj->target_;
    MyClass res = fun_oper_factory(target, env, argc, args);
    ////////////////////////////////////////////////////////////////////////

    napi_value result;
    napi_create_external(env, &res, nullptr, nullptr, &result);

    return result;
}
napi_value Addon::getX(napi_env env, napi_callback_info info)
{
    napi_value _this;
    env, napi_get_cb_info(env, info, nullptr, nullptr, &_this, nullptr);

    Addon *obj;
    napi_unwrap(env, _this, reinterpret_cast<void **>(&obj));

    ////////////////////////////////////////////////////////////////////////
    MyClass *target = obj->target_;
    napi_value res;
    napi_create_int32(env, target->getX(), &res);
    printf("=========== x : %d\n", target->getX());
    ////////////////////////////////////////////////////////////////////////

    return res;
}
napi_value Addon::setX(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value _this;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));

    Addon *obj;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void **>(&obj)));

    ////////////////////////////////////////////////////////////////////////
    MyClass *target = obj->target_;
    int32_t value;
    napi_get_value_int32(env, args[0], &value);
    target->setX(value);
    ////////////////////////////////////////////////////////////////////////

    return nullptr;
}

napi_value getStringFromInstance(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_get_cb_info(env, info, &argc, nullptr, nullptr, nullptr);
    napi_value args[argc];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    Addon *arg0;
    napi_unwrap(env, args[0], reinterpret_cast<void **>(&arg0));
    std::string res = MyClass::getStringFromInstance(*(arg0->target()));

    napi_value result;
    napi_create_string_utf8(env, res.c_str(), res.size(), &result);
    return result;
}

napi_value Init(napi_env env, napi_value exports)
{
    Addon::Init(env, exports);

    napi_property_descriptor desc[] = {

        NAPI_DECLARE_METHOD("getStringFromInstance", getStringFromInstance),
        NAPI_DECLARE_METHOD("createObject", Addon::CreateObject),
    };

    napi_define_properties(env,
                           exports,
                           sizeof(desc) / sizeof(*desc),
                           desc);
    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)