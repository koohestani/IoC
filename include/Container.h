#pragma once

#include <unordered_map>
#include <typeindex>
#include <any>
#include <functional>
#include <memory>
#include <exception>

class Container
{
public:
    template <class T>
    using Generator = std::function<std::shared_ptr<T>()>;

    template <class T>
    void registerFactory(Generator<T> gen)
    {
        _factoryMap[typeid(T)] = gen;
    }

    template <class T>
    std::shared_ptr<T> resolve()
    {
        if (!_factoryMap.contains(typeid(T)))
            throw std::runtime_error { "Could not find generator for type [" + std::string { typeid(T).name() }
                + "] in IoC container" };

        return std::any_cast<Generator<T>>(_factoryMap.at(typeid(T)))();
    }

    static Container& get()
    {
        static Container c;
        return c;
    }

private:
    std::unordered_map<std::type_index, std::any> _factoryMap;
};