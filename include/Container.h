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
        _factoryMap[typeid(T)] = { State::INSTANCE_GENERATOR, std::move(gen) };
    }

    template <Parameterized T>
    void registerFactory(ParameterizedGenerator<T> gen)
    {
        _factoryMap[typeid(T)] = { State::INSTANCE_GENERATOR, std::move(gen) };
    }

    template <class T>
    void registerSingleton(Generator<T> gen)
    {
        _factoryMap[typeid(T)] = { State::SINGLETON_GENERATOR, std::move(gen) };
    }

    template <NotParameterized T>
    std::shared_ptr<T> resolve()
    {
        return resolve<T, Generator<T>>();
    }

    template <Parameterized T>
    std::shared_ptr<T> resolve()
    {
        return resolve<T, Generator<T>>();
    }

    template <Parameterized T>
    std::shared_ptr<T> resolve(typename T::Params&& params)
    {
        return resolve<T, ParameterizedGenerator<T>>(std::forward<typename T::Params>(params));
    }

    static Container& get()
    {
        static Container c;
        return c;
    }

private:
    enum class State
    {
        INSTANCE_GENERATOR,
        SINGLETON_GENERATOR,
        SINGLETON_INSTANCE,
    };

    struct Entry
    {
        State state;
        std::any content;
    };

    template <class T, class G, class... Ps>
    std::shared_ptr<T> resolve(Ps&&... args)
    {
        if (const auto i = _factoryMap.find(typeid(T)); i != _factoryMap.end())
        {
            auto& entry = i->second;

            if (entry.state != State::INSTANCE_GENERATOR && sizeof...(Ps) > 0)
                throw std::runtime_error { "Parameters passed in while resolving a singleton!" };
            if (entry.state == State::INSTANCE_GENERATOR && Parameterized<T> && sizeof...(Ps) == 0)
                throw std::runtime_error { "Parameters passed in while resolving an independent instance!" };

            try
            {
                std::shared_ptr<T> ptr;
                switch (entry.state)
                {
                    using enum State;
                case INSTANCE_GENERATOR:
                    ptr = std::any_cast<G>(entry.content)(std::forward<Ps>(args)...);
                    break;
                case SINGLETON_GENERATOR:
                    ptr = std::any_cast<G>(entry.content)(std::forward<Ps>(args)...);
                    entry.content = ptr;
                    entry.state = SINGLETON_INSTANCE;
                    break;
                case SINGLETON_INSTANCE:
                    ptr = std::any_cast<std::shared_ptr<T>>(entry.content);
                    break;
                }
                return ptr;
            }
            catch (const std::bad_any_cast&)
            {
                throw std::runtime_error { "Could not resolve services mapped type\n from: ["
                    + std::string { entry.content.type().name() } + "]\n to: [" + std::string { typeid(G).name() }
                    + "]." };
            }
        }
        else
        {
            throw std::runtime_error { "Could not find generator for type [" + std::string { typeid(T).name() }
                + "] in IoC container" };
        }
    }

    std::unordered_map<std::type_index, Entry> _factoryMap;
};