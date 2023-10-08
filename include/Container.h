#pragma once

#include <unordered_map>
#include <typeindex>
#include <any>
#include <functional>
#include <memory>
#include <exception>

template <class T>
concept Parameterized = requires()
{
    { typename T::Params {} };
};

template <class T>
concept NotParameterized = !Parameterized<T>;

class Container
{
public:
    template <NotParameterized T>
    using Generator = std::function<std::shared_ptr<T>()>;

    template <Parameterized T>
    using ParameterizedGenerator = std::function<std::shared_ptr<T>(typename T::Params params)>;

    template <NotParameterized T>
    void registerFactory(Generator<T> gen)
    {
        _factoryMap[typeid(T)] = gen;
    }

    template <Parameterized T>
    void registerFactory(ParameterizedGenerator<T> gen)
    {
        _factoryMap[typeid(T)] = gen;
    }

    template <NotParameterized T>
    std::shared_ptr<T> resolve()
    {
        return resolve<T, Generator<T>>();
    }

    template <Parameterized T>
    std::shared_ptr<T> resolve(typename T::Params&& params = {})
    {
        return resolve<T, ParameterizedGenerator<T>>(std::forward<typename T::Params>(params));
    }

    static Container& get()
    {
        static Container c;
        return c;
    }

private:
    template <class T, class G, class... Ps>
    std::shared_ptr<T> resolve(Ps&&... args)
    {
        if (!_factoryMap.contains(typeid(T)))
            throw std::runtime_error { "Could not find generator for type [" + std::string { typeid(T).name() }
                + "] in IoC container" };

        try
        {
            return std::any_cast<G>(_factoryMap.at(typeid(T)))(std::forward<Ps>(args)...);
        }
        catch (const std::bad_any_cast&)
        {
            throw std::runtime_error { "Could not resolve services mapped type\n from: ["
                + std::string { _factoryMap.at(typeid(T)).type().name() } + "]\n to: ["
                + std::string { typeid(G).name() } + "]." };
        }
    }

    std::unordered_map<std::type_index, std::any> _factoryMap;
};