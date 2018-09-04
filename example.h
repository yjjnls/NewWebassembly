#include <string>


class MyClass
{
 public:
    MyClass(int x_)
        : x(x_)
        , y("hello")
    {
        printf("---------invoke MyClass constructor--------\n");
    }

    void incrementX()
    {
        ++x;
    }

    int getX() const { return x; }
    void setX(int x_) { x = x_; }

    static std::string getStringFromInstance(const MyClass &instance);

 private:
    int x;
    std::string y;
};