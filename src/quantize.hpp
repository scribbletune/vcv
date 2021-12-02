#pragma once

#include <iostream>
double quantize(std::vector<double> &scale, double freq, int L = 0, int R = 7);
std::string getNoteName(int idx);
std::string getRagaName(int idx);

struct RagaObj
{
    int len;
    int indices[18];
};

RagaObj getRagaByIdx(int idx);