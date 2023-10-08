#pragma once

#include <memory>
#include <optional>

#include "Container.h"

class ComponentD
{
public:
    virtual ~ComponentD() = default;

    virtual std::string doProcessing() const { return "D"; }
};

class ComponentC
{
public:
    virtual ~ComponentC() = default;

    ComponentC(std::shared_ptr<ComponentD> compD)
        : _compD { std::move(compD) }
    {
    }

    virtual std::string doProcessing() const { return _compD->doProcessing() + "-C"; }

protected:
    std::shared_ptr<ComponentD> _compD;
};

class ComponentB
{
public:
    virtual ~ComponentB() = default;

    ComponentB(std::shared_ptr<ComponentC> compC)
        : _compC { std::move(compC) }
    {
    }

    virtual std::string doProcessing() const { return _compC->doProcessing() + "-B"; }

protected:
    std::shared_ptr<ComponentC> _compC;
};

class ComponentA
{
public:
    struct Params
    {
        std::optional<std::string> str;
    };

    virtual ~ComponentA() = default;

    ComponentA(std::shared_ptr<ComponentB> compB, Params params)
        : _compB { std::move(compB) }
        , _str { params.str.value_or("default") }
    {
    }

    virtual std::string doProcessing() const { return _compB->doProcessing() + "-A[" + _str + "]"; }

protected:
    std::shared_ptr<ComponentB> _compB;
    std::string _str;
};

inline void bootFramework()
{
    auto& ioc = Container::get();

    ioc.registerFactory<ComponentA>(
        [&](ComponentA::Params params) { return std::make_shared<ComponentA>(ioc.resolve<ComponentB>(), params); });
    ioc.registerFactory<ComponentB>([&] { return std::make_shared<ComponentB>(ioc.resolve<ComponentC>()); });
    ioc.registerFactory<ComponentC>([&] { return std::make_shared<ComponentC>(ioc.resolve<ComponentD>()); });
    ioc.registerFactory<ComponentD>([] { return std::make_shared<ComponentD>(); });
}