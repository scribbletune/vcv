#pragma once

#include <iostream>
std::string getRagaName(int idx);

struct RagaObj
{
  int len;
  int indices[18];
};

RagaObj getRagaByIdx(int idx);