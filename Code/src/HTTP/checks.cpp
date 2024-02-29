#include "checks.hpp"

namespace AxxonsoftInternProject
{
    bool checks::characters::IsDigid(char symbol)
    {
        return symbol >= '0' && symbol <= '9';
    }

    bool checks::characters::IsChar(char symbol)
    {
        return symbol >= 0 && symbol <= 127;
    }

    bool checks::characters::IsControlChar(char symbol)
    {
        return symbol >= 0 && symbol <= 31 || symbol == 127;
    }

    bool checks::characters::IsSpesialChar(char symbol)
    {
        for (auto specialSymbol : spesialCharacters)
        {
            if (symbol == specialSymbol)
            {
                return true;
            }
        }

        return false;
    }
}