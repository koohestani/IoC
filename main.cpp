#include <iostream>

#include "Framework.h"
#include "Container.h"

int main()
{
    try
    {
        bootFramework();
        const auto compA = Container::get().resolve<ComponentA>();
        std::cout << compA->doProcessing() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}