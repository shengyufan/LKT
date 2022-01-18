#include "Object.h"
#include <algorithm>
using namespace std;

Object::Object(float xdim, float ydim)
{
    x = xdim;
    y = ydim;
}

Object::Object(float xdim, float ydim, unordered_map<uint64_t, float> wK)
{
    x = xdim;
    y = ydim;
    weightedKey = wK;
}

void Object::updateWeightedKey(uint64_t k, float weight)
{
    weightedKey[k] = weight;
}