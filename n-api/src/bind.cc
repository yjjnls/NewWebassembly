
#include <emscripten/bind.h>
#include <new>
#include <node_api.h>
#include "common.h"
#include "../../example.h"

class Addon
{
 public:
    Addon()
        : target_(nullptr)
        , env_(nullptr)
        , wrapper_(nullptr) {}
    ~Addon()
    {
        if (target_ != nullptr) {
            delete target_;
            target_ = nullptr;
        }
    }
    static void Init(napi_env env, napi_value exports);
    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

 private:
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
template <typename ClassType>
ClassType *constructor_factory(napi_env env, size_t argc, napi_value *args)
{
    ClassType *p = nullptr;
    switch (argc) {
        case 0: {
            p = new ClassType();
        } break;
        case 1: {
            int32_t arg0 = 0;
            napi_get_value_int32(env, args[0], &arg0);

            p = new ClassType(arg0);
        } break;
        case 2: {
            int32_t arg0 = 0;
            napi_get_value_int32(env, args[0], &arg0);

            size_t strlen;
            napi_get_value_string_utf8(env, args[1], NULL, 0, &strlen);
            std::string arg1(strlen + 1, 0);
            size_t res;
            napi_get_value_string_utf8(env, args[1], (char *)arg1.c_str(), strlen + 1, &res);

            p = new ClassType(arg0, arg1);
        } break;
        case 3: {
        } break;
        case 4: {
        } break;
    }
    return p;
}

template <typename ClassType>
void fun_incrementX_factory(ClassType *obj, napi_env env, size_t argc, napi_value *args)
{
    switch (argc) {
        case 0: {
            obj->incrementX();
        } break;
        case 1: {
            int32_t arg0 = 0;
            napi_get_value_int32(env, args[0], &arg0);

            obj->incrementX(arg0);
        } break;
    }
}

template <typename ClassType>
ClassType fun_oper_factory(ClassType *obj, napi_env env, size_t argc, napi_value *args)
{
    switch (argc) {
        case 1: {
            void *arg0 = nullptr;
            napi_get_value_external(env, args[0], &arg0);

            return (*obj)(*(MyClass *)arg0);
        } break;
    }
}
///////////////////////////////////////////////////////////////////////////////////


napi_ref Addon::constructor_;
void Addon::Destructor(napi_env env, void *nativeObject, void *finalize_hint)
{
    Addon *obj = static_cast<Addon *>(nativeObject);
    delete obj;
}

void Addon::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        //property
        {"x", nullptr, nullptr, getX, setX, 0, napi_default, 0},
        //function(return type, arguments, function name)
        NAPI_DECLARE_METHOD("incrementX", incrementX),
        NAPI_DECLARE_METHOD("oper", oper)

    };

    napi_value cons;
    NAPI_CALL_RETURN_VOID(env,
                          napi_define_class(env,
                                            "MyClass",
                                            -1,
                                            New,
                                            nullptr,
                                            sizeof(properties) / sizeof(properties[0]),
                                            properties,
                                            &cons));

    NAPI_CALL_RETURN_VOID(env, napi_create_reference(env, cons, 1, &constructor_));

    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, exports, "MyClass", cons));
}

napi_value Addon::New(napi_env env, napi_callback_info info)
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
        // Invoked as constructor: `new MyClass(...)`
        Addon *obj = new Addon();
        MyClass *target = constructor_factory<MyClass>(env, argc, args);

        obj->env_ = env;
        obj->target_ = target;
        NAPI_CALL(env, napi_wrap(env, _this, obj, Addon::Destructor,
                                 nullptr,  // finalize_hint
                                 &obj->wrapper_));

        return _this;
    }

    // Invoked as plain function `MyClass(...)`, turn into construct call.
    argc = 1;
    napi_value argv[1] = {args[0]};

    napi_value cons;
    NAPI_CALL(env, napi_get_reference_value(env, constructor_, &cons));

    napi_value instance;
    NAPI_CALL(env, napi_new_instance(env, cons, argc, argv, &instance));

    return instance;
}

napi_value Addon::incrementX(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value _this;
    napi_get_cb_info(env, info, &argc, nullptr, &_this, nullptr);
    napi_value args[argc];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));

    Addon *obj;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void **>(&obj)));

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
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));

    Addon *obj;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void **>(&obj)));

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
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &_this, nullptr));

    Addon *obj;
    NAPI_CALL(env, napi_unwrap(env, _this, reinterpret_cast<void **>(&obj)));

    ////////////////////////////////////////////////////////////////////////
    MyClass *target = obj->target_;
    napi_value num;
    NAPI_CALL(env, napi_create_int32(env, target->getX(), &num));
    ////////////////////////////////////////////////////////////////////////

    return num;
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
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &value));
    target->setX(value);
    ////////////////////////////////////////////////////////////////////////

    return nullptr;
}


napi_value Init(napi_env env, napi_value exports)
{
    Addon::Init(env, exports);
    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)