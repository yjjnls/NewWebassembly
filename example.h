#include <string>
#include <node_api.h>

class MyClass
{
 public:
    MyClass()
        : x_(0), y_("default"), env_(nullptr), wrapper_(nullptr)
    {
        printf("~~~~~~~~~~~~~\ninvoke constructor1  \nx=%d y=%s \n~~~~~~~~~~~~~\n", x_, y_.c_str());
    }

    MyClass(int x)
        : x_(x), y_("default"), env_(nullptr), wrapper_(nullptr)
    {
        printf("~~~~~~~~~~~~~\ninvoke constructor2 \nx=%d y=%s \n~~~~~~~~~~~~~\n", x_, y_.c_str());
    }

    MyClass(int x, const std::string &y)
        : x_(x), y_(y), env_(nullptr), wrapper_(nullptr)
    {
        printf("~~~~~~~~~~~~~\ninvoke constructor3 \nx=%d y=%s \n~~~~~~~~~~~~~\n", x_, y_.c_str());
    }

    ~MyClass()
    {
        printf("\ninvoke destructor\n");
        napi_delete_reference(env_, wrapper_);
    }


    void incrementX()
    {
        printf("~~~~~~~~~~~ incrementX \n");
        ++x_;
    }
    void incrementX(int i)
    {
        printf("~~~~~~~~~~~ incrementX %d \n",i);
        ++x_;
    }

    int getX() const { return x_; }
    void setX(int x) { x_ = x; }

    static std::string getStringFromInstance(const MyClass &instance);

    static void Init(napi_env env, napi_value exports);
    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

 private:
    static napi_value New(napi_env env, napi_callback_info info);
    static napi_value GetX(napi_env env, napi_callback_info info);
    static napi_value SetX(napi_env env, napi_callback_info info);
    static napi_value IncrementX(napi_env env, napi_callback_info info);
    // static napi_value Multiply(napi_env env, napi_callback_info info);
    static napi_ref constructor;

    int x_;
    std::string y_;

    napi_env env_;
    napi_ref wrapper_;
};