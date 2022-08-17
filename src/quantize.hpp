#pragma once

#include <iostream>
double quantize(std::vector<double> &scale, double freq, int L = 0, int R = 7);
std::string getNoteName(int idx);
float getNoteValue(int idx);
