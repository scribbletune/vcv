#include <iostream>
#include "quantize.hpp"

double scale[] = {130.81, 146.83, 164.81, 174.61, 196.00, 220.00, 246.94, 261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};

double quantize(double freq, int L, int R)
{
    if (scale[L] > freq)
    {
        return scale[L];
    }

    if (scale[R] < freq)
    {
        return scale[R];
    }

    // only 1 item
    if (R == L)
    {
        return scale[L];
    }

    if (R < L)
    {
        return freq;
    }

    // only 2 items left
    if (R - L == 1)
    {
        if (freq - scale[L] < scale[R] - freq)
        {
            return scale[L];
        }
        else
        {
            return scale[R];
        }
    }

    // more than 2 items left
    int mid = L + (R - L) / 2;

    // in case the mid of the array is already a quantized value
    if (scale[mid] == freq)
    {
        return freq;
    }

    if (scale[mid] > freq)
    {
        return quantize(freq, L, mid);
    }
    else
    {
        return quantize(freq, mid, R);
    }
}