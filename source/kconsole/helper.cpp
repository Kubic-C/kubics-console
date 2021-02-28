#include "headers/helper.hpp"

namespace kconsole
{
    void set_2D_wchar_array(
        wchar_t** array_,
        unsigned int width,
        unsigned int height,
        wchar_t value
    )
    {
        for (unsigned int x = 0; x < width; x++)
            for (unsigned int y = 0; y < height; y++)
                array_[x][y] = value;
    }
}