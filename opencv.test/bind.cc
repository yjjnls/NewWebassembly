
#include <emscripten/bind.h>
#include <new>
#include <node_api.h>
#include "common.h"
#include "../../example.h"
#include <map>

class Addon
{
 public:
    Addon() {}
    ~Addon() {}
    static void Init(napi_env env, napi_value exports);
    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

 private:
    static napi_value New(napi_env env, napi_callback_info info);
    static napi_value create(napi_env env, napi_callback_info info);
    static napi_value roi(napi_env env, napi_callback_info info);
    static napi_value get_rows(napi_env env, napi_callback_info info);
    static napi_value set_rows(napi_env env, napi_callback_info info);
    static napi_value get_data32S(napi_env env, napi_callback_info info);

    static napi_ref constructor;

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