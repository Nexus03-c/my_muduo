#ifndef MY_MUDUO_NONECOPYABLE_H
#define MY_MUDUO_NONECOPYABLE_H

class nonecopyable {
protected:
    nonecopyable() = default;
    ~nonecopyable() = default;
private:
    nonecopyable(const nonecopyable &object);
    nonecopyable& operator=(const nonecopyable &object){};
};

#endif