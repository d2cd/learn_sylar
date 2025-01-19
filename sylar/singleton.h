#ifndef __SYLAR_SINGLETON_H__
#define __SYLAR_SINGLETON_H__

namespace sylar{
template<class T, class X = void, int N=0>
class Singleton{
public:
    static T* getInstance(){
        static T singleton;
        return &singleton;
    }
private:
    Singleton();
    Singleton(const T& m) = delete;
    Singleton& operator=(const T& m) = delete;
};

template<class T, class X = void, int N=0>
class SingletonPtr{
public:
    static std::shared_ptr<T> getInstance(){
        static std::shared_ptr<T> v(new T);
        return v;
    }
private:
    SingletonPtr();
    SingletonPtr(const std::shared_ptr<T>& m) =  delete;
    std::shared_ptr<T> operator=(const std::shared_ptr<T>& m) = delete;
}; 
}

#endif