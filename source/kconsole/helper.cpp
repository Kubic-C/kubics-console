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

    void print(
        wchar_t** array_,
        unsigned int width,
        unsigned int height,
        unsigned int pos_x,
        unsigned int pos_y,
        const std::wstring& wstr
    )
    {
        unsigned int index = 0;
        for(unsigned int x = pos_x; x < width; x++)
            for (unsigned int y = 0; y < height; y++)
            {
               if (index >= wstr.size())
                    goto done;

               else if (x >= pos_x && y >= pos_y)
               {
                   array_[x][y] = wstr[index];
                   index++;
               }
            }

    done:
        return;
    }
}