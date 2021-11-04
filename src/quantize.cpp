#include <iostream>
#include <vector>
#include "quantize.hpp"

double quantize(std::vector<double> &scale, double freq, int L, int R)
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
        return quantize(scale, freq, L, mid);
    }
    else
    {
        return quantize(scale, freq, mid, R);
    }
}

std::string noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
std::string getNoteName(int idx)
{
    return noteNames[idx];
}

std::string ragaNames[] = {"Dhavalambari", "Nāmanarayani", "Kamavardhini", "Ramapriya", "Gamanashrama", "Vishwambari"};
std::string getRagaName(int idx)
{
    return ragaNames[idx];
}