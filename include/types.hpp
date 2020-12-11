#ifndef TYPES_H
#define TYPES_H

#include "json.hpp"

namespace xmlServer
{
namespace types
{


    struct Position
    {
        uint32_t line;
        uint32_t character;
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Position, line, character);


}
}

#endif /* TYPES_H */