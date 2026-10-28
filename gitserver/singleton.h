#pragma once//**单类保证整个程序运行期间，这个类只能创建出唯一的一个对象，并且提供全局访问点，任何地方都能拿到这同一个对象**。
#include <memory>
#include <mutex>
#include <iostream>
template <typename T>
class Singleton {
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>& st) = delete;

    static std::shared_ptr<T> _instance;
public:
    static std::shared_ptr<T> GetInstance() {
        static std::once_flag s_flag;
        std::call_once(s_flag, [&]() {
            _instance = std::shared_ptr<T>(new T);
            });

        return _instance;
    }
    void PrintAddress() {
        std::cout << _instance.get() << std::endl;
    }
    ~Singleton() {
        std::cout << "this is singleton destruct" << std::endl;
    }
};

template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;