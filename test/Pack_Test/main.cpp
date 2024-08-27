#include <print>
#include <iostream>
#include <RxSeraph/Core/Pack.h>


auto main() -> int
{
    try
    {
        //ZQF::RxSeraph::Pack::Export("ArchPac.Dat", "ArchPac/", 0x05EBB40D);
        ZQF::RxSeraph::Pack::Export("ScnPac.Dat", "ScnPac/", 0x0);
    }
    catch (const std::exception& err)
    {
        std::println(std::cerr, "std::exception: {}", err.what());
    }
}
