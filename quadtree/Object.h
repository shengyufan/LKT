#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <unordered_map>
#include <stdint.h>
using namespace std;

class Object
{
public:
    float x;
    float y;
    vector<uint64_t> keys;
    unordered_map<uint64_t, float> weightedKey;

    Object(float xdim, float ydim);
    Object(float xdim, float ydim, unordered_map<uint64_t, float> wK);
    bool operator == (const Object & o) const
    {
        return o.x == x && o.y == y;
    }

    void updateWeightedKey(uint64_t k, float weight);
};

#endif