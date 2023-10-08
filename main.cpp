#include <iostream>
#include <memory>
#include <exception>
#include <iostream>

#include "Framework.h"
#include "Container.h"

class ExtendedFromCompD : public ComponentD
{
public:
    std::string doProcessing() const override { return "Morb" + ComponentD::text; }
};

int main()
{
    try
    {
        bootFramework();

        Container::get().registerSingleton<ComponentD>([] { return std::make_shared<ExtendedFromCompD>(); });

        const auto compA = Container::get().resolve<ComponentA>(ComponentA::Params { .str = "parameterized" });
        std::cout << compA->doProcessing() << std::endl;

        const auto compD = Container::get().resolve<ComponentD>();
        compD->setText("joori!");

        std::cout << compA->doProcessing() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}