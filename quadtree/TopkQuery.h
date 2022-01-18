#ifndef TOPKQUERY_H
#define TOPKQUERY_H

#include <vector>
#include <stdint.h>
using namespace std;

class TopkQuery
{
public:
    float x;
    float y;
    vector<uint64_t> keys;
    
    TopkQuery(float xdim, float ydim, vector<uint64_t> ks);
};

#endif