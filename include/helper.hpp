// helper.hpp simply has some helper functions for the
// use to make code look more neater and readable
#pragma once
#ifndef KCONSOLE_HELPER_H
#define KCONSOLE_HELPER_H

#include <string>

namespace kconsole
{
    // name explains it
    template<typename T>
    T** make_2D_array(unsigned int width, unsigned int height)
    {
        T** array_ = new T * [width];

        for (unsigned int x = 0; x < width; x++)
            array_[x] = new T[height];

        return array_;
    }

    // name explains it
    template<typename T>
    void delete_2D_array(T** array_, unsigned int width)
    {
        for (unsigned int y = 0; y < width; y++)
        {
            delete[] array_[y];
        }

        delete[] array_;
    }

    void set_2D_wchar_array(
        wchar_t** array_,
        unsigned int width,
        unsigned int height,
        wchar_t value
    );

    void print(
        wchar_t** array_,
        unsigned int width,
        unsigned int height,
        unsigned int pos_x,
        unsigned int pos_y,
        const std::wstring& wstr
    );
}

#endif // KCONSOLE_HELPER_H