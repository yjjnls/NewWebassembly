#include <string>

class MyClass
{
 public:
    MyClass()
        : x_(0), y_("default")
    {
        printf("~~~~~~~~~~~~~\ninvoke constructor1  \nx=%d y=%s \n~~~~~~~~~~~~~\n", x_, y_.c_str());
    }

    MyClass(int x)
        : x_(x), y_("default")
    {
        printf("~~~~~~~~~~~~~\ninvoke constructor2 \nx=%d y=%s \n~~~~~~~~~~~~~\n", x_, y_.c_str());
    }

    MyClass(int x, const std::string &y)
        : x_(x), y_(y)
    {
        printf("~~~~~~~~~~~~~\ninvoke constructor3 \nx=%d y=%s \n~~~~~~~~~~~~~\n", x_, y_.c_str());
    }

    ~MyClass()
    {
    }


    void incrementX()
    {
        printf("~~~~~~~~~~~ incrementX \n");
        ++x_;
    }
    void incrementX(int i)
    {
        printf("~~~~~~~~~~~ incrementX %d \n", i);
        x_ += i;
    }

    int getX() const { return x_; }
    void setX(int x) { x_ = x; }

    static std::string getStringFromInstance(const MyClass &instance);

    MyClass operator()(const MyClass &m)
    {
        printf("~~~~~~~invoke operator ()\n");
        return m;
    }

 private:
    int x_;
    std::string y_;
};