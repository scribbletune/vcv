#include <iostream>
#include <vector>
#include "Brahma.hpp"

std::string ragaNames[] = {"DHAVALAMBARI", "NAMANARAYANI", "KAMAVARDHINI", "RAMAPRIYA", "GAMANASHRAMA", "VISHWAMBARI"};
std::string getRagaName(int idx)
{
  return ragaNames[idx];
}

struct RagaObj Dhavalambari = {7, {0, 3, 4, 6, 7, 10, 11}};
struct RagaObj Namanarayani = {7, {0, 2, 4, 6, 7, 10, 11}};
struct RagaObj Kamavardhini = {7, {0, 1, 4, 6, 7, 8, 11}};
struct RagaObj Ramapriya = {7, {0, 1, 4, 6, 7, 9, 10}};
struct RagaObj Gamanashrama = {7, {0, 1, 4, 6, 7, 9, 11}};
struct RagaObj Vishwambari = {7, {0, 1, 4, 6, 7, 10, 11}};

RagaObj getRagaByIdx(int idx)
{
  switch (idx)
  {
  case 0:
    return Dhavalambari;
    break;

  case 1:
    return Namanarayani;
    break;

  case 2:
    return Kamavardhini;
    break;

  case 3:
    return Ramapriya;
    break;

  case 4:
    return Gamanashrama;
    break;

  case 5:
    return Vishwambari;
    break;

  default:
    return Dhavalambari;
    break;
  }
}