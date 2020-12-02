#include <iostream>
#include "xmlService.hpp"

int main(int argc, char** argv)
{
    uint32_t portNr;
    if (argc == 1)
    {
        std::cout << "Specify port to connect to: ";
        std::cin >> portNr;
    }
    else if (argc > 1)
    {
        portNr = std::stoi(argv[1]);
    }

    xmlServer::XmlService::start("127.0.0.1", portNr);

    return 0;
}